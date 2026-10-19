#!/usr/bin/env python3
"""Test of the token-saving hooks and the log wrapper (docs/specs/2026-10-19-token-saving-spec.md, slice 1).

Usage: python3 -I tests/dev/test_hooks.py

Feeds the hooks (.claude/hooks/bash_rewrite.py, read_shrink.py) the JSON that Claude Code sends and checks
the command or path they return. Then runs the wrapper (tools/dev/run_logged.py) on small sample commands
and checks the error lines, the log and the exit code. Nothing here touches the game or the real run logs:
the logs go to a temporary folder (RUN_LOGS_DIR).
"""
import json
import os
import shlex
import struct
import subprocess
import sys
import tempfile
import zlib

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
BASH_HOOK = os.path.join(ROOT, ".claude", "hooks", "bash_rewrite.py")
READ_HOOK = os.path.join(ROOT, ".claude", "hooks", "read_shrink.py")
WRAPPER = os.path.join(ROOT, "tools", "dev", "run_logged.py")

failures = []


def check(ok, what, detail=""):
    if not ok:
        failures.append(what)
        print(f"FAIL  {what}" + (f"\n      {detail}" if detail else ""))


def run_hook(hook, tool_name, tool_input, env=None):
    event = {"session_id": "t", "cwd": ROOT, "hook_event_name": "PreToolUse", "tool_name": tool_name,
             "tool_input": tool_input, "tool_use_id": "toolu_test"}
    r = subprocess.run([sys.executable, "-I", hook], input=json.dumps(event), capture_output=True, text=True,
                       env=dict(os.environ, **(env or {})))
    check(r.returncode == 0, f"{os.path.basename(hook)} exits 0 for {tool_input}", r.stderr)
    if not r.stdout.strip():
        return None
    out = json.loads(r.stdout)["hookSpecificOutput"]
    check(out["hookEventName"] == "PreToolUse", "hook output names PreToolUse")
    return out


def rewritten(command, **extra):
    out = run_hook(BASH_HOOK, "Bash", dict({"command": command}, **extra))
    return None if out is None else out["updatedInput"]


# ---- the Bash hook: the command it returns -------------------------------------------------------------
def wrapped(command):
    return f"python3 -I {shlex.quote(WRAPPER)} -c {shlex.quote(command)}"


def capped(command):
    return f"python3 -I {shlex.quote(WRAPPER)} --head 100 -c {shlex.quote(command)}"


