/*!
 * @brief 地形特性トークン辞書実装
 * @author Hourier
 * @date 2024/12/08
 */

#include "info-reader/feature-info-tokens-table.h"
#include "system/enums/terrain/terrain-characteristics.h"
#include "system/enums/terrain/terrain-tag.h"

/*!
 * @brief 地形属性トークンの定義 / Feature info flags
 */
const std::unordered_map<std::string_view, TerrainCharacteristics> f_info_flags = {
    { "LOS", TerrainCharacteristics::LOS },
    { "PROJECT", TerrainCharacteristics::PROJECT },
    { "MOVE", TerrainCharacteristics::MOVE },
    { "PLACE", TerrainCharacteristics::PLACE },
    { "DROP", TerrainCharacteristics::DROP },
    { "SECRET", TerrainCharacteristics::SECRET },
    { "NOTICE", TerrainCharacteristics::NOTICE },
    { "REMEMBER", TerrainCharacteristics::REMEMBER },
    { "OPEN", TerrainCharacteristics::OPEN },
    { "CLOSE", TerrainCharacteristics::CLOSE },
    { "BASH", TerrainCharacteristics::BASH },
    { "SPIKE", TerrainCharacteristics::SPIKE },
    { "DISARM", TerrainCharacteristics::DISARM },
    { "STORE", TerrainCharacteristics::STORE },
    { "TUNNEL", TerrainCharacteristics::TUNNEL },
    { "MAY_HAVE_GOLD", TerrainCharacteristics::MAY_HAVE_GOLD },
    { "HAS_GOLD", TerrainCharacteristics::HAS_GOLD },
    { "HAS_ITEM", TerrainCharacteristics::HAS_ITEM },
    { "DOOR", TerrainCharacteristics::DOOR },
    { "TRAP", TerrainCharacteristics::TRAP },
    { "STAIRS", TerrainCharacteristics::STAIRS },
    { "RUNE_PROTECTION", TerrainCharacteristics::RUNE_PROTECTION },
    { "LESS", TerrainCharacteristics::LESS },
    { "MORE", TerrainCharacteristics::MORE },
    { "AVOID_RUN", TerrainCharacteristics::AVOID_RUN },
    { "FLOOR", TerrainCharacteristics::FLOOR },
    { "WALL", TerrainCharacteristics::WALL },
    { "PERMANENT", TerrainCharacteristics::PERMANENT },
    // { "XXX00", FF::XXX00 },
    // { "XXX01", FF::XXX01 },
    // { "XXX02", FF::XXX02 },
    { "HIT_TRAP", TerrainCharacteristics::HIT_TRAP },

    // { "BRIDGE", FF::BRIDGE },
    // { "RIVER", FF::RIVER },
    // { "LAKE", FF::LAKE },
    // { "BRIDGED", FF::BRIDGED },
    // { "COVERED", FF::COVERED },
    { "GLOW", TerrainCharacteristics::GLOW },
    { "ENSECRET", TerrainCharacteristics::ENSECRET },
    { "WATER", TerrainCharacteristics::WATER },
    { "LAVA", TerrainCharacteristics::LAVA },
    { "SHALLOW", TerrainCharacteristics::SHALLOW },
    { "DEEP", TerrainCharacteristics::DEEP },
    { "POISON_PUDDLE", TerrainCharacteristics::POISON_PUDDLE },
    { "HURT_ROCK", TerrainCharacteristics::HURT_ROCK },
    // { "HURT_FIRE", FF::HURT_FIRE },
    // { "HURT_COLD", FF::HURT_COLD },
    // { "HURT_ACID", FF::HURT_ACID },
    { "COLD_PUDDLE", TerrainCharacteristics::COLD_PUDDLE },
    { "ACID_PUDDLE", TerrainCharacteristics::ACID_PUDDLE },
    // { "OIL", FF::OIL },
    { "ELEC_PUDDLE", TerrainCharacteristics::ELEC_PUDDLE },
    // { "CAN_CLIMB", FF::CAN_CLIMB },
    { "CAN_FLY", TerrainCharacteristics::CAN_FLY },
    { "CAN_SWIM", TerrainCharacteristics::CAN_SWIM },
    { "CAN_PASS", TerrainCharacteristics::CAN_PASS },
    // { "CAN_OOZE", FF::CAN_OOZE },
    { "CAN_DIG", TerrainCharacteristics::CAN_DIG },
    // { "HIDE_ITEM", FF::HIDE_ITEM },
    // { "HIDE_SNEAK", FF::HIDE_SNEAK },
    // { "HIDE_SWIM", FF::HIDE_SWIM },
    // { "HIDE_DIG", FF::HIDE_DIG },
    // { "KILL_HUGE", FF::KILL_HUGE },
    // { "KILL_MOVE", FF::KILL_MOVE },

    // { "PICK_TRAP", FF::PICK_TRAP },
    // { "PICK_DOOR", FF::PICK_DOOR },
    // { "ALLOC", FF::ALLOC },
    // { "CHEST", FF::CHEST },
    // { "DROP_1D2", FF::DROP_1D2 },
    // { "DROP_2D2", FF::DROP_2D2 },
    // { "DROP_GOOD", FF::DROP_GOOD },
    // { "DROP_GREAT", FF::DROP_GREAT },
    // { "HURT_POIS", FF::HURT_POIS },
    // { "HURT_ELEC", FF::HURT_ELEC },
    // { "HURT_WATER", FF::HURT_WATER },
    // { "HURT_BWATER", FF::HURT_BWATER },
    // { "USE_FEAT", FF::USE_FEAT },
    // { "GET_FEAT", FF::GET_FEAT },
    // { "GROUND", FF::GROUND },
    // { "OUTSIDE", FF::OUTSIDE },
    // { "EASY_HIDE", FF::EASY_HIDE },
    // { "EASY_CLIMB", FF::EASY_CLIMB },
    // { "MUST_CLIMB", FF::MUST_CLIMB },
    { "TREE", TerrainCharacteristics::TREE },
    // { "NEED_TREE", FF::NEED_TREE },
    // { "BLOOD", FF::BLOOD },
    // { "DUST", FF::DUST },
    // { "SLIME", FF::SLIME },
    { "PLANT", TerrainCharacteristics::PLANT },
    // { "XXX2", FF::XXX2 },
    // { "INSTANT", FF::INSTANT },
    // { "EXPLODE", FF::EXPLODE },
    // { "TIMED", FF::TIMED },
    // { "ERUPT", FF::ERUPT },
    // { "STRIKE", FF::STRIKE },
    // { "SPREAD", FF::SPREAD },

    { "SPECIAL", TerrainCharacteristics::SPECIAL },
    { "HURT_DISI", TerrainCharacteristics::HURT_DISI },
    { "QUEST_ENTER", TerrainCharacteristics::QUEST_ENTER },
    { "QUEST_EXIT", TerrainCharacteristics::QUEST_EXIT },
    { "QUEST", TerrainCharacteristics::QUEST },
    { "SHAFT", TerrainCharacteristics::SHAFT },
    { "MOUNTAIN", TerrainCharacteristics::MOUNTAIN },
    { "BLDG", TerrainCharacteristics::BLDG },
    { "RUNE_EXPLOSION", TerrainCharacteristics::RUNE_EXPLOSION },
    { "PATTERN", TerrainCharacteristics::PATTERN },
    { "TOWN", TerrainCharacteristics::TOWN },
    { "ENTRANCE", TerrainCharacteristics::ENTRANCE },
    { "MIRROR", TerrainCharacteristics::MIRROR },
    { "UNPERM", TerrainCharacteristics::UNPERM },
    { "TELEPORTABLE", TerrainCharacteristics::TELEPORTABLE },
    { "CONVERT", TerrainCharacteristics::CONVERT },
    { "GLASS", TerrainCharacteristics::GLASS },
};

const std::unordered_map<std::string_view, TerrainTag> terrain_tags = {
    { "NONE", TerrainTag::NONE },
    { "FLOOR", TerrainTag::FLOOR },
    { "RUNE_PROTECTION", TerrainTag::RUNE_PROTECTION },
    { "RUNE_EXPLOSION", TerrainTag::RUNE_EXPLOSION },
    { "MIRROR", TerrainTag::MIRROR },
    { "UP_STAIR", TerrainTag::UP_STAIR },
    { "DOWN_STAIR", TerrainTag::DOWN_STAIR },
    { "ENTRANCE", TerrainTag::ENTRANCE },
    { "RUBBLE", TerrainTag::RUBBLE },
    { "MAGMA_VEIN", TerrainTag::MAGMA_VEIN },
    { "QUARTZ_VEIN", TerrainTag::QUARTZ_VEIN },
};
