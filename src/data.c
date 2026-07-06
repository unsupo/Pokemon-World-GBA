#include "global.h"
#include "malloc.h"
#include "battle.h"
#include "data.h"
#include "graphics.h"
#include "trainer_pools.h"
#include "battle_transition.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/trainers.h"
#include "constants/battle_ai.h"

const u16 gMinigameDigits_Pal[] = INCGFX_U16("graphics/link/minigame_digits.png", ".gbapal");
const u32 gMinigameDigits_Gfx[] = INCGFX_U32("graphics/link/minigame_digits.png", ".4bpp.smol");
static const u32 sMinigameDigitsThin_Gfx[] = INCGFX_U32("graphics/link/minigame_digits2.png", ".4bpp.smol"); // Unused

#define BATTLER_OFFSET(i) (gHeap + 0x8000 + MON_PIC_SIZE * (i))

const struct SpriteFrameImage gBattlerPicTable_PlayerLeft[] =
{
    {BATTLER_OFFSET(0), MON_PIC_SIZE},
    {BATTLER_OFFSET(1), MON_PIC_SIZE},
    {BATTLER_OFFSET(2), MON_PIC_SIZE},
    {BATTLER_OFFSET(3), MON_PIC_SIZE},
};

const struct SpriteFrameImage gBattlerPicTable_OpponentLeft[] =
{
    {BATTLER_OFFSET(4), MON_PIC_SIZE},
    {BATTLER_OFFSET(5), MON_PIC_SIZE},
    {BATTLER_OFFSET(6), MON_PIC_SIZE},
    {BATTLER_OFFSET(7), MON_PIC_SIZE},
};

const struct SpriteFrameImage gBattlerPicTable_PlayerRight[] =
{
    {BATTLER_OFFSET(8),  MON_PIC_SIZE},
    {BATTLER_OFFSET(9),  MON_PIC_SIZE},
    {BATTLER_OFFSET(10), MON_PIC_SIZE},
    {BATTLER_OFFSET(11), MON_PIC_SIZE},
};

const struct SpriteFrameImage gBattlerPicTable_OpponentRight[] =
{
    {BATTLER_OFFSET(12), MON_PIC_SIZE},
    {BATTLER_OFFSET(13), MON_PIC_SIZE},
    {BATTLER_OFFSET(14), MON_PIC_SIZE},
    {BATTLER_OFFSET(15), MON_PIC_SIZE},
};

const union AnimCmd sAnim_GeneralFrame0[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END,
};

const union AnimCmd sAnim_GeneralFrame3[] =
{
    ANIMCMD_FRAME(3, 0),
    ANIMCMD_END,
};

// Many of these affine anims seem to go unused, and
// instead SetSpriteRotScale is used to manipulate
// the battler sprites directly (for instance, in AnimTask_SwitchOutShrinkMon).
// Those with explicit indexes are referenced elsewhere.

