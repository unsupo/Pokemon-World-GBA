#ifndef GUARD_NEW_GAME_H
#define GUARD_NEW_GAME_H

extern bool8 gDifferentSaveFile;
extern u8 gNewGameRegion;
extern u8 gNewGameAvatarStyle;
extern u8 gNewGameHardMode;

#define STARTING_REGION_HOENN  0
#define STARTING_REGION_KANTO  1
#define STARTING_REGION_YELLOW 2
#define STARTING_REGION_JOHTO  3
#define STARTING_REGION_SINNOH 4
// Shortcuts some randomness in berry_blender.c, and enables debug printing
// in contest.c.
extern bool8 gEnableContestDebugging;

void SetTrainerId(u32 trainerId, u8 *dst);
u32 GetTrainerId(u8 *trainerId);
void CopyTrainerId(u8 *dst, u8 *src);
void NewGameInitData(void);
void ResetMenuAndMonGlobals(void);
void Sav2_ClearSetDefault(void);

#endif // GUARD_NEW_GAME_H
