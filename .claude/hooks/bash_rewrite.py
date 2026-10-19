#!/usr/bin/env python3
"""PreToolUse hook on Bash: send known commands through tools/dev/run_logged.py.

Reads the JSON that Claude Code sends on stdin. When it can rewrite the command safely it prints
{"hookSpecificOutput": {"hookEventName": "PreToolUse", "updatedInput": {...}}} and exits 0. In every
other case it prints nothing and exits 0, so the command runs as it was written.

Rewritten (docs/specs/2026-10-19-token-saving-spec.md, Q2 to Q4):
    cmake (build, configure), ctest, make, ninja, tests/**/*_run.py, *_check.py, smoke_run.py,
    make_release.sh, tools/recomp/recomp.py      -> run_logged.py -c '<command>'  (short view, full log)
    grep, egrep, fgrep, rg                       -> run_logged.py --head 100 -c '<command>'
Left alone: a command that starts with RAW=1, an unknown command, a grep that has -l, -L, -c, -m,
--count or --files-with-matches, and every command that this script cannot read with certainty
(a pipe, a background &, a subshell, a heredoc, $( ) or a backtick, a redirection into a file).
Only the known part of a compound command (&& || ; and new lines) is wrapped: "cd build && ctest" keeps
its cd.

Python 3.9 or later, no packages.
"""
import json
import os
import re
import shlex
import sys

WRAPPER = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))),
                       "tools", "dev", "run_logged.py")
GREP_CAP = 100

SHELL_WORDS = {"if", "then", "else", "elif", "fi", "for", "while", "until", "do", "done", "case", "esac",
               "in", "function", "select", "!", "{", "}", "[[", "exec", "eval", "source", ".",
               "export", "unset", "alias", "cd", "set", "trap", "wait", "sudo", "xargs", "command", "builtin"}
ASSIGN_RE = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*=")
PYTHON_RE = re.compile(r"^python(\d+(\.\d+)*)?$")
SCRIPT_RE = re.compile(r"(_run\.py|_check\.py|^recomp\.py|^smoke_run\.py|^make_release\.sh)$")


class Unsafe(Exception):
    """The command has a construct that this script does not rewrite."""


def split_command(command):
    """Cut a shell command at the top-level && || ; and new lines.

    Returns a list of (start, end, separator-free text) spans of the segments. Raises Unsafe for a
    construct that could change what a rewrite means."""
    segments, i, n, seg_start = [], 0, len(command), 0
    word_start = True            # at the start of a word: a # here starts a comment
    cut = n                      # the end of the command text before a comment on the last line

    def close(end):
        if command[seg_start:end].strip():
            segments.append((seg_start, end))

    while i < n:
        c = command[i]
        if c == "\\":
            i += 2
            word_start = False
            continue
        if c == "'":
            j = command.find("'", i + 1)
            if j < 0:
                raise Unsafe("open quote")
            i, word_start = j + 1, False
            continue
        if c == '"':
            i += 1
            while i < n and command[i] != '"':
                if command[i] == "\\":
                    i += 1
                elif command[i] == "`" or command.startswith("$(", i):
                    raise Unsafe("substitution")
                i += 1
            if i >= n:
                raise Unsafe("open quote")
            i, word_start = i + 1, False
            continue
        if c == "`" or command.startswith("$(", i) or command.startswith("<(", i) or command.startswith(">(", i):
            raise Unsafe("substitution")
        if command.startswith("<<", i):
            raise Unsafe("heredoc")
        if c == "#" and word_start:
            j = command.find("\n", i)
            end = n if j < 0 else j
            close(i)
            seg_start = end
            i = end
            continue
        if c in "()":
            raise Unsafe("subshell")
        if c == "\n" or c == ";":
            close(i)
            seg_start = i + 1
            i += 1
            word_start = True
            continue
        if command.startswith("&&", i) or command.startswith("||", i):
            close(i)
            seg_start = i + 2
            i += 2
            word_start = True
            continue
        if c == "|":
            raise Unsafe("pipe")
        if c == "&":
            if command[i + 1:i + 2] == ">" or command[i - 1:i] in (">", "<"):   # &> file, 2>&1
                i += 1
                word_start = False
                continue
            raise Unsafe("background")
        word_start = c in " \t"
        i += 1
    close(cut)
    return segments


def words_of(text):
    try:
        return shlex.split(text, comments=True)
    except ValueError:
        raise Unsafe("unreadable segment")


def has_redirect(text):
    """True when the segment has an unquoted > or < other than 2>&1 (a redirection into or from a file)."""
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if c == "\\":
            i += 2
            continue
        if c == "'":
            i = text.find("'", i + 1) + 1 or n
            continue
        if c == '"':
            i += 1
            while i < n and text[i] != '"':
                i += 2 if text[i] == "\\" else 1
            i += 1
            continue
        if c in "<>":
            m = re.match(r">&(\d+|-)|>\s*/dev/null\b", text[i:])     # 2>&1, >&2, > /dev/null: no file written
            if m:
                i += m.end()
                continue
            return True
        i += 1
    return False


