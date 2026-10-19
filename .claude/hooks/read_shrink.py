#!/usr/bin/env python3
"""PreToolUse hook on Read: give the agent a copy of a wide PNG that is at most 1024 px wide.

A PNG wider than 1024 px is copied with sips into <run logs>/shots-1024/ and the Read gets the path of
the copy (hookSpecificOutput.updatedInput). The copy is made once for each file version. A file whose
name ends in -full.png is read as it is, and so is every other file. Test scripts and their comparisons
keep using the full-size files, because only the Read of an agent changes.

Only a PNG in the cm0102 folder, the project folder or a temporary folder is copied: the copy is
read without the permission check of the original path.

Python 3.9 or later, no packages. The run logs folder is /Users/max/Projects/cm0102/run-logs, or the
environment variable RUN_LOGS_DIR.
"""
import hashlib
import json
import os
import struct
import subprocess
import sys

MAX_WIDTH = 1024
CM0102_ROOT = os.environ.get("CM0102_ROOT", "/Users/max/Projects/cm0102")
RUN_LOGS = os.environ.get("RUN_LOGS_DIR", os.path.join(CM0102_ROOT, "run-logs"))
TEMP_ROOTS = ("/tmp", "/private/tmp", "/private/var/folders", "/var/folders")


def png_width(path):
    with open(path, "rb") as f:
        head = f.read(24)
    if len(head) < 24 or head[:8] != b"\x89PNG\r\n\x1a\n" or head[12:16] != b"IHDR":
        return None
    return struct.unpack(">I", head[16:20])[0]


def inside(path, roots):
    return any(path == r or path.startswith(r.rstrip("/") + "/") for r in roots if r)


def shrink(path, project_dir):
    """The path of a copy at most MAX_WIDTH px wide, or None when the file stays as it is."""
    if not path.lower().endswith(".png") or path.lower().endswith("-full.png"):
        return None
    real = os.path.realpath(path)
    roots = [os.path.realpath(r) for r in (CM0102_ROOT, project_dir) if r] + \
        [os.path.realpath(r) for r in TEMP_ROOTS]
    if not os.path.isfile(real) or not inside(real, roots):
        return None
    if os.path.dirname(real) == os.path.realpath(os.path.join(RUN_LOGS, "shots-1024")):
        return None
    width = png_width(real)
    if width is None or width <= MAX_WIDTH:
        return None
    st = os.stat(real)
    key = hashlib.sha1(f"{real}|{st.st_size}|{st.st_mtime_ns}".encode()).hexdigest()[:10]
    out_dir = os.path.join(RUN_LOGS, "shots-1024")
    os.makedirs(out_dir, exist_ok=True)
    out = os.path.join(out_dir, f"{key}-{os.path.basename(real)}")
    if not os.path.exists(out):
        tmp = out + f".{os.getpid()}.tmp.png"
        r = subprocess.run(["sips", "--resampleWidth", str(MAX_WIDTH), real, "--out", tmp],
                           capture_output=True, timeout=60)
        if r.returncode != 0 or not os.path.exists(tmp):
            return None
        os.replace(tmp, out)
    return out


def main():
    try:
        data = json.load(sys.stdin)
        if data.get("tool_name") != "Read":
            return 0
        tool_input = data.get("tool_input") or {}
        path = tool_input.get("file_path")
        if not isinstance(path, str):
            return 0
        copy = shrink(path, os.environ.get("CLAUDE_PROJECT_DIR") or data.get("cwd"))
        if copy is None:
            return 0
        note = (f"Read a copy of {path} at most {MAX_WIDTH} px wide ({copy}). "
                f"To read the full-size picture, copy it to a name that ends in -full.png.")
        print(json.dumps({"hookSpecificOutput": {
            "hookEventName": "PreToolUse",
            "updatedInput": dict(tool_input, file_path=copy),
            "additionalContext": note}}))
    except Exception:       # a hook that fails must never stop a Read
        return 0
    return 0


if __name__ == "__main__":
    sys.exit(main())
