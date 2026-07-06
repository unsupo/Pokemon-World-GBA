#include "global.h"
#include "clock.h"
#include "new_game.h"
#include "random.h"
#include "pokemon.h"
#include "roamer.h"
#include "pokemon_size_record.h"
#include "script.h"
#include "lottery_corner.h"
#include "play_time.h"
#include "mauville_old_man.h"
#include "match_call.h"
#include "lilycove_lady.h"
#include "load_save.h"
#include "pokeblock.h"
#include "dewford_trend.h"
#include "berry.h"
#include "rtc.h"
#include "easy_chat.h"
#include "event_data.h"
#include "money.h"
#include "trainer_hill.h"
#include "trainer_tower.h"
#include "tv.h"
#include "coins.h"
#include "text.h"
#include "overworld.h"
#include "mail.h"
#include "battle_records.h"
#include "item.h"
#include "pokedex.h"
#include "apprentice.h"
#include "frontier_util.h"
#include "pokedex.h"
#include "save.h"
#include "link_rfu.h"
#include "main.h"
#include "contest.h"
#include "item_menu.h"
#include "pokemon_storage_system.h"
#include "pokemon_jump.h"
#include "decoration_inventory.h"
#include "secret_base.h"
#include "string_util.h"
#include "event_scripts.h"
#include "player_pc.h"
#include "field_specials.h"
#include "berry_powder.h"
#include "mystery_gift.h"
#include "union_room_chat.h"
#include "constants/map_groups.h"
#include "constants/items.h"
#include "difficulty.h"
#include "follower_npc.h"

extern const u8 EventScript_ResetAllMapFlags[];
extern const u8 EventScript_ResetAllMapFlagsFrlg[];
extern const u8 EventScript_ResetAllMapFlagsJohto[];

static void ClearFrontierRecord(void);
static void WarpToTruck(void);
static void ResetMiniGamesRecords(void);
static void ResetItemFlags(void);
static void ResetDexNav(void);

EWRAM_DATA bool8 gDifferentSaveFile = FALSE;
EWRAM_DATA bool8 gEnableContestDebugging = FALSE;
EWRAM_DATA u8 gNewGameRegion = STARTING_REGION_HOENN;
EWRAM_DATA u8 gNewGameAvatarStyle = 0;
EWRAM_DATA u8 gNewGameHardMode = 0;

static const struct ContestWinner sContestWinnerPicDummy =
{
    .monName = _(""),
    .trainerName = _("")
};

void SetTrainerId(u32 trainerId, u8 *dst)
{
    dst[0] = trainerId;
    dst[1] = trainerId >> 8;
    dst[2] = trainerId >> 16;
    dst[3] = trainerId >> 24;
}

u32 GetTrainerId(u8 *trainerId)
{
    return (trainerId[3] << 24) | (trainerId[2] << 16) | (trainerId[1] << 8) | (trainerId[0]);
}

void CopyTrainerId(u8 *dst, u8 *src)
{
    s32 i;
    for (i = 0; i < TRAINER_ID_LENGTH; i++)
        dst[i] = src[i];
}

static void InitPlayerTrainerId(void)
{
    u32 trainerId = (Random() << 16) | GetGeneratedTrainerIdLower();
    SetTrainerId(trainerId, gSaveBlock2Ptr->playerTrainerId);
}

// L=A isnt set here for some reason.
static void SetDefaultOptions(void)
{
    gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_MID;
    gSaveBlock2Ptr->optionsWindowFrameType = 0;
    gSaveBlock2Ptr->optionsSound = OPTIONS_SOUND_MONO;
    gSaveBlock2Ptr->optionsBattleStyle = OPTIONS_BATTLE_STYLE_SHIFT;
    gSaveBlock2Ptr->optionsBattleSceneOff = FALSE;
    gSaveBlock2Ptr->regionMapZoom = FALSE;
}

static void ClearPokedexFlags(void)
{
    gUnusedPokedexU8 = 0;
    memset(&gSaveBlock1Ptr->dexCaught, 0, sizeof(gSaveBlock1Ptr->dexCaught));
    memset(&gSaveBlock1Ptr->dexSeen, 0, sizeof(gSaveBlock1Ptr->dexSeen));
}

