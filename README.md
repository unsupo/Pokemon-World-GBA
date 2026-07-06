# Pokémon World GBA

A custom GBA ROM hack built on **pokeemerald-expansion** that combines the Hoenn, Kanto, Johto, and Sinnoh regions into a single game. Start your journey in any region, travel between them by ferry, collect starters from multiple professors, and battle across all four worlds.

## Features

### Multi-Region New Game
Choose your starting region when beginning a new game:
- **Hoenn** — start in Littleroot Town with Treecko / Torchic / Mudkip
- **Kanto** — start in Pallet Town with Bulbasaur / Charmander / Squirtle
- **Yellow** — start in Pallet Town with Pikachu (Oak's gift)
- **Johto** — start in New Bark Town

### Cross-Region Travel
- **SS Tidal** (Slateport Harbor) sails to Kanto in addition to its standard Hoenn routes — no Champion gate required
- **Vermilion City** sailor offers a return voyage to Hoenn
- Warp NPCs in Pallet Town and Littleroot Town connect Hoenn ↔ Kanto ↔ Johto

### Cross-Region Starters
- Professor Birch (Littleroot) offers Kanto starters once you've traveled to Kanto
- Professor Oak (Pallet Town FRLG lab) offers Hoenn starters once you've traveled to Hoenn
- Each set can only be claimed once per save

### Trainer Steal Mechanic
With `FLAG_ENABLE_STEALING` set, players can catch Pokémon belonging to trainers — like in a certain fan-made game.

### Hard Mode
Optional hard mode flag available at new game start.

### Scaled Gym Leader Rematches
Gym leaders scale across 7 difficulty tiers (`trainerbattle_scaled` macro) using a global badge tier system. Hoenn gym leaders have full rematch rosters through all tiers.

### Tileset Support
- Full Johto tilesets (primary + secondary): New Bark Town, Azalea Town, Goldenrod City, Ecruteak City, Olivine City, and more
- Full Kanto tileset set: Pallet Town, Pewter City, Cerulean City, Vermilion City, Celadon City, Fuchsia City, Saffron City, Viridian City, and more
- Sinnoh secondary tilesets: Canalave City, Celestic Town, Hearthome City, Jubilife City, and more

### Other
- Johto badge flags (`FLAG_JOHTO_BADGE01_GET` – `FLAG_JOHTO_BADGE08_GET`)
- All custom flags and vars are consistent across Emerald and FRLG builds
- Dual-build system: compiles to either `pokeemerald.gba` (Hoenn/Johto base) or FireRed/LeafGreen via the `IS_FRLG` constant

## Building

```bash
# Standard Emerald build
make

# Parallel build (faster)
make -j$(sysctl -n hw.ncpu)     # macOS
make -j$(nproc)                  # Linux

# FRLG builds
make firered -j$(sysctl -n hw.ncpu)
make leafgreen -j$(sysctl -n hw.ncpu)

# Clean build artifacts
make tidy    # removes .gba/.elf/obj dirs
make clean   # full clean including generated assets
```

**Toolchain:** Requires `arm-none-eabi-gcc`. The Homebrew `arm-none-eabi-gcc` 16.1.0 on macOS has a pre-existing bug where it cannot find `string.h` — use [devkitARM](https://devkitpro.org/wiki/Getting_Started) or an older arm-none-eabi-gcc instead.

## Credits

This project is built on top of the following open-source projects — huge thanks to their contributors:

| Project | Use |
|---|---|
| [rh-hideout/pokeemerald-expansion](https://github.com/rh-hideout/pokeemerald-expansion) | GBA Emerald + FireRed/LeafGreen engine base (Gen 1–9 mechanics, dual-build system) |
| [pret/pokeemerald](https://github.com/pret/pokeemerald) | Original Emerald decompilation that pokeemerald-expansion builds on |
| [PokemonHnS-Development/pokemonHnS](https://github.com/PokemonHnS-Development/pokemonHnS) | Johto and Kanto tileset graphics |
| [LiderMorti00/Sinnoh-pokeemerald-expansion](https://github.com/LiderMorti00/Sinnoh-pokeemerald-expansion) | Sinnoh tileset graphics |
