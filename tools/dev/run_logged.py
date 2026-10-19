#!/usr/bin/env python3
"""Run a shell command, keep its full output in a log file and print a short view.

Usage:
    run_logged.py [--log-dir <folder>] -c '<shell command>'
    run_logged.py [--log-dir <folder>] --head <n> -c '<shell command>'

Short view (default): the exit code, the lines that look like an error (at most 60), the last 40 lines
of the output and the path of the log. The log holds the full stdout and stderr of the command, in the
order they came, in <log folder>/<time>-<command name>.log. The log folder is
/Users/max/Projects/cm0102/run-logs, or RUN_LOGS_DIR, or --log-dir. The exit code is the exit code of
the command (128 + n when a signal n ended it).

--head n (the grep and rg cap): stdout shows its first n lines and then the line "N more lines: narrow
the search or use -l". The cut text goes to a log file, and the line after it names the file. stderr
passes through. The exit code is the exit code of the command.

Python 3.9 or later, no packages. A hook (.claude/hooks/bash_rewrite.py) calls this script for the
commands that it knows (docs/specs/2026-10-19-token-saving-spec.md, slice 1).
"""
import os
import re
import shlex
import signal
import subprocess
import sys
import time

DEFAULT_LOG_DIR = "/Users/max/Projects/cm0102/run-logs"
MAX_ERROR_LINES = 60
TAIL_LINES = 40
LINE_WIDTH = 300      # a printed line is cut to this many characters (the log keeps the full line)

# Lines that show a failure. A line that matches any of these is printed in the error list.
ERROR_PATTERNS = [
    r"\berror:",                                  # clang, gcc, and -Werror warnings
    r"\bfatal error\b",
    r"\bFAILED\b",                                # ninja, ctest, test scripts
    r"\bFAIL\b",                                  # smoke_run.py, rules_report_check.py: "FAIL  name: ..."
    r"\*\*\*\s*(Failed|Exception|Timeout|Not Run|\[)",   # ctest, make
    r"^Traceback \(most recent call last\)",
    r"^\w[\w.]*(Error|Exception|Exit|Interrupt)\b",    # the last line of a Python traceback
    r"^\s+File \".*\", line \d+",                 # a frame of a Python traceback
    r"\bAssertion\b.*\bfailed\b|\bAssertionError\b|\bassert(ion)? failed\b",
    r"\bdiffers?\b.*\bsave\b|\bsave\b.*\bdiffers?\b",   # the same-save check
    r"recomp: message box",                       # the runtime prints every error box of the game
    r"\b[A-Za-z_][\w]*\.\.?cpp \d+\b",            # an error box of the original: "Database.cpp 13471"
    r"^\s*\d+ - \S.* \((Failed|Timeout|Not Run|SEGFAULT|SIGSEGV|SIGABRT|Subprocess|Child|Exception|Interrupted|Illegal)[^)]*\)\s*$",
    r"^CMake Error",
    r"^make(\[\d+\])?: \*\*\*",
    r"^ninja: build stopped",
    r"^Undefined symbols|^ld: |linker command failed|clang: error|collect2: error",
    r"No rule to make target|No such file or directory|command not found|Permission denied",
    r"Segmentation fault|Bus error|Abort trap|core dumped|\bKilled\b|Illegal instruction",
    r"^fatal:|^panic:",
    r"[1-9]\d* tests? failed|\bTimeout\b",
]
ERROR_RE = re.compile("|".join(f"(?:{p})" for p in ERROR_PATTERNS))
WARNING_RE = re.compile(r"\bwarning:")


def command_name(command):
    """A short file name part for a shell command: 'smoke_run-same-save', 'cmake-build', 'ctest'."""
    try:
        words = shlex.split(command, comments=True)
    except ValueError:
        words = command.split()
    words = [w for w in words if not re.match(r"^[A-Za-z_]\w*=", w)]
    if not words:
        return "command"
    first = os.path.basename(words[0])
    parts = [first]
    if re.match(r"^(python|python3|python\d\.\d+|bash|sh|zsh)$", first):
        script = next((w for w in words[1:] if not w.startswith("-")), None)
        if script:
            parts = [os.path.splitext(os.path.basename(script))[0]]
    parts += [w.lstrip("-") for w in words[1:] if w.startswith("--") and len(w) > 2][:2]
    name = re.sub(r"[^A-Za-z0-9._-]+", "-", "-".join(parts)).strip("-")
    return name[:60] or "command"


def log_path(log_dir, command):
    os.makedirs(log_dir, exist_ok=True)
    stamp = time.strftime("%Y%m%d-%H%M%S")
    base = os.path.join(log_dir, f"{stamp}-{command_name(command)}")
    path, n = base + ".log", 1
    while True:      # one file per run, also when two runs start in the same second
        try:
            fd = os.open(path, os.O_WRONLY | os.O_CREAT | os.O_EXCL, 0o644)
            return path, os.fdopen(fd, "wb")
        except FileExistsError:
            n += 1
            path = f"{base}-{n}.log"