CASES = [
    # (command, expected rewritten command or None for "unchanged")
    ("cmake --build build -j8", wrapped("cmake --build build -j8")),
    ("cmake -S . -B build -DCM0102_EXE=/x/cm0102.exe", wrapped("cmake -S . -B build -DCM0102_EXE=/x/cm0102.exe")),
    ("ctest --test-dir build --output-on-failure", wrapped("ctest --test-dir build --output-on-failure")),
    ("make -j4", wrapped("make -j4")),
    ("ninja -C build", wrapped("ninja -C build")),
    ("python3 tests/smoke/smoke_run.py build/cm0102_recomp /g --same-save --save start.sav",
     wrapped("python3 tests/smoke/smoke_run.py build/cm0102_recomp /g --same-save --save start.sav")),
    ("python3 tests/seasons/rules_report_check.py build/p /g", wrapped("python3 tests/seasons/rules_report_check.py build/p /g")),
    ("tests/smoke/foreign_run.py a b", wrapped("tests/smoke/foreign_run.py a b")),
    ("/Users/max/.venvs/cm0102/bin/python tools/recomp/recomp.py --exe x --out y",
     wrapped("/Users/max/.venvs/cm0102/bin/python tools/recomp/recomp.py --exe x --out y")),
    ("bash tools/release/make_release.sh --dry-run", wrapped("bash tools/release/make_release.sh --dry-run")),
    ("tools/release/make_release.sh", wrapped("tools/release/make_release.sh")),
    ("FOO=1 ctest -R x", wrapped("FOO=1 ctest -R x")),
    # compound commands: only the known part is wrapped
    ("cd build && ctest -j4", "cd build && " + wrapped("ctest -j4")),
    ("cd build && cmake --build . && ctest", "cd build && " + wrapped("cmake --build .") + " && " + wrapped("ctest")),
    ("make; echo done", wrapped("make") + "; echo done"),
    ("make || echo failed", wrapped("make") + " || echo failed"),
    ("cmake --build build  # the build", wrapped("cmake --build build") + "  # the build"),
    ("echo a\nmake -j2\necho b", "echo a\n" + wrapped("make -j2") + "\necho b"),
    ("cmake --build build 2>&1", wrapped("cmake --build build 2>&1")),
    # grep and rg: capped at 100 lines
    ("grep -rn foo src", capped("grep -rn foo src")),
    ("rg 'foo bar' src tests", capped("rg 'foo bar' src tests")),
    ("grep -n foo f 2>/dev/null", capped("grep -n foo f 2>/dev/null")),
    ("cd src && grep -rn foo .", "cd src && " + capped("grep -rn foo .")),
    # left alone
    ("RAW=1 cmake --build build", None),
    ("RAW=1 grep -rn foo src", None),
    ("cd build && RAW=1 ctest", None),
    ("ls -la", None),
    ("git status", None),
    ("python3 -c 'print(1)'", None),
    ("python3 tools/seasons/make_db_table.py --check", None),
    ("cmake -E copy a b", None),
    ("cmake --version", None),
    ("make --help", None),
    ("grep -rl foo src", None),
    ("grep -rL foo src", None),
    ("grep -c foo f", None),
    ("grep -rc foo src", None),
    ("grep -m 5 foo f", None),
    ("grep -m5 foo f", None),
    ("grep --count foo f", None),
    ("grep --files-with-matches foo f", None),
    ("rg -l foo", None),
    ("rg --count foo", None),
    ("rg -c foo", None),
    ("grep foo f | head -5", None),
    ("grep foo f | sort", None),
    ("rg foo | wc -l", None),
    ("cmake --build build | tail -20", None),
    ("cmake --build build 2>&1 | tail -20", None),
    ("ctest && grep foo f | head", None),
    ("make &", None),
    ("make -j4 > build.log", None),
    ("grep foo f > out.txt", None),
    ("echo 'make && ctest'", None),
    ('echo "cmake --build x"', None),
    ("(cd build && ctest)", None),
    ("x=$(cmake --build build)", None),
    ("echo `ctest`", None),
    ("cat <<EOF\nmake\nEOF", None),
    ("for f in a b; do make $f; done", None),
    ("if make; then echo ok; fi", None),
    ("VAR=1; ctest -R $VAR", None),
    ("export A=1 && ctest", "export A=1 && " + wrapped("ctest")),
    ("set -e; make", None),
    ("make 'unclosed", None),
]


def test_bash_hook_cases():
    for command, want in CASES:
        got = rewritten(command)
        if want is None:
            check(got is None, f"unchanged: {command!r}", f"got {got}")
        else:
            check(got is not None and got["command"] == want, f"rewritten: {command!r}",
                  f"want {want!r}\n      got  {got}")


def test_bash_hook_keeps_other_fields():
    got = rewritten("ctest -j4", description="Run tests", timeout=600000, run_in_background=True)
    check(got is not None and got.get("timeout") == 600000 and got.get("run_in_background") is True
          and got.get("description") == "Run tests", "the other fields of tool_input stay", str(got))
    # other tools and bad input: no output, exit 0
    check(run_hook(BASH_HOOK, "Edit", {"command": "make"}) is None, "the hook ignores other tools")
    r = subprocess.run([sys.executable, "-I", BASH_HOOK], input="not json", capture_output=True, text=True)
    check(r.returncode == 0 and not r.stdout, "bad JSON: no output, exit 0")
    r = subprocess.run([sys.executable, "-I", BASH_HOOK], input=json.dumps({"tool_name": "Bash", "tool_input": {}}),
                       capture_output=True, text=True)
    check(r.returncode == 0 and not r.stdout, "no command: no output, exit 0")


# ---- the wrapper --------------------------------------------------------------------------------------
def write(folder, name, text):
    path = os.path.join(folder, name)
    with open(path, "w") as f:
        f.write(text)
    return path


def run_wrapper(command, logs, *flags):
    env = dict(os.environ, RUN_LOGS_DIR=logs)
    r = subprocess.run([sys.executable, "-I", WRAPPER, *flags, "-c", command], capture_output=True, text=True, env=env)
    return r


def log_of(r):
    for line in r.stdout.splitlines():
        if line.startswith("log: "):
            return line[5:]
    return None


