#include "global.h"
#include "event_data.h"
#include "pokemon.h"
#include "battle.h"
#include "battle_setup.h"
#include "script.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "field_specials.h"
#include "trainer_rematch.h"

// ---------------------------------------------------------------------------
// Champion defeat counter
// ---------------------------------------------------------------------------

u8 CountBeatenChampions(void)
{
    u8 count = 0;
    if (FlagGet(FLAG_SYS_GAME_CLEAR))        count++; // Hoenn champion
    if (FlagGet(FLAG_BEAT_KANTO_CHAMPION))   count++; // Kanto champion
    if (FlagGet(FLAG_BEAT_JOHTO_CHAMPION))   count++; // Johto champion
    // Sinnoh champion flag (add when Sinnoh story is implemented)
    // if (FlagGet(FLAG_BEAT_SINNOH_CHAMPION)) count++;
    return count;
}

// ---------------------------------------------------------------------------
// Per-trainer stage storage (packed into gSaveBlock1Ptr->trainerRematchStages)
// ---------------------------------------------------------------------------

u8 GetTrainerRematchStage(u16 trainerId)
{
    if (trainerId == 0 || trainerId >= TRAINER_REMATCH_MAX_ID)
        return REMATCH_STAGE_NONE;
    return gSaveBlock1Ptr->trainerRematchStages[trainerId];
}

void SetTrainerRematchStage(u16 trainerId, u8 stage)
{
    if (trainerId == 0 || trainerId >= TRAINER_REMATCH_MAX_ID)
        return;
    if (stage > REMATCH_STAGE_MAX)
        stage = REMATCH_STAGE_MAX;
    gSaveBlock1Ptr->trainerRematchStages[trainerId] = stage;
}

// ---------------------------------------------------------------------------
// Global milestone: highest rematch stage available to all trainers
// ---------------------------------------------------------------------------

u8 GetMaxUnlockedRematchStage(void)
{
    u8 badges = GetGlobalBadgeCount();
    u8 champions = CountBeatenChampions();

    if (champions >= 4)  return REMATCH_STAGE_MAX; // all 4 E4s beaten
    if (champions >= 2)  return REMATCH_STAGE_4;   // 2 E4s beaten
    if (champions >= 1)  return REMATCH_STAGE_3;   // 1 E4 beaten
    if (badges >= 4)     return REMATCH_STAGE_2;   // got 4+ global badges
    return REMATCH_STAGE_1;  // can advance to stage 1 only (first defeat)
}

bool8 CanAdvanceRematchStage(u16 trainerId)
{
    u8 current = GetTrainerRematchStage(trainerId);
    u8 maxAllowed = GetMaxUnlockedRematchStage();
    return current < maxAllowed;
}

void TryAdvanceTrainerRematchStage(u16 trainerId)
{
    if (trainerId == 0 || trainerId >= TRAINER_REMATCH_MAX_ID)
        return;
    if (CanAdvanceRematchStage(trainerId))
        SetTrainerRematchStage(trainerId, GetTrainerRematchStage(trainerId) + 1);
}

// ---------------------------------------------------------------------------
// Battle scaling: modify gParties[B_TRAINER_OPPONENT_A] based on stage.
// Called right after CreateNPCTrainerParty in battle_main.c.
// ---------------------------------------------------------------------------

static u8 ScaleLevelForStage(u8 baseLevel, u8 stage)
{
    u16 scaled;
    u8 cap;

    switch (stage)
    {
    case REMATCH_STAGE_2:
        scaled = ((u16)baseLevel * 13) / 10;
        cap = REMATCH_LEVEL_CAP_STAGE_2;
        break;
    case REMATCH_STAGE_3:
        scaled = ((u16)baseLevel * 16) / 10;
        cap = REMATCH_LEVEL_CAP_STAGE_3;
        break;
    case REMATCH_STAGE_4:
        scaled = ((u16)baseLevel * 18) / 10;
        cap = REMATCH_LEVEL_CAP_STAGE_4;
        break;
    case REMATCH_STAGE_MAX:
        return REMATCH_LEVEL_STAGE_MAX;
    default:
        return baseLevel;
    }

    if (scaled < baseLevel) scaled = baseLevel; // never reduce
    return (u8)(scaled > cap ? cap : scaled);
}

// Give competitive held items at stage 5 if the mon has none.
static void AssignCompetitiveItem(struct Pokemon *mon)
{
    u16 currentItem;
    u16 baseAtk, baseSpa, baseHp, baseDef;

    if (GetMonData(mon, MON_DATA_SPECIES) == SPECIES_NONE)
        return;

    currentItem = GetMonData(mon, MON_DATA_HELD_ITEM);
    if (currentItem != ITEM_NONE)
        return;  // already has an item

    baseAtk = GetMonData(mon, MON_DATA_ATK);
    baseSpa = GetMonData(mon, MON_DATA_SPATK);
    baseHp  = GetMonData(mon, MON_DATA_MAX_HP);
    baseDef = GetMonData(mon, MON_DATA_DEF);

    if (baseHp > 200 && baseDef > 150)
    {
        // Bulky wall: Leftovers
        SetMonData(mon, MON_DATA_HELD_ITEM, &(u16){ITEM_LEFTOVERS});
    }
    else if (baseAtk > baseSpa)
    {
        // Physical attacker: Choice Band
        SetMonData(mon, MON_DATA_HELD_ITEM, &(u16){ITEM_CHOICE_BAND});
    }
    else
    {
        // Special attacker or mixed: Choice Specs
        SetMonData(mon, MON_DATA_HELD_ITEM, &(u16){ITEM_CHOICE_SPECS});
    }
}

void ApplyTrainerRematchScaling(u16 trainerId)
{
    u8 i;
    u8 stage = GetTrainerRematchStage(trainerId);
    u8 newLevel;
    u16 species;

    if (stage <= REMATCH_STAGE_1)
        return;  // no scaling needed for first defeat or unbeaten

    for (i = 0; i < PARTY_SIZE; i++)
    {
        species = GetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_SPECIES);
        if (species == SPECIES_NONE)
            break;

        newLevel = ScaleLevelForStage(
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_LEVEL),
            stage);
        SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_LEVEL, &newLevel);

        // Stage 5: max IVs + competitive items
        if (stage == REMATCH_STAGE_MAX)
        {
            static const u8 maxIV = 31;
            SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_HP_IV,    &maxIV);
            SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_ATK_IV,   &maxIV);
            SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_DEF_IV,   &maxIV);
            SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_SPEED_IV, &maxIV);
            SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_SPATK_IV, &maxIV);
            SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_SPDEF_IV, &maxIV);
            AssignCompetitiveItem(&gParties[B_TRAINER_OPPONENT_A][i]);
            // Recalculate stats after level/IV change
            CalculateMonStats(&gParties[B_TRAINER_OPPONENT_A][i]);
        }
        else
        {
            CalculateMonStats(&gParties[B_TRAINER_OPPONENT_A][i]);
        }
    }
}

// ---------------------------------------------------------------------------
// Script specials
// ---------------------------------------------------------------------------

void Special_GetTrainerRematchStage(void)
{
    gSpecialVar_0x8000 = GetTrainerRematchStage(TRAINER_BATTLE_PARAM.opponentA);
}

void Special_GetMaxRematchStage(void)
{
    gSpecialVar_0x8000 = GetMaxUnlockedRematchStage();
}

void Special_CountBeatenChampions(void)
{
    gSpecialVar_0x8000 = CountBeatenChampions();
}
