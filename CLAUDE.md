# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Context

This is a custom ROM hack built on **pokeemerald-expansion** — a GBA ROM hack base for Pokémon Emerald (and optionally FireRed/LeafGreen) that supports Pokémon through Gen 9. The project adds dual Hoenn+Kanto regions, a steal mechanic, and region selection at new game start. It is not a standalone game — it compiles to `pokeemerald.gba`.

## Build Commands

```bash
# Standard Emerald build
make

# Parallel build (faster — use nproc on Linux, sysctl -n hw.ncpu on Mac)
make -j$(sysctl -n hw.ncpu)

# FireRed / LeafGreen builds
make firered -j$(sysctl -n hw.ncpu)
make leafgreen -j$(sysctl -n hw.ncpu)

# Debug build (includes debug symbols and -Og)
make debug

# Run all tests
make check -j

# Run specific tests by name prefix
make check TESTS="Stun Spore"

# Build test ROM for viewing in mGBA
make pokeemerald-test.elf TESTS="Spikes"

# Clean build artifacts
make tidy        # removes .gba/.elf/obj directories
make clean       # full clean including generated assets
```

**Important**: When switching between Emerald and FRLG builds, run `make clean` first.

**Known issue**: The Homebrew `arm-none-eabi-gcc` 16.1.0 on macOS cannot find `string.h` — this is a pre-existing toolchain problem and not caused by code changes. Use devkitARM or an older gcc if you need a working build.

## Architecture Overview

### Dual-Build System (`IS_FRLG`)

The codebase compiles to either Emerald or FireRed/LeafGreen from the same source via the `IS_FRLG` compile-time constant (0=Emerald, 1=FRLG), set in `include/constants/global.h` based on whether `FIRERED` or `LEAFGREEN` is defined by the Makefile.

Guard all FRLG-specific code with `#if IS_FRLG` / `#if !IS_FRLG`. Flag and variable constants have parallel definitions in:
- `include/constants/flags.h` — Emerald flags (the `#else` branch after `#if IS_FRLG`)
- `include/constants/flags_frlg.h` — FRLG flags (included when `IS_FRLG`)

Both files must stay in sync for any flag added to the game.

### Configuration System (`include/config/`)

All tuneable mechanics live in `include/config/*.h`. Key files:
- `battle.h` — battle mechanics, generation-accuracy settings, feature flags like `B_FLAG_TERA_ORB_CHARGED`
- `pokemon.h` — Pokémon data accuracy (stats, types, learnsets) per generation
- `general.h` — `GEN_3` through `GEN_9` constants, `GEN_LATEST`, debug/logging settings
- `species_enabled.h` — toggles which Pokémon generations are included

Most config values are set to `GEN_LATEST` meaning they follow the most recent game behavior. Numeric generation constants: `GEN_3=2`, `GEN_4=3`, … `GEN_9=8`, `GEN_LATEST=GEN_9`.

### Script System (`.inc` files)

Maps and events are driven by a custom assembly-like scripting language in `.inc` files under `data/maps/<MapName>/`. Each map directory contains:
- `scripts.inc` — NPC dialogues, event handlers, warp logic
- `events.inc` — NPC/object event placements (`object_event` macros)
- `map.json` — Porymap source-of-truth for map metadata; maps require a `region` field (`REGION_HOENN` or `REGION_KANTO`) to be included in the correct build

Key script conventions:
- `.s` files (tabs) include map `.inc` files
- `map_script MAP_SCRIPT_ON_FRAME_TABLE` + `map_script_2 VAR, VALUE, Label` = conditional per-frame trigger
- `map_script MAP_SCRIPT_ON_TRANSITION` = runs once when entering a map
- `lock`/`release` bracket NPC interaction; `lockall`/`releaseall` for cutscenes
- NPC scripts end with `release\nend`; warp scripts end with `waitstate\nrelease\nend`
- `call` pushes a return address; `goto` transfers permanently; `return` pops; `end` terminates regardless of call stack depth
- `call_if_set FLAG, Label` / `goto_if_unset FLAG, Label` are the conditional branch idioms

Common movement sequences live in `data/scripts/movement.inc` (e.g., `Common_Movement_WalkInPlaceFasterUp`).

### Event/Save Data