def classify(text):
    """'run' for a build or test command, 'head' for grep and rg, None for anything else."""
    words = words_of(text)
    if not words or has_redirect(text):
        return None
    if words[0] == "RAW=1":
        return None
    while words and ASSIGN_RE.match(words[0]):
        if words[0].startswith("RAW="):
            return None
        words = words[1:]
    while words and (words[0] in ("time", "nohup") or words[0] == "env" or
                     (words[0] == "timeout" and len(words) > 2 and re.match(r"^\d+[smhd]?$", words[1]))):
        if words[0] == "timeout":
            words = words[2:]
        else:
            words = words[1:]
        while words and ASSIGN_RE.match(words[0]):
            if words[0].startswith("RAW="):
                return None
            words = words[1:]
    if not words or words[0] in SHELL_WORDS:
        return None
    first, rest = os.path.basename(words[0]), words[1:]
    if any(w in ("--help", "--version") for w in rest):
        return None
    if first in ("cmake", "ctest", "make", "gmake", "ninja"):
        if first == "cmake" and ("-E" in rest or "--system-information" in rest):
            return None
        return "run"
    if first in ("grep", "egrep", "fgrep", "rg"):
        return None if grep_limited(first, rest) else "head"
    if PYTHON_RE.match(first) or first in ("bash", "sh", "zsh"):
        script, skip = None, False
        for w in rest:
            if skip:
                skip = False
            elif w in ("-m", "-c"):
                return None
            elif w in ("-X", "-W", "-Q", "-O"):
                skip = True
            elif not w.startswith("-"):
                script = w
                break
        if script and SCRIPT_RE.search(os.path.basename(script)):
            return "run"
        return None
    if SCRIPT_RE.search(first):
        return "run"
    return None


def grep_limited(tool, args):
    """True when the grep already limits its output (-l -L -c -m ...), so its size is the agent's choice."""
    for a in args:
        if a == "--":
            break
        if a in ("--count", "--files-with-matches", "--files-without-match", "--max-count", "--files"):
            return True
        if a.startswith("--max-count=") or a.startswith("--count"):
            return True
        if a.startswith("-") and not a.startswith("--") and len(a) > 1:
            for ch in a[1:]:       # a cluster such as -rl, -c, -m5, -nic; a letter that takes a value ends it
                if ch in "lcm" or (ch == "L" and tool != "rg"):
                    return True
                if ch in "efABCdDtTgjME" or (ch == "r" and tool == "rg"):
                    break
    return False


STATE_WORDS = {"set", "declare", "typeset", "local", "unset", "source", ".", "alias", "function", "eval",
               "shopt", "setopt", "unsetopt", "readonly", "trap"}


# A loop, an if or a function body: a variable of the loop is not exported, so the whole command stays as it is.
BLOCK_WORDS = {"for", "while", "until", "if", "then", "else", "elif", "fi", "do", "done", "case", "esac",
               "select", "{", "}", "[[", "!"}


def changes_shell_state(text):
    """True for a segment that sets a shell variable or option. The wrapped segment runs in a child
    shell, where a variable that is not exported is missing, so the whole command stays as it is."""
    try:
        words = words_of(text.strip())
    except Unsafe:
        return True
    return bool(words) and (all(ASSIGN_RE.match(w) for w in words) or words[0] in STATE_WORDS
                            or words[0] in BLOCK_WORDS)


def rewrite(command):
    """The new command, or None when nothing changes."""
    if re.match(r"\s*RAW=1(\s|$)", command):
        return None
    if not os.path.exists(WRAPPER):
        return None
    try:
        segments = split_command(command)
    except Unsafe:
        return None
    for start, end in segments:
        if changes_shell_state(command[start:end]):
            return None
    out, pos, changed = [], 0, False
    for start, end in segments:
        text = command[start:end]
        try:
            kind = classify(text.strip())
        except Unsafe:
            kind = None
        if not kind:
            continue
        lead = len(text) - len(text.lstrip())
        body = text.strip()
        flag = f"--head {GREP_CAP} " if kind == "head" else ""
        wrapped = f"python3 -I {shlex.quote(WRAPPER)} {flag}-c {shlex.quote(body)}"
        out.append(command[pos:start + lead] + wrapped)
        pos = start + lead + len(body)
        changed = True
    if not changed:
        return None
    return "".join(out) + command[pos:]


def main():
    try:
        data = json.load(sys.stdin)
        if data.get("tool_name") != "Bash":
            return 0
        tool_input = data.get("tool_input") or {}
        command = tool_input.get("command")
        if not isinstance(command, str):
            return 0
        new = rewrite(command)
        if new is None:
            return 0
        updated = dict(tool_input, command=new)
        print(json.dumps({"hookSpecificOutput": {"hookEventName": "PreToolUse", "updatedInput": updated}}))
    except Exception:       # a hook that fails must never stop a command
        return 0
    return 0


if __name__ == "__main__":
    sys.exit(main())