static const union AffineAnimCmd sAffineAnim_Battler_Normal[] =
{
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 0),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_Battler_Flipped[] =
{
    AFFINEANIMCMD_FRAME(-0x100, 0x100, 0, 0),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_Battler_Emerge[] =
{
    AFFINEANIMCMD_FRAME(0x28, 0x28, 0, 0),
    AFFINEANIMCMD_FRAME(0x12, 0x12, 0, 12),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_Battler_Return[] =
{
    AFFINEANIMCMD_FRAME( -0x2,  -0x2, 0, 18),
    AFFINEANIMCMD_FRAME(-0x10, -0x10, 0, 15),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_Battler_HorizontalSquishLoop[] =
{
    AFFINEANIMCMD_FRAME(0xA0, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME( 0x4,   0x0, 0, 8),
    AFFINEANIMCMD_FRAME(-0x4,   0x0, 0, 8),
    AFFINEANIMCMD_JUMP(1),
};

static const union AffineAnimCmd sAffineAnim_Battler_Grow[] =
{
    AFFINEANIMCMD_FRAME(0x2, 0x2, 0, 20),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_Battler_Shrink[] =
{
    AFFINEANIMCMD_FRAME(-0x2, -0x2, 0, 20),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_Battler_BigToSmall[] =
{
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(-0x10, -0x10, 0, 9),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_Battler_GrowLarge[] =
{
    AFFINEANIMCMD_FRAME(0x4, 0x4, 0, 63),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_Battler_TipRight[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, -3, 5),
    AFFINEANIMCMD_FRAME(0x0, 0x0,  3, 5),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gAffineAnims_BattleSpritePlayerSide[] =
{
    [BATTLER_AFFINE_NORMAL] = sAffineAnim_Battler_Normal,
    [BATTLER_AFFINE_EMERGE] = sAffineAnim_Battler_Emerge,
    [BATTLER_AFFINE_RETURN] = sAffineAnim_Battler_Return,
    sAffineAnim_Battler_HorizontalSquishLoop,
    sAffineAnim_Battler_Grow,
    sAffineAnim_Battler_Shrink,
    sAffineAnim_Battler_GrowLarge,
    sAffineAnim_Battler_TipRight,
    sAffineAnim_Battler_BigToSmall,
};

static const union AffineAnimCmd sAffineAnim_Battler_SpinShrink[] =
{
    AFFINEANIMCMD_FRAME(-0x4, -0x4, 4, 63),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_Battler_TipLeft[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0,  3, 5),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -3, 5),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_Battler_RotateUpAndBack[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, -5, 20),
    AFFINEANIMCMD_FRAME(0x0, 0x0,  0, 20),
    AFFINEANIMCMD_FRAME(0x0, 0x0,  5, 20),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_Battler_Spin[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, 9, 110),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gAffineAnims_BattleSpriteOpponentSide[] =
{
    [BATTLER_AFFINE_NORMAL] = sAffineAnim_Battler_Normal,
    [BATTLER_AFFINE_EMERGE] = sAffineAnim_Battler_Emerge,
    [BATTLER_AFFINE_RETURN] = sAffineAnim_Battler_Return,
    sAffineAnim_Battler_HorizontalSquishLoop,
    sAffineAnim_Battler_Grow,
    sAffineAnim_Battler_Shrink,
    sAffineAnim_Battler_SpinShrink,
    sAffineAnim_Battler_TipLeft,
    sAffineAnim_Battler_RotateUpAndBack,
    sAffineAnim_Battler_BigToSmall,
    sAffineAnim_Battler_Spin,
};

const union AffineAnimCmd *const gAffineAnims_BattleSpriteContest[] =
{
    [BATTLER_AFFINE_NORMAL] = sAffineAnim_Battler_Flipped,
    [BATTLER_AFFINE_EMERGE] = sAffineAnim_Battler_Emerge,
    [BATTLER_AFFINE_RETURN] = sAffineAnim_Battler_Return,
    sAffineAnim_Battler_HorizontalSquishLoop,
    sAffineAnim_Battler_Grow,
    sAffineAnim_Battler_Shrink,
    sAffineAnim_Battler_SpinShrink,
    sAffineAnim_Battler_TipLeft,
    sAffineAnim_Battler_RotateUpAndBack,
    sAffineAnim_Battler_BigToSmall,
    sAffineAnim_Battler_Spin,
};


static const union AnimCmd sAnim_MonPic_0[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MonPic_1[] =
{
    ANIMCMD_FRAME(1, 0),
    ANIMCMD_END,
};

const union AnimCmd *const gAnims_MonPic[MAX_MON_PIC_FRAMES] =
{
    sAnim_MonPic_0,
    sAnim_MonPic_1,
};

const union AnimCmd *const gAnims_Trainer[] ={
    sAnim_GeneralFrame0,
    sAnim_GeneralFrame0,
};

#if !TESTING
const struct Trainer gTrainers[DIFFICULTY_COUNT][TRAINERS_COUNT] =
{
#if IS_FRLG
#include "data/trainers_frlg.h"
#else
#include "data/trainers.h"
    // FRLG trainers for Kanto new game (IDs remapped to 900+ range via opponents.h)
#include "data/trainers_frlg.h"
    // Explicit FRLG rival trainers (indices 871-873 in gTrainers array)
    [DIFFICULTY_NORMAL][TRAINER_RIVAL_OAKS_LAB_SQUIRTLE] = {
        .trainerName = _("TERRY"),
        .trainerClass = TRAINER_CLASS_RIVAL_EARLY_FRLG,
        .trainerPic = TRAINER_PIC_RIVAL_EARLY_FRLG,
        .gender = TRAINER_GENDER_MALE,
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
        .battleType = TRAINER_BATTLE_TYPE_SINGLES,
        .aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY,
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
        .partySize = 1,
        .party = (const struct TrainerMon[]) {{
            .species = SPECIES_SQUIRTLE,
            .gender = TRAINER_MON_RANDOM_GENDER,
            .iv = TRAINER_PARTY_IVS(0, 0, 0, 0, 0, 0),
            .lvl = 5,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            .moves = {MOVE_TACKLE, MOVE_TAIL_WHIP},
        }},
    },
    [DIFFICULTY_NORMAL][TRAINER_RIVAL_OAKS_LAB_BULBASAUR] = {
        .trainerName = _("TERRY"),
        .trainerClass = TRAINER_CLASS_RIVAL_EARLY_FRLG,
        .trainerPic = TRAINER_PIC_RIVAL_EARLY_FRLG,
        .gender = TRAINER_GENDER_MALE,
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
        .battleType = TRAINER_BATTLE_TYPE_SINGLES,
        .aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY,
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
        .partySize = 1,
        .party = (const struct TrainerMon[]) {{
            .species = SPECIES_BULBASAUR,
            .gender = TRAINER_MON_RANDOM_GENDER,
            .iv = TRAINER_PARTY_IVS(0, 0, 0, 0, 0, 0),
            .lvl = 5,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            .moves = {MOVE_TACKLE, MOVE_GROWL},
        }},
    },
    [DIFFICULTY_NORMAL][TRAINER_RIVAL_OAKS_LAB_CHARMANDER] = {
        .trainerName = _("TERRY"),
        .trainerClass = TRAINER_CLASS_RIVAL_EARLY_FRLG,
        .trainerPic = TRAINER_PIC_RIVAL_EARLY_FRLG,
        .gender = TRAINER_GENDER_MALE,
        .encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE,
        .battleType = TRAINER_BATTLE_TYPE_SINGLES,
        .aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY,
        .multiTeamSize = MULTI_TEAM_SIZE_FULL,
        .partySize = 1,
        .party = (const struct TrainerMon[]) {{
            .species = SPECIES_CHARMANDER,
            .gender = TRAINER_MON_RANDOM_GENDER,
            .iv = TRAINER_PARTY_IVS(0, 0, 0, 0, 0, 0),
            .lvl = 5,
            .ball = POKEBALL_COUNT,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            .moves = {MOVE_SCRATCH, MOVE_GROWL},
        }},
    },
#endif
};
#endif

#include "data/text/follower_messages.h"
