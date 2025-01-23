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
    { "UNDETECTED", TerrainTag::UNDETECTED },
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
    { "GRANITE", TerrainTag::GRANITE_WALL },
    { "PERMANENT", TerrainTag::PERMANENT_WALL },
    { "GLASS_FLOOR", TerrainTag::GLASS_FLOOR },
    { "GLASS_WALL", TerrainTag::GLASS_WALL },
    { "PERMANENT_GLASS_WALL", TerrainTag::PERMANENT_GLASS_WALL },
    { "BLACK_MARKET", TerrainTag::BLACK_MARKET },
    { "TOWN", TerrainTag::TOWN },
    { "DEEP_WATER", TerrainTag::DEEP_WATER },
    { "SHALLOW_WATER", TerrainTag::SHALLOW_WATER },
    { "DEEP_LAVA", TerrainTag::DEEP_LAVA },
    { "SHALLOW_LAVA", TerrainTag::SHALLOW_LAVA },
    { "HEAVY_COLD_ZONE", TerrainTag::HEAVY_COLD_ZONE },
    { "COLD_ZONE", TerrainTag::COLD_ZONE },
    { "HEAVY_ELECTRICAL_ZONE", TerrainTag::HEAVY_ELECTRICAL_ZONE },
    { "ELECTRICAL_ZONE", TerrainTag::ELECTRICAL_ZONE },
    { "DEEP_ACID_PUDDLE", TerrainTag::DEEP_ACID_PUDDLE },
    { "SHALLOW_ACID_PUDDLE", TerrainTag::SHALLOW_ACID_PUDDLE },
    { "DEEP_POISONOUS_PUDDLE", TerrainTag::DEEP_POISONOUS_PUDDLE },
    { "SHALLOW_POISONOUS_PUDDLE", TerrainTag::SHALLOW_POISONOUS_PUDDLE },
    { "DIRT", TerrainTag::DIRT },
    { "GRASS", TerrainTag::GRASS },
    { "BRAKE", TerrainTag::BRAKE },
    { "FLOWER", TerrainTag::FLOWER },
    { "TREE", TerrainTag::TREE },
    { "MOUNTAIN", TerrainTag::MOUNTAIN },
    { "SWAMP", TerrainTag::SWAMP },

    { "TRAP_TRAPDOOR", TerrainTag::TRAP_TRAPDOOR },
    { "TRAP_PIT", TerrainTag::TRAP_PIT },
    { "TRAP_SPIKED_PIT", TerrainTag::TRAP_SPIKED_PIT },
    { "TRAP_POISON_PIT", TerrainTag::TRAP_POISON_PIT },
    { "TRAP_TY_CURSE", TerrainTag::TRAP_TY_CURSE },
    { "TRAP_TELEPORT", TerrainTag::TRAP_TELEPORT },
    { "TRAP_FIRE", TerrainTag::TRAP_FIRE },
    { "TRAP_ACID", TerrainTag::TRAP_ACID },
    { "TRAP_SLOW", TerrainTag::TRAP_SLOW },
    { "TRAP_LOSE_STR", TerrainTag::TRAP_LOSE_STR },
    { "TRAP_LOSE_DEX", TerrainTag::TRAP_LOSE_DEX },
    { "TRAP_LOSE_CON", TerrainTag::TRAP_LOSE_CON },
    { "TRAP_BLIND", TerrainTag::TRAP_BLIND },
    { "TRAP_CONFUSE", TerrainTag::TRAP_CONFUSE },
    { "TRAP_POISON", TerrainTag::TRAP_POISON },
    { "TRAP_SLEEP", TerrainTag::TRAP_SLEEP },
    { "TRAP_TRAPS", TerrainTag::TRAP_TRAPS },
    { "TRAP_ALARM", TerrainTag::TRAP_ALARM },

    { "TRAP_OPEN", TerrainTag::TRAP_OPEN },
    { "TRAP_PIRANHA", TerrainTag::TRAP_PIRANHA },
    { "TRAP_ARMAGEDDON", TerrainTag::TRAP_ARMAGEDDON },

    { "PATTERN_START", TerrainTag::PATTERN_START },
    { "PATTERN_1", TerrainTag::PATTERN_1 },
    { "PATTERN_2", TerrainTag::PATTERN_2 },
    { "PATTERN_3", TerrainTag::PATTERN_3 },
    { "PATTERN_4", TerrainTag::PATTERN_4 },
    { "PATTERN_END", TerrainTag::PATTERN_END },
    { "PATTERN_OLD", TerrainTag::PATTERN_OLD },
    { "PATTERN_EXIT", TerrainTag::PATTERN_EXIT },
    { "PATTERN_CORRUPTED", TerrainTag::PATTERN_CORRUPTED },
};
