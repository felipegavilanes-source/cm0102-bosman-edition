#!/usr/bin/env python3
"""Read-only world competition change report from the Bosman 0.6.0 research tables.

This tool never changes the executable, game database, saves or user interface.
"""
import argparse
import csv
import json
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PARTS = ROOT / "docs" / "research" / "rules-0.6.0"
# Exact format of the 20 source rules tables (tools/seasons/make_rules.py).
# Using exact equality avoids accidentally reading *.check.tsv and club tables.
FIELDS = ("id", "season", "real_season", "nation", "comp_file", "comp", "tier", "name",
          "clubs", "up", "down", "playoffs", "split", "format", "subs_used", "bench",
          "foreign", "work_permit", "registration", "windows", "notes", "sources")
RULE_FIELDS = ("clubs", "up", "down", "playoffs", "split", "format")


def read_parts(folder=PARTS):
    found = []
    for path in sorted(Path(folder).glob("*.tsv")):
        with path.open(encoding="utf-8", newline="") as stream:
            reader = csv.DictReader(stream, delimiter="\t")
            if tuple(reader.fieldnames or ()) != FIELDS:
                continue
            rows = list(reader)
        for line, row in enumerate(rows, 2):
            if not row.get("id") or not row.get("season") or any(v is None for v in row.values()):
                raise ValueError(f"{path.name}:{line}: malformed competition rule")
            row["source_part"] = path.stem
        found.extend(rows)
    if not found:
        raise ValueError(f"No world competition rules found in {folder}")
    return found


def key(row):
    # A comp like "league" can be shared across several tiers of one nation.
    # New records are distinguished by name, since comp_file is always "new".
    return (row["source_part"], row["nation"], row["comp_file"],
            row["name"] if row["comp_file"] == "new" else row["comp"])


def is_administrative(row):
    return row["comp"].strip().lower() in ("nation rules", "access list")


def status(row):
    if row is None:
        return "not_listed"
    if is_administrative(row):
        return "administrative"
    fmt = row["format"].strip().lower()
    if fmt.startswith(("not held", "cancelled", "canceled")):
        return "inactive"
    if not fmt or fmt.startswith("unknown"):
        return "uncertain"
    return "active"


def uncertainty(row):
    if row is None:
        return []
    return [f for f in RULE_FIELDS if "unknown" in (row.get(f) or "").lower()
            or "inference" in (row.get(f) or "").lower()]


def index_year(rows, year):
    result = {}
    for row in rows:
        if row["season"] != year:
            continue
        ident = key(row)
        if ident in result:
            raise ValueError(f"Duplicate competition {ident!r} for {year}: "
                             f"{result[ident]['id']} and {row['id']}")
        result[ident] = row
    if not result:
        raise ValueError(f"No competition entries for {year}")
    return result


def compare(rows, season, previous):
    current, earlier = index_year(rows, season), index_year(rows, previous)
    result = []
    for ident in sorted(set(current) | set(earlier)):
        before, after = earlier.get(ident), current.get(ident)
        prior, now = status(before), status(after)
        if after is None:
            transition = "missing_from_research"  # Not proof of extinction.
        elif before is None:
            transition = "newly_listed"  # Not proof the competition is new.
        elif prior == "inactive" and now in ("active", "uncertain"):
            transition = "resumed_or_introduced"
        elif prior in ("active", "uncertain") and now == "inactive":
            transition = "discontinued_or_paused"
        elif any(before.get(f) != after.get(f) for f in RULE_FIELDS):
            transition = "rules_changed"
        else:
            transition = "unchanged"
        chosen = after or before
        result.append({
            "source_part": ident[0], "nation": ident[1], "competition": chosen["comp"],
            "name": chosen["name"], "status": now, "previous_status": prior,
            "transition": transition, "year": season, "previous_year": previous,
            "record_id": after["id"] if after else None,
            "comp_file": after["comp_file"] if after else None,
            "fields_to_verify": uncertainty(after),
            "implementation_review": bool(after and after["comp_file"] == "new"
                                           and now in ("active", "uncertain")),
        })
    return result


def markdown(result, season, previous):
    totals = Counter(r["status"] for r in result)
    changes = Counter(r["transition"] for r in result)
    lines = [
        f"# World competitions — {season} compared with {previous}", "",
        "Generated from the Bosman Edition research TSV files. This is **not** a game patch or",
        "proof that every researched format is implemented in the executable.", "",
        "## Coverage", "",
        f"- Records in comparison: {len(result)}",
        f"- Active in target season: {totals['active']}",
        f"- Inactive in target season: {totals['inactive']}",
        f"- Uncertain format in target season: {totals['uncertain']}",
        f"- Not listed in target season: {totals['not_listed']}",
        f"- New entries in the research: {changes['newly_listed']}",
        f"- Changed researched rules: {changes['rules_changed']}",
        f"- Transitions to inactive: {changes['discontinued_or_paused']}",
        f"- Active new-code records requiring review: {sum(r['implementation_review'] for r in result)}",
        "", "## Entries to review", "",
        "| Region / file | Competition | Status | Change | Verify |",
        "|---|---|---|---|---|",
    ]
    for r in result:
        if r["transition"] == "unchanged" and not r["fields_to_verify"] and not r["implementation_review"]:
            continue
        def clean(value):
            return str(value).replace("|", "\\|").replace("\n", " ")
        verify = ", ".join(r["fields_to_verify"]) or ("new code" if r["implementation_review"] else "—")
        columns = (r["nation"] + " / " + r["source_part"], r["name"], r["status"],
                   r["transition"], verify)
        lines.append("| " + " | ".join(map(clean, columns)) + " |")
    lines += ["", "Statuses reflect researched regulations, not verified game behavior.",
              "Missing is not equivalent to extinct. Unknown or inferred rules need confirmation.", ""]
    return "\n".join(lines)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--season", default="2026/27")
    parser.add_argument("--previous", default="2025/26")
    parser.add_argument("--rules-dir", type=Path, default=PARTS)
    parser.add_argument("--out", type=Path, help="Write report as Markdown (default stdout)")
    parser.add_argument("--json-out", type=Path, help="Optional machine-readable JSON")
    args = parser.parse_args(argv)
    if args.season == args.previous:
        parser.error("--season and --previous must differ")
    try:
        entries = compare(read_parts(args.rules_dir), args.season, args.previous)
    except ValueError as error:
        parser.error(str(error))
    report = markdown(entries, args.season, args.previous)
    if args.out:
        args.out.parent.mkdir(parents=True, exist_ok=True)
        args.out.write_text(report, encoding="utf-8")
    else:
        print(report)
    if args.json_out:
        args.json_out.parent.mkdir(parents=True, exist_ok=True)
        args.json_out.write_text(json.dumps(entries, indent=2, ensure_ascii=False) + "\n",
                                 encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