def shell():
    sh = os.environ.get("SHELL", "")
    return sh if os.path.basename(sh) in ("bash", "zsh") and os.path.exists(sh) else "/bin/bash"


def start(command, stdout, stderr):
    return subprocess.Popen([shell(), "-c", command], stdout=stdout, stderr=stderr)


def forward_signals(child):
    def handler(signum, frame):
        child.send_signal(signum)
    for s in (signal.SIGINT, signal.SIGTERM, signal.SIGHUP):
        signal.signal(s, handler)


def exit_code(child):
    code = child.returncode
    return 128 - code if code < 0 else code


def clip(text):
    text = text.rstrip("\r\n")
    if "\r" in text:                                   # progress bars: show the last state
        text = next((p for p in reversed(text.split("\r")) if p), "")
    if len(text) > LINE_WIDTH:
        text = text[:LINE_WIDTH] + f"... [+{len(text) - LINE_WIDTH} chars]"
    return text


def run_short(command, path, log):
    start_time = time.time()
    child = start(command, subprocess.PIPE, subprocess.STDOUT)
    forward_signals(child)
    tail, matches, warnings, count = [], [], 0, 0
    with log:
        for raw in child.stdout:
            log.write(raw)
            log.flush()
            count += 1
            line = raw.decode("utf-8", "replace")
            tail.append(line)
            if len(tail) > TAIL_LINES:
                tail.pop(0)
            if ERROR_RE.search(line):
                matches.append((count, line))
            elif WARNING_RE.search(line):
                warnings += 1
    child.wait()
    code = exit_code(child)
    out = [f"exit code: {code}  ({time.time() - start_time:.0f} s, {count} lines of output)"]
    if matches:
        shown = matches
        if len(matches) > MAX_ERROR_LINES:       # the first failure and the last one both matter
            half = MAX_ERROR_LINES // 2
            shown = matches[:half] + [None] + matches[-half:]
        out.append(f"error lines ({len(matches)}" + (f", the first {MAX_ERROR_LINES // 2} and the last "
                   f"{MAX_ERROR_LINES // 2} shown" if len(matches) > MAX_ERROR_LINES else "") + "):")
        for item in shown:
            out.append(f"  ...{len(matches) - MAX_ERROR_LINES} more in the log" if item is None
                       else f"  L{item[0]}: {clip(item[1])}")
    elif code != 0:
        out.append("error lines: none matched. Read the last lines, or open the log.")
    if warnings:
        out.append(f"warnings: {warnings} lines with 'warning:' (in the log)")
    out.append(f"last {len(tail)} lines:")
    out += [f"  {clip(line)}" for line in tail]
    out.append(f"log: {path}")
    print("\n".join(out))
    return code


def run_head(command, log_dir, limit):
    """stdout: the first `limit` lines, then a count of the rest. stderr passes through."""
    child = start(command, subprocess.PIPE, None)
    forward_signals(child)
    shown, rest, log, path = 0, 0, None, None
    first = []
    try:
        for raw in child.stdout:
            if shown < limit:
                shown += 1
                first.append(raw)
                sys.stdout.write(raw.decode("utf-8", "replace"))
                continue
            if log is None and path is None:     # the output is too long: keep all of it in a file
                try:
                    path, log = log_path(log_dir, command)
                    log.writelines(first)
                except OSError:
                    path = "(the log folder cannot be written)"
            rest += 1
            if log:
                log.write(raw)
    finally:
        if log:
            log.close()
    child.wait()
    sys.stdout.flush()
    if rest:
        print(f"{rest} more lines: narrow the search or use -l")
        print(f"full output: {path}")
    return exit_code(child)


def main(argv):
    log_dir = os.environ.get("RUN_LOGS_DIR", DEFAULT_LOG_DIR)
    head = None
    args = list(argv[1:])
    while args and args[0] != "-c":
        flag = args.pop(0)
        if flag == "--log-dir" and args:
            log_dir = args.pop(0)
        elif flag == "--head" and args:
            head = int(args.pop(0))
        else:
            print(__doc__)
            return 2
    if len(args) != 2:
        print(__doc__)
        return 2
    command = args[1]
    if head:
        return run_head(command, log_dir, head)
    try:
        path, log = log_path(log_dir, command)
    except OSError as e:     # the log folder cannot be written: run the command as it is
        print(f"run_logged: no log ({e}); running the command without it", file=sys.stderr)
        return subprocess.call([shell(), "-c", command])
    return run_short(command, path, log)


if __name__ == "__main__":
    sys.exit(main(sys.argv))
