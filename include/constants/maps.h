#ifndef GUARD_CONSTANTS_MAPS_H
#define GUARD_CONSTANTS_MAPS_H

#include "map_groups.h"

enum
{
    // Warps using this map will instead use the warp data stored in gSaveBlock1Ptr->dynamicWarp.
    // Used for warps that need to change destinations, e.g. when stepping off an elevator.
    MAP_DYNAMIC = (0x7F | (0x7F << 8)),

    MAP_UNDEFINED = (0xFF | (0xFF << 8)),
};

#define MAP_GROUP(map) (map >> 8)
#define MAP_NUM(map) (map & 0xFF)

// IDs for dynamic warps. Both are used in the dest_warp_id field for warp events, but they
// are never read in practice. A dest_map of MAP_DYNAMIC is used to indicate that a
// dynamic warp should be used, at which point the warp id is ignored. They can be passed
// as the argument to SetWarpDestinationToDynamicWarp, but this argument is unused.
// As only one dynamic warp is saved at a time there's no need to distinguish between them.
#define WARP_ID_SECRET_BASE 0x7E
#define WARP_ID_DYNAMIC     0x7F

// Used to indicate an invalid warp id, for dummy warps or when a warp should
// use the given coordinates rather than the coordinates of a target warp.
#define WARP_ID_NONE (-1)

// Johto maps not yet imported — stub to MAP_DYNAMIC until map data is added
#ifndef MAP_TIN_TOWER_1F
#define MAP_TIN_TOWER_1F                    MAP_DYNAMIC
#endif
#ifndef MAP_TIN_TOWER_ROOF_DAY
#define MAP_TIN_TOWER_ROOF_DAY              MAP_DYNAMIC
#endif
#ifndef MAP_TOHJO_FALLS_CAVERN
#define MAP_TOHJO_FALLS_CAVERN              MAP_DYNAMIC
#endif
#ifndef MAP_RECEPTION_GATE
#define MAP_RECEPTION_GATE                  MAP_DYNAMIC
#endif
#ifndef MAP_GATE_AZALEA_TOWN_ILEX_FOREST
#define MAP_GATE_AZALEA_TOWN_ILEX_FOREST    MAP_DYNAMIC
#endif
#ifndef MAP_GATE_ECRUTEAK_CITY_ROUTE38
#define MAP_GATE_ECRUTEAK_CITY_ROUTE38      MAP_DYNAMIC
#endif
#ifndef MAP_GATE_ECRUTEAK_CITY_ROUTE42
#define MAP_GATE_ECRUTEAK_CITY_ROUTE42      MAP_DYNAMIC
#endif
#ifndef MAP_GATE_ILEX_FOREST_ROUTE34
#define MAP_GATE_ILEX_FOREST_ROUTE34        MAP_DYNAMIC
#endif
#ifndef MAP_GATE_MAHOGANY_TOWN_ROUTE43
#define MAP_GATE_MAHOGANY_TOWN_ROUTE43      MAP_DYNAMIC
#endif
#ifndef MAP_GATE_NATIONAL_PARK
#define MAP_GATE_NATIONAL_PARK              MAP_DYNAMIC
#endif
#ifndef MAP_GATE_ROUTE29_ROUTE46
#define MAP_GATE_ROUTE29_ROUTE46            MAP_DYNAMIC
#endif
#ifndef MAP_GATE_RUINS_OF_ALPH_ROUTE32
#define MAP_GATE_RUINS_OF_ALPH_ROUTE32      MAP_DYNAMIC
#endif
#ifndef MAP_GATE_RUINS_OF_ALPH_ROUTE36
#define MAP_GATE_RUINS_OF_ALPH_ROUTE36      MAP_DYNAMIC
#endif
#ifndef MAP_ROUTE41
#define MAP_ROUTE41                         MAP_DYNAMIC
#endif
#ifndef MAP_ROUTE42
#define MAP_ROUTE42                         MAP_DYNAMIC
#endif
#ifndef MAP_ROUTE44
#define MAP_ROUTE44                         MAP_DYNAMIC
#endif
#ifndef MAP_ROUTE45
#define MAP_ROUTE45                         MAP_DYNAMIC
#endif
#ifndef MAP_ROUTE46
#define MAP_ROUTE46                         MAP_DYNAMIC
#endif
#ifndef MAP_ROUTE47
#define MAP_ROUTE47                         MAP_DYNAMIC
#endif
// HnS-specific hub map (region selector); we handle region transit differently
#ifndef MAP_WORLD_HUB
#define MAP_WORLD_HUB                       MAP_DYNAMIC
#endif
// Johto maps not yet imported — train stations, ferry ports, and SS Aqua
#ifndef MAP_SAFFRON_CITY_TRAIN_STATION
#define MAP_SAFFRON_CITY_TRAIN_STATION      MAP_DYNAMIC
#endif
#ifndef MAP_VERMILION_CITY_PORT_INSIDE
#define MAP_VERMILION_CITY_PORT_INSIDE      MAP_DYNAMIC
#endif
#ifndef MAP_SSAQUA_1F
#define MAP_SSAQUA_1F                       MAP_DYNAMIC
#endif
#ifndef MAP_NEW_BARK_TOWN
#define MAP_NEW_BARK_TOWN                   MAP_DYNAMIC
#endif
#ifndef MAP_AZALEA_TOWN
#define MAP_AZALEA_TOWN                     MAP_DYNAMIC
#endif
// Sinnoh maps not yet imported
#ifndef MAP_TWINLEAF_TOWN_MAIN_HOUSE_2F
#define MAP_TWINLEAF_TOWN_MAIN_HOUSE_2F     MAP_DYNAMIC
#endif
// Johto city ports for ferry destinations
#ifndef MAP_OLIVINE_CITY_PORT_INSIDE
#define MAP_OLIVINE_CITY_PORT_INSIDE        MAP_DYNAMIC
#endif
// Sinnoh towns not yet imported
#ifndef MAP_SANDGEM_TOWN
#define MAP_SANDGEM_TOWN                    MAP_DYNAMIC
#endif

#endif  // GUARD_CONSTANTS_MAPS_H