- **Flags** (`FlagSet`/`FlagGet` in C, `setflag`/`checkflag` in scripts): persistent booleans in the save block. Defined in `include/constants/flags.h`. Custom flags for this project start at `0x20`.
- **Vars** (`VarSet`/`VarGet`, `setvar`/`getvar`): 16-bit persistent values. `VAR_SS_TIDAL_STATE`, `VAR_RESULT`, etc.
- **EWRAM globals**: Use `EWRAM_DATA u8 gMyVar = 0;` for data that must survive `NewGameInitData()` but is reset on power cycle. `InitEventData()` clears all flags/vars, so EWRAM is needed for values that must outlive save initialization (e.g., `gNewGameRegion`).
- **Special vars** (`gSpecialVar_0x8000`–`gSpecialVar_0x800B`): scratch registers for script↔C communication; `VAR_RESULT` holds multichoice/yesno results.

### Task System

Async game logic runs via `gTasks[]` — an array of `struct Task` with a `func` pointer and 16 `data` words. Create with `CreateTask(func, priority)`, advance state by setting `gTasks[taskId].func = NextFunc`. The main menu Birch speech (`src/main_menu.c`) is a canonical example of a multi-step task chain.

### Memory Sections

- `EWRAM_DATA` — external RAM, zero-initialized BSS; survives function calls but not power cycles
- `IWRAM_DATA` — fast internal RAM BSS
- `EWRAM_INIT` / `IWRAM_INIT` — initialized data sections

### Multichoice System

Multichoice menus are defined in `src/data/script_menu.h` as `static const struct MenuAction[]` arrays, registered in `sMultichoiceLists[]` using designated initializers (`[MULTI_ID] = MULTICHOICE(array)`). IDs are an `enum` in `include/constants/script_menu.h`. Scripts invoke them with `multichoicedefault x, y, MULTI_ID, default, FALSE`.

### SS Tidal State Machine

`VAR_SS_TIDAL_STATE` (enum `SSTidalState` in `include/constants/field_specials.h`) drives all ferry logic. OnFrame map scripts in `data/maps/SSTidalCorridor/scripts.inc` react to state transitions. Custom Kanto/Hoenn voyage states (`SS_TIDAL_BOARD_KANTO`, `SS_TIDAL_LAND_KANTO`, `SS_TIDAL_BOARD_HOENN`, `SS_TIDAL_LAND_HOENN`) were appended to the enum. `special SetSSTidalFlag` sets `FLAG_SYS_CRUISE_MODE`; `special ResetSSTidalFlag` clears it.

### Testing System

Tests live in `test/` and follow a `GIVEN`/`WHEN`/`SCENE` DSL (see `include/test/battle.h`). The test runner uses mGBA headless. `ASSUMPTIONS { ASSUME(GetMoveEffect(MOVE_X) == EFFECT_Y); }` guards skip a test file if game data doesn't match. Tests are compiled separately (`TEST=1`) and do not ship in the final ROM.

## Code Style

- C files: 4 spaces, no tabs
- Assembly/script `.s`/`.inc` files: tabs
- Functions/structs: `PascalCase`; variables/fields: `camelCase`; globals: `g` prefix; statics: `s` prefix; constants/macros: `CAPS_WITH_UNDERSCORES`
- Braces on their own line (Allman style); `switch` cases left-aligned with the `switch` block
- Comments explain WHY, not WHAT; inline `// tag` comments on data arrays for enum values

## Key Custom Additions (This ROM Hack)

### New Game Region Selection

- **`src/new_game.c` / `include/new_game.h`**: `gNewGameRegion` (EWRAM) stores the player's region choice. `WarpToTruck()` redirects: Kanto/Yellow/Johto → `MAP_PALLET_TOWN_PLAYERS_HOUSE_2F` (FRLG 2F room), Hoenn → truck.
- **`src/main_menu.c`**: `Task_NewGameBirchSpeech_AskRegion` chain offers HOENN / KANTO / YELLOW / JOHTO (4-item menu), then Hard Mode YES/NO.
- **`NewGameInitData()`** (src/new_game.c ~line 253):
  - Kanto/Yellow: runs `EventScript_ResetAllMapFlagsFrlg`, sets `FLAG_TRAVELED_TO_HOENN`, `FLAG_TRAVELED_TO_JOHTO`, `FLAG_HIDE_OAK_IN_PALLET_TOWN`; Yellow also sets `FLAG_YELLOW_MODE`
  - Johto: runs `EventScript_ResetAllMapFlags`, sets `FLAG_TRAVELED_TO_KANTO`, `FLAG_TRAVELED_TO_HOENN`
  - Rival name: Kanto/Yellow → `_("GARY")` (inline literal); others → default (May/Brendan)