void ClearAllContestWinnerPics(void)
{
    s32 i;

    ClearContestWinnerPicsInContestHall();

    // Clear Museum paintings
    for (i = MUSEUM_CONTEST_WINNERS_START; i < NUM_CONTEST_WINNERS; i++)
        gSaveBlock1Ptr->contestWinners[i] = sContestWinnerPicDummy;
}

static void ClearFrontierRecord(void)
{
    CpuFill32(0, &gSaveBlock2Ptr->frontier, sizeof(gSaveBlock2Ptr->frontier));

    gSaveBlock2Ptr->frontier.opponentNames[0][0] = EOS;
    gSaveBlock2Ptr->frontier.opponentNames[1][0] = EOS;
}

static void WarpToTruck(void)
{
    if (gNewGameRegion == STARTING_REGION_KANTO || gNewGameRegion == STARTING_REGION_YELLOW)
        SetWarpDestination(MAP_GROUP(MAP_PALLET_TOWN_PLAYERS_HOUSE_2F), MAP_NUM(MAP_PALLET_TOWN_PLAYERS_HOUSE_2F), WARP_ID_NONE, 6, 6);
    else if (gNewGameRegion == STARTING_REGION_JOHTO)
        SetWarpDestination(MAP_GROUP(MAP_NEW_BARK_TOWN), MAP_NUM(MAP_NEW_BARK_TOWN), WARP_ID_NONE, 20, 13);
    else if (gNewGameRegion == STARTING_REGION_SINNOH)
        SetWarpDestination(MAP_GROUP(MAP_TWINLEAF_TOWN_MAIN_HOUSE_2F), MAP_NUM(MAP_TWINLEAF_TOWN_MAIN_HOUSE_2F), WARP_ID_NONE, 7, 2);
    else
        SetWarpDestination(MAP_GROUP(MAP_INSIDE_OF_TRUCK), MAP_NUM(MAP_INSIDE_OF_TRUCK), WARP_ID_NONE, -1, -1);
    WarpIntoMap();
}

void Sav2_ClearSetDefault(void)
{
    ClearSav2();
    SetDefaultOptions();
}

void ResetMenuAndMonGlobals(void)
{
    gDifferentSaveFile = FALSE;
    ResetPokedexScrollPositions();
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    ResetBagScrollPositions();
    ResetPokeblockScrollPositions();
}