def test_wrapper(work, logs):
    noise = "'compiling a file of the game ...'"
    # 1. a compiler error in the middle of 5,000 lines
    p = write(work, "build.py", f"""
import sys
for i in range(2500):
    print("[%d%%] Building C object gen/fn_%04d.c.o" % (i // 25, i))
print("/src/win/season.c:42:7: error: use of undeclared identifier 'x'", file=sys.stderr)
print("  42 |   x = 1;", file=sys.stderr)
for i in range(2500):
    print("[%d%%] Building C object gen/fn_%04d.c.o" % (i // 25, i))
print("make[2]: *** [CMakeFiles/cm0102_recomp.dir/season.c.o] Error 1")
sys.exit(2)
""")
    r = run_wrapper(f"{shlex.quote(sys.executable)} {p}", logs)
    check(r.returncode == 2, "compiler error: exit code kept", f"{r.returncode}")
    check("exit code: 2" in r.stdout, "compiler error: exit code printed")
    check("season.c:42:7: error: use of undeclared identifier 'x'" in r.stdout, "compiler error: error line shown")
    check("*** [CMakeFiles/cm0102_recomp.dir/season.c.o] Error 1" in r.stdout, "compiler error: make line shown")
    check(len(r.stdout.splitlines()) < 70, "compiler error: short view", f"{len(r.stdout.splitlines())} lines")
    lg = log_of(r)
    check(lg and os.path.exists(lg) and lg.startswith(logs), "compiler error: log path printed and file exists", str(lg))
    if lg and os.path.exists(lg):
        text = open(lg).read()
        check(text.count("\n") == 5003 and "use of undeclared identifier" in text, "compiler error: log has the full output",
              str(text.count("\n")))
        check(os.path.basename(lg).endswith("-build.log") or "build" in os.path.basename(lg), "log name has the command name", lg)
    # 2. a Python traceback
    p = write(work, "boom.py", """
def f():
    raise ValueError("boom in the middle")
for i in range(300):
    print("line", i)
f()
""")
    r = run_wrapper(f"{shlex.quote(sys.executable)} {p}", logs)
    check(r.returncode == 1, "traceback: exit code kept", f"{r.returncode}")
    check("Traceback (most recent call last)" in r.stdout and "ValueError: boom in the middle" in r.stdout,
          "traceback: header and exception shown", r.stdout)
    check('boom.py", line' in r.stdout, "traceback: frame shown")
    # 3. a same-save difference, as smoke_run.py prints it
    p = write(work, "same_save_run.py", """
import sys
print("run   normal: saved after 14 game days in 71 s, idle sleeps 12")
print("run   faster: saved after 14 game days in 70 s, idle sleeps 11")
print("ok    again: the save is the same as normal")
print("FAIL  faster: the save differs from normal in: general.dat, staff history.tmp")
print("FAIL")
sys.exit(1)
""")
    r = run_wrapper(f"{shlex.quote(sys.executable)} {p}", logs)
    check(r.returncode == 1, "same-save difference: exit code kept")
    check("FAIL  faster: the save differs from normal in: general.dat, staff history.tmp" in r.stdout,
          "same-save difference: the differing parts shown", r.stdout)
    for line in ("FAIL  normal: no saved game (code 3) after 120 s", "FAILED: 2", "FAIL  the run wrote no report"):
        q = write(work, "f.py", f"import sys\nprint({line!r})\nsys.exit(1)\n")
        r = run_wrapper(f"{shlex.quote(sys.executable)} {q}", logs)
        check(f"error lines" in r.stdout and line in r.stdout.split("last ")[0], f"failure line shown in the error list: {line}", r.stdout)
    # 4. a game error box
    p = write(work, "box.py", """
import sys
for i in range(400):
    print("recomp: tick", i, file=sys.stderr)
print('recomp: message box "Error": Database.cpp 13471', file=sys.stderr)
for i in range(400):
    print("recomp: tock", i, file=sys.stderr)
sys.exit(3)
""")
    r = run_wrapper(f"{shlex.quote(sys.executable)} {p}", logs)
    check(r.returncode == 3, "error box: exit code kept")
    check('Database.cpp 13471' in r.stdout.split("last ")[0], "error box: shown in the error list", r.stdout)
    p = write(work, "box2.py", """
print("the game stops at the box Database..cpp 13126")
print("news..cpp 7797")
""")
    r = run_wrapper(f"{shlex.quote(sys.executable)} {p}", logs)
    check("Database..cpp 13126" in r.stdout.split("last ")[0] and "news..cpp 7797" in r.stdout.split("last ")[0],
          "error box with two dots: shown", r.stdout)
    # 5. ctest and assert lines
    p = write(work, "ctest.py", """
import sys
print("Test #4: seasons_db ...***Failed  0.12 sec")
print("98% tests passed, 1 tests failed out of 50")
print("The following tests FAILED:")
print("\\t  4 - seasons_db (Failed)")
print("Assertion failed: (n > 0), function f, file x.c, line 9.")
sys.exit(8)
""")
    r = run_wrapper(f"{shlex.quote(sys.executable)} {p}", logs)
    head = r.stdout.split("last ")[0]
    check(r.returncode == 8 and "***Failed" in head and "The following tests FAILED" in head
          and "4 - seasons_db (Failed)" in head and "Assertion failed" in head and "1 tests failed" in head,
          "ctest and assert lines shown", r.stdout)
    # 6. a success
    p = write(work, "ok.py", """
for i in range(100):
    print("test", i, "ok")
print("PASS")
""")
    r = run_wrapper(f"{shlex.quote(sys.executable)} {p}", logs)
    check(r.returncode == 0 and "exit code: 0" in r.stdout and "error lines" not in r.stdout, "success: exit 0, no error lines", r.stdout)
    check("test 99 ok" in r.stdout and "test 50 ok" not in r.stdout, "success: last 40 lines only", r.stdout)
    r = run_wrapper("echo 100% tests passed, 0 tests failed out of 5", logs)
    check("error lines" not in r.stdout, "'0 tests failed' is not an error line", r.stdout)
    # 7. a failure with no known pattern: the short view says so
    r = run_wrapper("echo something odd; exit 5", logs)
    check(r.returncode == 5 and "none matched" in r.stdout and "something odd" in r.stdout, "unmatched failure says so", r.stdout)
    # 8. a signal keeps a failing code
    r = run_wrapper("kill -9 $$", logs)
    check(r.returncode == 137 and "exit code: 137" in r.stdout, "signal: 128 + n", f"{r.returncode}\n{r.stdout}")
    # 9. more than 60 error lines: the first and the last are shown
    p = write(work, "many.py", """
for i in range(200):
    print("a.c:%d:1: error: first kind %d" % (i, i))
print("z.c:1:1: error: the last error")
""")
    r = run_wrapper(f"{shlex.quote(sys.executable)} {p}", logs)
    check("first kind 0" in r.stdout and "the last error" in r.stdout and "more in the log" in r.stdout
          and "first kind 100" not in r.stdout.split("last ")[0], "many errors: first and last shown, count named", r.stdout[:600])
    # 10. a very long line is cut in the view, not in the log
    r = run_wrapper(f"{shlex.quote(sys.executable)} -c \"print('x.c:1: error: ' + 'y' * 5000)\"", logs)
    check(len(r.stdout) < 3000 and "chars]" in r.stdout, "long line cut in the view", str(len(r.stdout)))
    check(os.path.getsize(log_of(r)) > 5000, "long line complete in the log")
    # 11. the command's own stdin, working directory and environment work as before
    r = run_wrapper("cd / && pwd && echo $RUN_LOGS_DIR", logs)
    check("\n  /\n" in r.stdout and logs in r.stdout, "cd, pwd and environment inside the command", r.stdout)
    # 12. warnings are counted
    r = run_wrapper("echo 'a.c:1:1: warning: unused'; echo 'b.c:1:1: warning: unused'", logs)
    check("warnings: 2" in r.stdout, "warnings counted", r.stdout)