### FRLG Objects in Emerald Build

**Critical**: `include/constants/flags.h` stubs out all FRLG object-hide flags as `0` (FLAG_NONE) because the original Emerald has no FRLG maps. FLAG_NONE means the object engine always shows those objects. Fix: flags.h now gives these real values mirroring `flags_frlg.h`:

| Flag | Value | Controls |
|------|-------|---------|
| `FLAG_HIDE_BULBASAUR_BALL` | 0x028 | Bulbasaur Pokéball on Oak's lab desk |
| `FLAG_HIDE_SQUIRTLE_BALL` | 0x029 | Squirtle Pokéball |
| `FLAG_HIDE_CHARMANDER_BALL` | 0x02A | Charmander Pokéball |
| `FLAG_HIDE_OAK_IN_HIS_LAB` | 0x02B | Oak's position behind the desk |
| `FLAG_HIDE_OAK_IN_PALLET_TOWN` | 0x02C | Oak's outdoor sprite in Pallet Town |
| `FLAG_HIDE_RIVAL_IN_LAB` | 0x02D | Gary/Blue in Oak's lab post-battle |
| `FLAG_HIDE_PEWTER_CITY_GYM_GUIDE` | 0x02E | Guide NPC outside Brock's gym |
| `FLAG_HIDE_DOME_FOSSIL` / `FLAG_HIDE_HELIX_FOSSIL` | 0x02F / 0x030 | Mt. Moon fossils |

The `flags_frlg.h` `#ifndef` guards now correctly skip (since flags.h defines the same values), keeping both builds in sync.

**FRLG NPC color palettes** (`src/event_object_movement.c`): Moved `OBJ_EVENT_PAL_TAG_NPC_BLUE/PINK/GREEN/WHITE` (+ reflections) OUTSIDE the `#if IS_FRLG` guard. Without this, FRLG NPCs render with garbage palettes in Emerald builds.

### Kanto/FRLG Map Setup

- **`data/maps/PalletTown_Frlg/events.inc`**: Object 4 — warp woman NPC at (7,13). Script offers Hoenn warp (sets `VAR_LITTLEROOT_TOWN_STATE=4`, hides trucks, warps to Littleroot) or Johto warp (warps to `MAP_NEW_BARK_TOWN`).
- **`data/maps/LittlerootTown/events.inc`**: Object 9 — warp man NPC at (10,13). Script offers Kanto warp (warps to Pallet Town outdoor).
- **`data/maps/LittlerootTown/scripts.inc`**: Must set `FLAG_HIDE_LITTLEROOT_TOWN_BRENDANS_HOUSE_TRUCK` (0x2F9) and `FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_TRUCK` (0x2FA) when warping here from Kanto; trucks are controlled by these flags (set = hidden).

### Cross-Region Ferries

- **`data/maps/SlateportCity_Harbor/scripts.inc`**: SS Tidal extended with Kanto destination (no `FLAG_SYS_GAME_CLEAR` gate). Uses `MULTI_SSTIDAL_SLATEPORT_NO_BF_KANTO` / `MULTI_SSTIDAL_SLATEPORT_WITH_BF_KANTO`.
- **`data/maps/VermilionCity_Frlg/scripts.inc` + `events.inc`**: Sailor NPC (object event 9) at (28,33) offers Hoenn ferry — no champion gate.

### Cross-Region Starters

- **`data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc`**: Birch offers Kanto starters (Bulbasaur/Charmander/Squirtle) when `FLAG_TRAVELED_TO_KANTO && !FLAG_RECEIVED_KANTO_STARTER`.
- **`data/maps/PalletTown_ProfessorOaksLab_Frlg/scripts.inc`**: Oak offers Hoenn starters (Treecko/Torchic/Mudkip) when `FLAG_TRAVELED_TO_HOENN && !FLAG_RECEIVED_HOENN_STARTER`. Also: `EventScript_RivalTakesStarter` sets all three ball hide flags after rival picks; `EventScript_EndRivalBattle` sets `FLAG_HIDE_RIVAL_IN_LAB`.