void NewGameInitData(void)
{
    u8 rivalName[PLAYER_NAME_LENGTH + 1];
    DebugPrintf("NewGameInitData: START");
    if (gSaveFileStatus == SAVE_STATUS_EMPTY || gSaveFileStatus == SAVE_STATUS_CORRUPT)
        RtcReset();
    DebugPrintf("NewGameInitData: StringCopy-rivalName");
    memcpy(rivalName, gSaveBlock1Ptr->rivalName, PLAYER_NAME_LENGTH + 1);
    rivalName[PLAYER_NAME_LENGTH] = EOS; // ensure termination if source had no EOS
    DebugPrintf("NewGameInitData: ZeroPartyMons");
    gDifferentSaveFile = TRUE;
    gSaveBlock2Ptr->encryptionKey = 0;
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    DebugPrintf("NewGameInitData: ResetPokedex");
    ResetPokedex();
    DebugPrintf("NewGameInitData: ClearFrontierRecord");
    ClearFrontierRecord();
    DebugPrintf("NewGameInitData: ClearSav1");
    ClearSav1();
    DebugPrintf("NewGameInitData: ClearSav3");
    ClearSav3();
    DebugPrintf("NewGameInitData: ClearAllMail");
    ClearAllMail();
    DebugPrintf("NewGameInitData: InitPlayerTrainerId");
    gSaveBlock2Ptr->specialSaveWarpFlags = 0;
    gSaveBlock2Ptr->gcnLinkFlags = 0;
    InitPlayerTrainerId();
    DebugPrintf("NewGameInitData: PlayTimeCounter_Reset");
    PlayTimeCounter_Reset();
    DebugPrintf("NewGameInitData: ClearPokedexFlags");
    ClearPokedexFlags();
    DebugPrintf("NewGameInitData: InitEventData");
    InitEventData();
    DebugPrintf("NewGameInitData: VarSet+ClearTV");
    VarSet(VAR_AVATAR_STYLE, AVATAR_STYLE_HOENN);
    ClearTVShowData();
    DebugPrintf("NewGameInitData: ResetGabbyAndTy");
    ResetGabbyAndTy();
    DebugPrintf("NewGameInitData: ClearSecretBases");
    ClearSecretBases();
    DebugPrintf("NewGameInitData: ClearBerryTrees");
    ClearBerryTrees();
    DebugPrintf("NewGameInitData: SetMoney");
    SetMoney(&gSaveBlock1Ptr->money, 3000);
    SetCoins(0);
    DebugPrintf("NewGameInitData: ResetGameStats");
    ResetLinkContestBoolean();
    ResetGameStats();
    DebugPrintf("NewGameInitData: ClearAllContestWinnerPics");
    ClearAllContestWinnerPics();
    DebugPrintf("NewGameInitData: ClearPlayerLinkBattleRecords");
    ClearPlayerLinkBattleRecords();
    DebugPrintf("NewGameInitData: InitSeedotSizeRecord");
    InitSeedotSizeRecord();
    InitLotadSizeRecord();
    DebugPrintf("NewGameInitData: ResetPokemonStorageSystem");
    gPartiesCount[B_TRAINER_PLAYER] = 0;
    ZeroPlayerPartyMons();
    ResetPokemonStorageSystem();
    DebugPrintf("NewGameInitData: DeactivateAllRoamers");
    DeactivateAllRoamers();
    DebugPrintf("NewGameInitData: registeredItem");
    gSaveBlock1Ptr->registeredItem = ITEM_NONE;
    DebugPrintf("NewGameInitData: ClearBag");
    ClearBag();
    DebugPrintf("NewGameInitData: NewGameInitPCItems");
    NewGameInitPCItems();
    DebugPrintf("NewGameInitData: ClearPokeblocks");
    ClearPokeblocks();
    DebugPrintf("NewGameInitData: ClearDecorationInventories");
    ClearDecorationInventories();
    DebugPrintf("NewGameInitData: InitEasyChatPhrases");
    InitEasyChatPhrases();
    DebugPrintf("NewGameInitData: SetMauvilleOldMan");
    SetMauvilleOldMan();
    DebugPrintf("NewGameInitData: InitDewfordTrend");
    InitDewfordTrend();
    DebugPrintf("NewGameInitData: ResetFanClub");
    ResetFanClub();
    DebugPrintf("NewGameInitData: ResetLotteryCorner");
    ResetLotteryCorner();
    DebugPrintf("NewGameInitData: UpdateDailySeed");
    UpdateDailySeed();
    DebugPrintf("NewGameInitData: WarpToTruck");
    WarpToTruck();
    DebugPrintf("NewGameInitData: WarpToTruck done warp=%d/%d", gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum);
    if (gNewGameRegion == STARTING_REGION_KANTO || gNewGameRegion == STARTING_REGION_YELLOW)
    {
        RunScriptImmediately(EventScript_ResetAllMapFlags);
        RunScriptImmediately(EventScript_ResetAllMapFlagsFrlg);
        RunScriptImmediately(EventScript_ResetAllMapFlagsJohto);
        DebugPrintf("NewGameInitData: ResetAllMapFlags done");
        if (gNewGameRegion == STARTING_REGION_YELLOW)
            FlagSet(FLAG_YELLOW_MODE);
        FlagSet(FLAG_TRAVELED_TO_HOENN);
        FlagSet(FLAG_TRAVELED_TO_JOHTO);
        FlagSet(FLAG_HIDE_OAK_IN_PALLET_TOWN);
    }
    else if (gNewGameRegion == STARTING_REGION_JOHTO)
    {
        RunScriptImmediately(EventScript_ResetAllMapFlags);
        RunScriptImmediately(EventScript_ResetAllMapFlagsFrlg);
        FlagSet(FLAG_TRAVELED_TO_JOHTO);
        FlagSet(FLAG_TRAVELED_TO_KANTO);
        FlagSet(FLAG_TRAVELED_TO_HOENN);
        FlagSet(FLAG_HIDE_OAK_IN_PALLET_TOWN);
    }
    else if (gNewGameRegion == STARTING_REGION_SINNOH)
    {
        RunScriptImmediately(EventScript_ResetAllMapFlags);
        RunScriptImmediately(EventScript_ResetAllMapFlagsFrlg);
        RunScriptImmediately(EventScript_ResetAllMapFlagsJohto);
        FlagSet(FLAG_TRAVELED_TO_KANTO);
        FlagSet(FLAG_TRAVELED_TO_JOHTO);
        FlagSet(FLAG_TRAVELED_TO_HOENN);
        FlagSet(FLAG_HIDE_OAK_IN_PALLET_TOWN);
    }
    else
    {
        RunScriptImmediately(EventScript_ResetAllMapFlags);
        RunScriptImmediately(EventScript_ResetAllMapFlagsFrlg);
        RunScriptImmediately(EventScript_ResetAllMapFlagsJohto);
        FlagSet(FLAG_HIDE_OAK_IN_PALLET_TOWN);
    }
    if (gNewGameHardMode)
        FlagSet(FLAG_HARD_MODE);
    DebugPrintf("NewGameInitData: region=%d StringCopy rivalName", gNewGameRegion);
    if (gNewGameRegion == STARTING_REGION_KANTO || gNewGameRegion == STARTING_REGION_YELLOW)
    {
        static const u8 sRivalName_Gary[] = _("GARY");
        StringCopy(gSaveBlock1Ptr->rivalName, sRivalName_Gary);
    }
    else
        StringCopy(gSaveBlock1Ptr->rivalName, rivalName);
    DebugPrintf("NewGameInitData: ResetMiniGamesRecords");
    ResetMiniGamesRecords();
    DebugPrintf("NewGameInitData: InitUnionRoomChatRegisteredTexts");
    InitUnionRoomChatRegisteredTexts();
    DebugPrintf("NewGameInitData: InitLilycoveLady");
    InitLilycoveLady();
    DebugPrintf("NewGameInitData: ResetAllApprenticeData");
    ResetAllApprenticeData();
    DebugPrintf("NewGameInitData: ClearRankingHallRecords");
    ClearRankingHallRecords();
    DebugPrintf("NewGameInitData: InitMatchCallCounters");
    InitMatchCallCounters();
    DebugPrintf("NewGameInitData: ClearMysteryGift");
    ClearMysteryGift();
    DebugPrintf("NewGameInitData: WipeTrainerNameRecords");
    WipeTrainerNameRecords();
    DebugPrintf("NewGameInitData: ResetTrainerHillResults");
    ResetTrainerHillResults();
    DebugPrintf("NewGameInitData: ResetTrainerTowerResults");
    ResetTrainerTowerResults();
    DebugPrintf("NewGameInitData: ResetContestLinkResults");
    ResetContestLinkResults();
    DebugPrintf("NewGameInitData: SetCurrentDifficultyLevel");
    SetCurrentDifficultyLevel(DIFFICULTY_NORMAL);
    DebugPrintf("NewGameInitData: ResetItemFlags");
    ResetItemFlags();
    DebugPrintf("NewGameInitData: ResetDexNav");
    ResetDexNav();
    DebugPrintf("NewGameInitData: ClearFollowerNPCData");
    ClearFollowerNPCData();
    DebugPrintf("NewGameInitData: DONE");
}

static void ResetMiniGamesRecords(void)
{
    CpuFill16(0, &gSaveBlock2Ptr->berryCrush, sizeof(struct BerryCrush));
    SetBerryPowder(&gSaveBlock2Ptr->berryCrush.berryPowderAmount, 0);
    ResetPokemonJumpRecords();
    CpuFill16(0, &gSaveBlock2Ptr->berryPick, sizeof(struct BerryPickingResults));
}

static void ResetItemFlags(void)
{
#if OW_SHOW_ITEM_DESCRIPTIONS == OW_ITEM_DESCRIPTIONS_FIRST_TIME
    memset(&gSaveBlock3Ptr->itemFlags, 0, sizeof(gSaveBlock3Ptr->itemFlags));
#endif
}

static void ResetDexNav(void)
{
#if USE_DEXNAV_SEARCH_LEVELS == TRUE
    memset(gSaveBlock3Ptr->dexNavSearchLevels, 0, sizeof(gSaveBlock3Ptr->dexNavSearchLevels));
#endif
    gSaveBlock3Ptr->dexNavChain = 0;
}
