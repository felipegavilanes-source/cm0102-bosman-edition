"""World competition report tests; no game executable or databases needed."""
import csv
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools" / "world_competitions"))
import report


def entry(comp, season, fmt, **kw):
    return {"id": f"{comp}-{season}", "season": season, "nation": "UEFA",
            "source_part": "uefa", "comp": comp,
            "comp_file": kw.get("comp_file", "old_cup"),
            "tier": "cup", "name": comp, "clubs": kw.get("clubs", "16"),
            "up": "n/a", "down": "n/a", "playoffs": "n/a", "split": "n/a",
            "format": fmt}


class WorldCompetitionReportTests(unittest.TestCase):
    def test_discontinued_resumed_and_changed(self):
        rows = [
            entry("Former Cup", "2025/26", "one match"),
            entry("Former Cup", "2026/27", "not held"),
            entry("New Cup", "2025/26", "not held", comp_file="new"),
            entry("New Cup", "2026/27", "groups", comp_file="new"),
            entry("Stable Cup", "2025/26", "groups", clubs="16"),
            entry("Stable Cup", "2026/27", "groups", clubs="20"),
        ]
        result = {r["competition"]: r for r in report.compare(rows, "2026/27", "2025/26")}
        self.assertEqual(result["Former Cup"]["transition"], "discontinued_or_paused")
        self.assertEqual(result["New Cup"]["transition"], "resumed_or_introduced")
        self.assertTrue(result["New Cup"]["implementation_review"])
        self.assertEqual(result["Stable Cup"]["transition"], "rules_changed")

    def test_missing_is_not_extinct(self):
        rows = [entry("Legacy", "2025/26", "groups"),
                entry("Modern", "2026/27", "groups")]
        result = {r["competition"]: r for r in report.compare(rows, "2026/27", "2025/26")}
        self.assertEqual(result["Legacy"]["status"], "not_listed")
        self.assertEqual(result["Legacy"]["transition"], "missing_from_research")
        self.assertEqual(result["Modern"]["transition"], "newly_listed")

    def test_unknown_and_inference(self):
        r = entry("Cup", "2026/27", "unknown", clubs="20 (inference)")
        self.assertEqual(report.status(r), "uncertain")
        self.assertEqual(report.uncertainty(r), ["clubs", "format"])

    def test_competition_identity_distinguishes_divisions(self):
        a = entry("league", "2026/27", "round robin", comp_file="den_prm")
        b = entry("league", "2026/27", "round robin", comp_file="den_first")
        self.assertNotEqual(report.key(a), report.key(b))
        self.assertEqual(len(report.index_year([a, b], "2026/27")), 2)

    def test_duplicate_is_an_error(self):
        r = entry("Cup", "2026/27", "groups")
        with self.assertRaisesRegex(ValueError, "Duplicate competition"):
            report.index_year([r, dict(r, id="other")], "2026/27")

    def test_exact_tsv_header_ignores_check_files(self):
        with tempfile.TemporaryDirectory() as tmp:
            folder = Path(tmp)
            with (folder / "uefa.tsv").open("w", newline="", encoding="utf-8") as f:
                writer = csv.DictWriter(f, fieldnames=report.FIELDS, delimiter="\t")
                writer.writeheader()
                writer.writerow({k: entry("Cup", "2026/27", "groups").get(k, "n/a")
                                 for k in report.FIELDS})
            with (folder / "uefa.check.tsv").open("w", encoding="utf-8") as f:
                f.write("not\ta\tcompetition\ttable\n")
            self.assertEqual(len(report.read_parts(folder)), 1)

    def test_report_is_deterministic(self):
        rows = [entry("Cup", "2025/26", "groups"),
                entry("Cup", "2026/27", "not held")]
        result = report.compare(rows, "2026/27", "2025/26")
        generated = report.markdown(result, "2026/27", "2025/26")
        self.assertIn("discontinued_or_paused", generated)
        self.assertEqual(generated, report.markdown(result, "2026/27", "2025/26"))


if __name__ == "__main__":
    unittest.main()
