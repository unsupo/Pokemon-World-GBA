#ifndef GUARD_TRAINER_REMATCH_H
#define GUARD_TRAINER_REMATCH_H

// Rematch stage constants.
// Stage 0: Trainer not yet beaten (shows their normal party).
// Stage 1: First defeat. Rematch unlocks when player has 4+ global badges.
// Stage 2: Rematched at 4+ badges. Unlocks when first champion is beaten.
// Stage 3: Rematched after champion 1. Unlocks after champion 2.
// Stage 4: Rematched after champion 2. Unlocks after all 4 champions beaten.
// Stage 5: Final form. Level 100, max IVs, competitive held items.
#define REMATCH_STAGE_NONE    0  // not yet beaten
#define REMATCH_STAGE_1       1  // beaten once (base team)
#define REMATCH_STAGE_2       2  // rematch: 4+ global badges
#define REMATCH_STAGE_3       3  // rematch: 1 champion beaten
#define REMATCH_STAGE_4       4  // rematch: 2 champions beaten
#define REMATCH_STAGE_MAX     5  // rematch: all 4 champions beaten (level 100)

// Level scaling applied per stage (applied multiplicatively to base level, capped at 100).
#define REMATCH_LEVEL_CAP_STAGE_2  75
#define REMATCH_LEVEL_CAP_STAGE_3  85
#define REMATCH_LEVEL_CAP_STAGE_4  95
#define REMATCH_LEVEL_STAGE_MAX   100

// TRAINER_REMATCH_MAX_ID is defined in constants/global.h (included via global.h).

u8  GetTrainerRematchStage(u16 trainerId);
void SetTrainerRematchStage(u16 trainerId, u8 stage);
u8  GetMaxUnlockedRematchStage(void);
bool8 CanAdvanceRematchStage(u16 trainerId);
void TryAdvanceTrainerRematchStage(u16 trainerId);
void ApplyTrainerRematchScaling(u16 trainerId);
void Special_GetTrainerRematchStage(void);
void Special_GetMaxRematchStage(void);
void Special_CountBeatenChampions(void);
u8 CountBeatenChampions(void);

#endif // GUARD_TRAINER_REMATCH_H