def test_head(work, logs):
    p = write(work, "lines.txt", "".join(f"match {i}\n" for i in range(250)))
    r = run_wrapper(f"grep match {p}", logs, "--head", "100")
    lines = r.stdout.splitlines()
    check(r.returncode == 0, "head: exit code of grep kept")
    check(lines[:100] == [f"match {i}" for i in range(100)], "head: first 100 lines")
    check(lines[100] == "150 more lines: narrow the search or use -l", "head: the count line", str(lines[100:102]))
    check(lines[101].startswith("full output: ") and os.path.exists(lines[101][13:]), "head: the log path is named")
    if lines[101].startswith("full output: "):
        check(open(lines[101][13:]).read() == "".join(f"match {i}\n" for i in range(250)), "head: the log has all lines")
    r = run_wrapper(f"grep match {p}", logs, "--head", "1000")
    check(r.stdout.count("\n") == 250 and "more lines" not in r.stdout, "head: short output unchanged")
    r = run_wrapper(f"grep nomatch {p}", logs, "--head", "100")
    check(r.returncode == 1 and r.stdout == "", "head: no match gives exit code 1 and no output")
    r = run_wrapper(f"grep match /nonexistent/file", logs, "--head", "100")
    check(r.returncode == 2 and "No such file" in r.stderr, "head: stderr passes through, exit code 2", r.stderr)


