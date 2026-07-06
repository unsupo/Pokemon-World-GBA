#include "global.h"

// Stub for HnS day/night encounter switching. Full implementation would
// swap encounter tables based on RtcGetHour(). No-op until Johto encounters
// are authored.
void SetTimeBasedEncounters(void)
{
}

// HnS static-encounter randomizer stubs. No-op — expansion handles static
// encounters through its own system.
void EnableStaticRandomizer(void)
{
}

void DisableStaticRandomizer(void)
{
}

// Johto HnS special stubs — no-op until Johto gameplay is fully implemented
void CheckAerodactyl(void) { }
void CheckHooh(void) { }
void CheckKabuto(void) { }
void CheckOmanyte(void) { }
void CheckTogepi(void) { }
void DoSlidingPuzzle(void) { }
void GetMaxPartySize(void) { }
void HaircutBrother1(void) { }
#undef IsNuzlockeNicknamingActive  // global.h defines this as 0xFFFF (config value)
void IsNuzlockeNicknamingActive(void) { }
void IsRandomMovesActivated(void) { }
void NameRival(void) { }
void ShowBugContestChosenMon(void) { }
void ToggleShinyColors(void) { }
