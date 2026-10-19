#!/usr/bin/env python3
"""Token report from Claude Code session files (standard library only).

Usage:
    python3 tools/dev/token_report.py [--from YYYY-MM-DD] [--to YYYY-MM-DD]

Reads every .jsonl file under ~/.claude/projects/-Users-max-Projects-cm0102*
(recursively, subagent files included). Prints three tables:

  1. Per day and model: input, cache write, cache read and output tokens,
     and the number of turns. Each assistant message counts once, by message.id.
  2. Tokens per turn: cache read divided by turns.
  3. Tool output size in characters by tool name, text and images apart.
     Results are matched to tool calls by tool_use id.

Days are UTC dates, the same rule as the 19 October measurement. --from and --to
are inclusive. Streamed entries that repeat a message.id count once, using the
first entry. Read-only: the script writes nothing.
"""

import argparse
import glob
import json
import os
import re
from collections import defaultdict
from datetime import datetime, timezone

PROJECT_GLOB = os.path.expanduser("~/.claude/projects/-Users-max-Projects-cm0102*")


def parse_args():
    p = argparse.ArgumentParser(description="Token report from session files.")
    p.add_argument("--from", dest="date_from", metavar="YYYY-MM-DD", help="first day, inclusive")
    p.add_argument("--to", dest="date_to", metavar="YYYY-MM-DD", help="last day, inclusive")
    args = p.parse_args()
    for name in ("date_from", "date_to"):
        value = getattr(args, name)
        if value and not re.fullmatch(r"\d{4}-\d{2}-\d{2}", value):
            p.error(f"--{name.replace('date_', '')} must be YYYY-MM-DD")
    return args


def session_files():
    files = []
    for project_dir in sorted(glob.glob(PROJECT_GLOB)):
        for root, _dirs, names in os.walk(project_dir):
            for name in names:
                if name.endswith(".jsonl"):
                    files.append(os.path.join(root, name))
    return sorted(files)


def utc_day(timestamp):
    """Return the UTC YYYY-MM-DD for an ISO timestamp such as 2026-10-17T10:56:55.348Z."""
    if not timestamp:
        return None
    try:
        when = datetime.fromisoformat(timestamp.replace("Z", "+00:00"))
    except ValueError:
        return None
    return when.astimezone(timezone.utc).strftime("%Y-%m-%d")


def in_range(day, date_from, date_to):
    if day is None:
        return False
    if date_from and day < date_from:
        return False
    if date_to and day > date_to:
        return False
    return True


def result_size(content):
    """Return (text_chars, image_count, image_chars) for a tool_result content value."""
    if content is None:
        return 0, 0, 0
    if isinstance(content, str):
        return len(content), 0, 0
    text_chars = 0
    images = 0
    image_chars = 0
    for block in content:
        if not isinstance(block, dict):
            continue
        kind = block.get("type")
        if kind == "text":
            text_chars += len(block.get("text", ""))
        elif kind == "image":
            images += 1
            source = block.get("source") or {}
            image_chars += len(source.get("data", ""))
    return text_chars, images, image_chars


