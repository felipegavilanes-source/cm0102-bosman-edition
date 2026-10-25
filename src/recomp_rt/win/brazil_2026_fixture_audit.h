/* Opt-in, read-only observation of successful original FIXTURE_MANAGER::add_fixture
 * calls while a modern Brazilian league is creating its fixtures.
 * No fixture, table, manager or save game is modified.
 */
#ifndef RECOMP_BRAZIL_2026_FIXTURE_AUDIT_H
#define RECOMP_BRAZIL_2026_FIXTURE_AUDIT_H
#include <stdint.h>

void brazil_fixture_audit_begin(uint32_t league);
void brazil_fixture_audit_observe(uint32_t fixture);
void brazil_fixture_audit_end(uint32_t league);
/* 1 if this immediate league-create call observed all 380 valid directed
 * successful add_fixture returns for the given league, 0 otherwise.
 * Not proof of save/load persistence or future unchanged match dates.
 */
int brazil_fixture_audit_ready(uint32_t league);

#endif
