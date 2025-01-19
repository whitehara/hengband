/*!
 * @brief 地形特性タグ
 * @author Hourier
 * @date 2024/12/08
 */

#pragma once

enum class TerrainTag {
    NONE,
    FLOOR,
    RUNE_PROTECTION,
    RUNE_EXPLOSION,
    MIRROR,
    UP_STAIR,
    DOWN_STAIR,
    ENTRANCE,
    RUBBLE,
    MAGMA_VEIN,
    QUARTZ_VEIN,
    GRANITE_WALL,
    PERMANENT_WALL,
    GLASS_FLOOR,
    GLASS_WALL,
    PERMANENT_GLASS_WALL,
    BLACK_MARKET,
    TOWN,
    DEEP_WATER,
    SHALLOW_WATER,
    DEEP_LAVA,
    SHALLOW_LAVA,
    HEAVY_COLD_ZONE,
    COLD_ZONE,
    HEAVY_ELECTRICAL_ZONE,
    ELECTRICAL_ZONE,
    DEEP_ACID_PUDDLE,
    SHALLOW_ACID_PUDDLE,
    DEEP_POISONOUS_PUDDLE,
    SHALLOW_POISONOUS_PUDDLE,
    DIRT,
    GRASS,
    BRAKE, //!< 草むら.
    FLOWER,
};