def test_end_to_end(work, logs):
    """The command that the hook returns runs in a shell and gives the exit code and the short view."""
    p = write(work, "x_check.py", "import sys\nprint('FAIL  something: broke')\nsys.exit(4)\n")
    got = rewritten(f"cd {shlex.quote(work)} && python3 x_check.py && echo not-reached")
    check(got is not None, "end to end: hook rewrote the command")
    if got:
        r = subprocess.run(["/bin/bash", "-c", got["command"]], capture_output=True, text=True,
                           env=dict(os.environ, RUN_LOGS_DIR=logs))
        check(r.returncode == 4 and "FAIL  something: broke" in r.stdout and "not-reached" not in r.stdout,
              "end to end: failure stops the && chain with its exit code", f"{r.returncode}\n{r.stdout}")
    q = write(work, "y_check.py", "print('fine')\n")
    got = rewritten(f"cd {shlex.quote(work)} && python3 y_check.py && echo reached")
    r = subprocess.run(["/bin/bash", "-c", got["command"]], capture_output=True, text=True, env=dict(os.environ, RUN_LOGS_DIR=logs))
    check(r.returncode == 0 and "reached" in r.stdout and "exit code: 0" in r.stdout, "end to end: success continues the chain", r.stdout)
    # RAW=1 gives the full output
    got = rewritten("RAW=1 python3 y_check.py")
    check(got is None, "RAW=1: no rewrite")
    # a grep through the hook keeps its exit code in a && chain
    t = write(work, "t.txt", "a\n")
    got = rewritten(f"grep zzz {t} || echo none")
    r = subprocess.run(["/bin/bash", "-c", got["command"]], capture_output=True, text=True, env=dict(os.environ, RUN_LOGS_DIR=logs))
    check(r.stdout.strip() == "none", "end to end: grep no match still runs the || branch", r.stdout)


# ---- the Read hook --------------------------------------------------------------------------------------
def make_png(path, width, height):
    def chunk(kind, data):
        body = kind + data
        return struct.pack(">I", len(data)) + body + struct.pack(">I", zlib.crc32(body) & 0xFFFFFFFF)
    rows = b"".join(b"\x00" + bytes(((x * 5) % 256, (y * 9) % 256, 128)[i] for x in range(width) for i in range(3))
                    for y in range(height))
    data = b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 2, 0, 0, 0)) + \
        chunk(b"IDAT", zlib.compress(rows)) + chunk(b"IEND", b"")
    with open(path, "wb") as f:
        f.write(data)


def png_width(path):
    with open(path, "rb") as f:
        return struct.unpack(">I", f.read(24)[16:20])[0]