def main():
    args = parse_args()
    files = session_files()

    # Each assistant message id counts once. Streamed entries repeat the same
    # message, so the first entry seen for an id is kept.
    messages = {}
    tool_names = {}  # tool_use id -> tool name
    results = {}  # tool_use id -> (day, text_chars, images, image_chars)

    for path in files:
        with open(path, encoding="utf-8", errors="replace") as handle:
            for line in handle:
                if '"type"' not in line:
                    continue
                try:
                    record = json.loads(line)
                except json.JSONDecodeError:
                    continue
                message = record.get("message")
                if not isinstance(message, dict):
                    continue
                content = message.get("content")
                if record.get("type") == "assistant" and message.get("role", "assistant") == "assistant":
                    msg_id = message.get("id")
                    usage = message.get("usage")
                    if msg_id and usage and msg_id not in messages:
                        messages[msg_id] = {
                            "day": utc_day(record.get("timestamp")),
                            "model": message.get("model", "unknown"),
                            "input": usage.get("input_tokens", 0) or 0,
                            "cache_write": usage.get("cache_creation_input_tokens", 0) or 0,
                            "cache_read": usage.get("cache_read_input_tokens", 0) or 0,
                            "output": usage.get("output_tokens", 0) or 0,
                        }
                    if isinstance(content, list):
                        for block in content:
                            if isinstance(block, dict) and block.get("type") == "tool_use":
                                tool_names.setdefault(block.get("id"), block.get("name", "unknown"))
                elif isinstance(content, list):
                    for block in content:
                        if isinstance(block, dict) and block.get("type") == "tool_result":
                            tool_id = block.get("tool_use_id")
                            if tool_id and tool_id not in results:
                                results[tool_id] = (
                                    utc_day(record.get("timestamp")),
                                    *result_size(block.get("content")),
                                )

    # Table 1: tokens per day and model.
    totals = defaultdict(lambda: {"turns": 0, "input": 0, "cache_write": 0, "cache_read": 0, "output": 0})
    for msg in messages.values():
        if not in_range(msg["day"], args.date_from, args.date_to):
            continue
        row = totals[(msg["day"], msg["model"])]
        row["turns"] += 1
        row["input"] += msg["input"]
        row["cache_write"] += msg["cache_write"]
        row["cache_read"] += msg["cache_read"]
        row["output"] += msg["output"]

    def millions(value):
        return f"{value / 1e6:,.1f}M"

    print(f"Session files read: {len(files)}")
    print(f"Assistant messages (by message.id): {len(messages)}")
    print()
    print("1. Tokens per day and model")
    print(f"{'day':<12}{'model':<22}{'turns':>9}{'input':>10}{'cache write':>14}{'cache read':>13}{'output':>10}")
    for (day, model), row in sorted(totals.items()):
        print(
            f"{day:<12}{model:<22}{row['turns']:>9,}"
            f"{millions(row['input']):>10}{millions(row['cache_write']):>14}"
            f"{millions(row['cache_read']):>13}{millions(row['output']):>10}"
        )
    print()

    # Table 2: cache read per turn.
    print("2. Tokens per turn (cache read / turns)")
    print(f"{'day':<12}{'model':<22}{'turns':>9}{'cache read per turn':>22}")
    for (day, model), row in sorted(totals.items()):
        per_turn = row["cache_read"] / row["turns"] if row["turns"] else 0
        print(f"{day:<12}{model:<22}{row['turns']:>9,}{per_turn:>22,.0f}")
    print()

    # Table 3: tool output by tool name, for results dated inside the range.
    tool_totals = defaultdict(lambda: {"results": 0, "text_chars": 0, "images": 0, "image_chars": 0})
    unmatched = 0
    for tool_id, (day, text_chars, images, image_chars) in results.items():
        if not in_range(day, args.date_from, args.date_to):
            continue
        name = tool_names.get(tool_id)
        if name is None:
            unmatched += 1
            name = "(no matching tool call)"
        row = tool_totals[name]
        row["results"] += 1
        row["text_chars"] += text_chars
        row["images"] += images
        row["image_chars"] += image_chars

    print("3. Tool output by tool name (characters; images counted apart)")
    print(f"{'tool':<28}{'results':>9}{'text chars':>14}{'images':>9}{'image data chars':>19}")
    for name, row in sorted(tool_totals.items(), key=lambda item: -item[1]["text_chars"]):
        print(
            f"{name:<28}{row['results']:>9,}{row['text_chars']:>14,}"
            f"{row['images']:>9,}{row['image_chars']:>19,}"
        )
    if unmatched:
        print(f"Results without a matching tool call: {unmatched:,}")


if __name__ == "__main__":
    main()