### Johto Tilesets

Johto tile data lives in `data/tilesets/primary/johto_*/` and `data/tilesets/secondary/*/`. The C hookup is in the standard tileset files.

**Primary tilesets** (fully implemented — `src/tileset_anims.c` / `src/data/tilesets/headers.h`):
| Struct | Directory | Animations |
|--------|-----------|-----------|
| `gTileset_Johto_General` | `primary/johto_general` | flower (5f), sandwatersedge (8f), water_current (8f) |
| `gTileset_Johto_Building` | `primary/johto_building` | none |
| `gTileset_Johto_NorthEast` | `primary/johto_north_east` | same as General |
| `gTileset_Johto_NorthWest` | `primary/johto_north_west` | same as General |
| `gTileset_Johto_South` | `primary/johto_south` | same as General |

Animation tile offsets match FRLG General: flower→`TILE_OFFSET_4BPP(508)`, sandwatersedge→`TILE_OFFSET_4BPP(464)`, water_current→`TILE_OFFSET_4BPP(416)`.

**Secondary tilesets** (fully implemented): AzaleaTown, AzaleaTown_Gym, BellchimeTrail, Blackthorn, BlackthornGym, BurnedTower, Cave_DragonsDen, Cave_Gray, Cave_Ice, CherrygroveCity, CianwoodCity, CianwoodCity_Gym, Ecruteak_City, EcruteakCity_Gym, EcruteakTheater, Goldenrod, GoldenrodCity_TrainStation, GoldenrodUnderground{Rocket,Tunnel,_Storage}, JohtoBikeShop, JohtoMart, MahoganyTown, NationalPark, NewBarkTown, OlivineCity, RuinsOfAlph{Writing,_B1F,_Outside}, SafariZoneJohto, VioletCity, WhirlIslands.

**Stubs remaining** (no tile data yet — `src/johto_tileset_stubs.c`): Barn, Cafe, Cave_Default, DragonsDen_Shrine, Gate_Standard, House_2, House_Lab, Kanto_General, Kanto_PokemonCenter, KurtsHouse, Lighthouse, MtSilverSnow, PlayersHouse, PokemonCenter_White, PortIndoor, PowerPlant_GeneratorRoom, Route32, Route38_Farmland, SafariZone_Entrance, ShopRooftop, TrainerSchool, Unused3.

**Palette count gotcha**: Some secondary tileset directories have extra `.pla` (typo) and `_over.pal` (override variant) files alongside the real numbered palettes. Count only `^[0-9]+\.pal$` files when adding a new tileset.

### Custom Flags

All custom flags are in `include/constants/flags.h` (Emerald) and mirrored in `include/constants/flags_frlg.h`:

| Flag | Value | Purpose |
|------|-------|---------|
| `FLAG_ENABLE_STEALING` | 0x20 | Enables catching trainer Pokémon |
| `FLAG_TRAVELED_TO_KANTO` | 0x21 | Set when player first reaches Kanto |
| `FLAG_TRAVELED_TO_HOENN` | 0x22 | Set when player first reaches Hoenn |
| `FLAG_RECEIVED_KANTO_STARTER` | 0x23 | Prevents Birch re-offering Kanto starter |
| `FLAG_RECEIVED_HOENN_STARTER` | 0x24 | Prevents Oak re-offering Hoenn starter |
| `FLAG_YELLOW_MODE` | 0x25 | Yellow-mode new game (Pikachu start) |
| `FLAG_PIKACHU_RECEIVED` | 0x494 | Set after Oak gives Pikachu in Yellow mode |
| `FLAG_HARD_MODE` | 0x493 | Hard mode enabled |
| `FLAG_TRAVELED_TO_JOHTO` | 0x495 | Set when player first reaches Johto |
| `FLAG_RECEIVED_JOHTO_STARTER` | 0x496 | Prevents Elm re-offering Johto starter |
| `FLAG_JOHTO_BADGE01_GET`–`FLAG_JOHTO_BADGE08_GET` | 0x497–0x49E | Johto gym badges |
