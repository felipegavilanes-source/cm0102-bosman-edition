#!/usr/bin/env python3
"""Static 2026 Brazilian league-rule guard; does not claim runtime support.

Run: python3 tests/seasons/test_brazil_2026_rules.py
"""
import csv
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[2]
RULES = ROOT / "docs/research/rules-0.6.0/bra.tsv"


def brazil_2026_rules():
    with RULES.open(encoding="utf-8", newline="") as stream:
        rows = list(csv.DictReader(stream, delimiter="\t"))
    return {
        row["comp_file"]: row for row in rows
        if row["real_season"] == "2026"
        and row["comp_file"] in ("bra_nat_first", "bra_nat_second")
    }


class Brazil2026RulesTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.rules = brazil_2026_rules()

    def test_both_divisions_exist(self):
        self.assertEqual(set(self.rules), {"bra_nat_first", "bra_nat_second"})

    def test_twenty_teams_double_round_robin(self):
        for league in self.rules.values():
            with self.subTest(league=league["comp_file"]):
                self.assertEqual(int(league["clubs"]), 20)
                self.assertIn("double round robin", league["format"])
                self.assertEqual(int(league["clubs"]) * 19, 380)
                self.assertEqual(2 * (int(league["clubs"]) - 1), 38)

    def test_four_relegations_each(self):
        for league in self.rules.values():
            with self.subTest(league=league["comp_file"]):
                self.assertEqual(league["down"], "4")

    def test_serie_a_no_playoffs(self):
        self.assertEqual(self.rules["bra_nat_first"]["playoffs"], "none")

    def test_serie_b_two_direct_two_playoff_promotions(self):
        second = self.rules["bra_nat_second"]
        self.assertEqual(second["up"], "2")
        shape = second["playoffs"].lower()
        for fragment in ("3rd v 6th", "4th v 5th", "two-legged", "two winners"):
            self.assertIn(fragment, shape)
        self.assertNotIn("final", shape)

    def test_2026_calendar_year(self):
        for league in self.rules.values():
            self.assertEqual(league["season"], "2025/26")
            self.assertEqual(league["real_season"], "2026")


if __name__ == "__main__":
    unittest.main()
