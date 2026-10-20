# Slice 15b research brief: club facts for 39 missing clubs

Input: `rows.tsv` in this folder (52 rows, 39 clubs). Columns: `season`, `part`, `list`, `club`, `database_division`, `source`.

Season key: `season` is the start year of the game season. 2005 means the 2005/06 game season. A calendar-year league (K League, J2 League, Irish First Division, Russian First Division up to 2010, Primera B Nacional is August to June) uses the year after: the 2005 row of the K League is the real 2006 season. Use the `source` URL of the row to see which real season the row means.

## What to write

Repo worktree: `/Users/max/Projects/cm0102/cm0102-mac/.claude/worktrees/slice-15b-86a4b1`. Write two files in `docs/research/club-records-0.6.0/`:

1. `clubs.tsv`, one row per club and season row of `rows.tsv` (52 rows), columns:
   `season real_season nation division club name short_name city stadium capacity colour1 colour2 founded source source2 notes`
   - `club`: the name as in rows.tsv (the key).
   - `name`: the club's full name in that real season, at most 50 characters, Latin letters with the accents of Windows-1252 only (Croatian č ć đ, Polish ę ł ń ś ż, Turkish ğ ı ş have no Windows-1252 form: write the plain letter, for example "Medimurje", "Drweca", and say so in `notes`). Use the common English form the game uses for clubs of that nation (look at the neighbouring clubs of the same division in `docs/research/rules-0.6.0/<part>-clubs.tsv`, column `db_club`, for the style).
   - `short_name`: at most 25 characters, the name a table shows ("Daegu", "Central Coast").
   - `city`: the city where the club played its home games that season.
   - `stadium`, `capacity`: the home ground of that season and its capacity then (a number).
   - `colour1`, `colour2`: main and second home kit colours, as plain colour words (Red, White, Sky Blue, Navy Blue, Yellow, Green, Black, Orange, Purple, Maroon, Gold ...).
   - `founded`: the year of foundation.
   - `source`: the URL that gives these facts, read on the day. `source2`: a page of another publisher that agrees with the name, city, stadium and division of that season (RSSSF, worldfootball.net, transfermarkt, soccerway, the league's own site, a national newspaper). Another language edition of Wikipedia is the same publisher and does not count.
   - `notes`: any fact that you could not confirm ("capacity: no second source"), any name change, and any doubt.
2. `clubs.check.md`: for each club, which facts the second source agrees with, disagrees with, or does not give. One line per club. A short list of the open points at the end.

## Rules

- A fact you cannot find: write `unknown`. Do not guess. Mark an inference with "(inference)" in `notes`.
- Check that each row is right: the club really played in that division in that real season. If a row is wrong, say so in `notes` and in the check file. Do not delete it.
- Read raw pages with `curl -s -A "Mozilla/5.0" <url>` and grep them: the summary of a fetch tool can get numbers wrong.
- Never put an email address or personal data in a request header.
- Never name another modder, a game patch, a fan database or its tool, or a forum poster in any file.
- Temporary files go in your own `mktemp -d` folder. Never run `rm` with a wildcard or a variable path.
- Do not edit any other file in the repo. Do not commit.
- Write plain English. No em dashes.

Report in at most 200 words: the files, how many rows have a second source for each fact, and the open points.