def test_read_hook(work, logs):
    env = {"RUN_LOGS_DIR": logs, "CM0102_ROOT": work}
    wide = os.path.join(work, "wide.png")
    narrow = os.path.join(work, "narrow.png")
    full = os.path.join(work, "wide-full.png")
    make_png(wide, 1600, 20)
    make_png(narrow, 1000, 20)
    make_png(full, 1600, 20)
    exact = os.path.join(work, "exact.png")
    make_png(exact, 1024, 20)
    check(run_hook(READ_HOOK, "Read", {"file_path": narrow}, env) is None, "Read: a PNG of 1000 px is not changed")
    check(run_hook(READ_HOOK, "Read", {"file_path": exact}, env) is None, "Read: a PNG of 1024 px is not changed")
    check(run_hook(READ_HOOK, "Read", {"file_path": full}, env) is None, "Read: -full.png is not changed")
    check(run_hook(READ_HOOK, "Read", {"file_path": os.path.join(work, "x.txt")}, env) is None, "Read: a text file is not changed")
    check(run_hook(READ_HOOK, "Read", {"file_path": os.path.join(work, "missing.png")}, env) is None, "Read: a missing file is not changed")
    check(run_hook(READ_HOOK, "Write", {"file_path": wide}, env) is None, "Read hook ignores other tools")
    # a PNG outside the allowed folders is not copied
    outside = os.path.join(os.path.expanduser("~"), "token_hooks_test_outside.png")
    out = run_hook(READ_HOOK, "Read", {"file_path": outside}, env)
    check(out is None, "Read: a file outside the cm0102 and temporary folders is not changed")
    if not shutil_which("sips"):
        print("SKIP  Read: sips is not installed here, the resize is not tested")
        return
    out = run_hook(READ_HOOK, "Read", {"file_path": wide, "offset": 5, "limit": 10}, env)
    check(out is not None, "Read: a PNG of 1600 px is rewritten")
    if out:
        new = out["updatedInput"]
        check(new["file_path"].startswith(os.path.join(logs, "shots-1024") + os.sep), "Read: copy is in shots-1024", new["file_path"])
        check(os.path.exists(new["file_path"]) and png_width(new["file_path"]) <= 1024, "Read: copy exists and is at most 1024 px wide")
        check(new.get("offset") == 5 and new.get("limit") == 10, "Read: other fields stay")
        check(png_width(wide) == 1600, "Read: the original is not changed")
        again = run_hook(READ_HOOK, "Read", {"file_path": wide}, env)
        check(again and again["updatedInput"]["file_path"] == new["file_path"], "Read: the same copy is used again")
        # the copy itself is not shrunk again
        check(run_hook(READ_HOOK, "Read", {"file_path": new["file_path"]}, env) is None, "Read: a copy is left alone")
        make_png(wide, 1700, 20)                 # a new version of the file gets a new copy
        newer = run_hook(READ_HOOK, "Read", {"file_path": wide}, env)
        check(newer and newer["updatedInput"]["file_path"] != new["file_path"], "Read: a changed file gets a new copy")


def shutil_which(name):
    for d in os.environ.get("PATH", "").split(os.pathsep):
        if os.access(os.path.join(d, name), os.X_OK):
            return True
    return False


def test_settings():
    with open(os.path.join(ROOT, ".claude", "settings.json")) as f:
        s = json.load(f)
    check(s.get("promptCacheTtl") == "1h" and s.get("subagentPromptCacheTtl") == "1h", "settings: cache lifetime 1h")
    parent = "/Users/max/Projects/cm0102/.claude/settings.json"
    files = [(os.path.join(ROOT, ".claude", "settings.json"), "")]
    if os.path.exists(parent):
        files.append((parent, "cm0102-mac/"))
    for path, prefix in files:
        with open(path) as f:
            cfg = json.load(f)
        check(cfg.get("promptCacheTtl") == "1h", f"{path}: cache lifetime")
        for g in cfg["hooks"]["PreToolUse"]:
            cmd = g["hooks"][0]["command"]
            script = "bash_rewrite.py" if g["matcher"] == "Bash" else "read_shrink.py"
            check(f"$CLAUDE_PROJECT_DIR/{prefix}.claude/hooks/{script}" in cmd, f"{path}: {g['matcher']} hook path")
            # a missing script lets the call through: exit 0, no output
            event = json.dumps({"tool_name": g["matcher"], "tool_input": {"command": "ctest", "file_path": "/x.png"}})
            r = subprocess.run(["/bin/sh", "-c", cmd], input=event, capture_output=True, text=True,
                               env=dict(os.environ, CLAUDE_PROJECT_DIR="/nonexistent-project"))
            check(r.returncode == 0 and not r.stdout and not r.stderr, f"{path}: missing {script} lets the call through",
                  f"{r.returncode} {r.stdout} {r.stderr}")
            # a script that exists is run, from the project folder given
            if not prefix:
                r = subprocess.run(["/bin/sh", "-c", cmd], input=json.dumps({"tool_name": "Bash", "tool_input": {"command": "ctest"}}),
                                   capture_output=True, text=True, env=dict(os.environ, CLAUDE_PROJECT_DIR=ROOT))
                check(r.returncode == 0 and (g["matcher"] != "Bash" or "run_logged.py" in r.stdout), f"{script} runs through the settings command", r.stdout + r.stderr)


def main():
    with tempfile.TemporaryDirectory(prefix="test-hooks-") as tmp:
        work = os.path.realpath(os.path.join(tmp, "work"))
        logs = os.path.realpath(os.path.join(tmp, "logs"))
        os.makedirs(work)
        test_bash_hook_cases()
        test_bash_hook_keeps_other_fields()
        test_wrapper(work, logs)
        test_head(work, logs)
        test_end_to_end(work, logs)
        test_read_hook(work, logs)
        test_settings()
    print("PASS" if not failures else f"FAILED: {len(failures)}")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
