#pragma once

#include "dungeon/dungeon-flag-types.h"
#include "monster-race/race-ability-flags.h"
#include "monster-race/race-behavior-flags.h"
#include "monster-race/race-brightness-flags.h"
#include "monster-race/race-drop-flags.h"
#include "monster-race/race-feature-flags.h"
#include "monster-race/race-flags-resistance.h"
#include "monster-race/race-kind-flags.h"
#include "monster-race/race-misc-flags.h"
#include "monster-race/race-population-flags.h"
#include "monster-race/race-resistance-mask.h"
#include "monster-race/race-speak-flags.h"
#include "monster-race/race-special-flags.h"
#include "monster-race/race-visual-flags.h"
#include "monster-race/race-wilderness-flags.h"
#include "system/angband.h"
#include "util/flag-group.h"
#include <array>
#include <optional>
#include <string>
#include <utility>
#include <vector>

constexpr auto TERRAIN_PROBABILITY_NUM = 3;

enum class DungeonMode {
    AND = 1,
    NAND = 2,
    OR = 3,
    NOR = 4,
};

class TerrainProbabilityEntry {
public:
    short terrain_id{}; //!< 地形ID.
    int chance{}; //!< 確率(%).
};

/* A structure for the != dungeon types */
enum class DoorKind;
enum class FixedArtifactId : short;
enum class MonraceId : short;
enum class MonsterSex;
enum class TerrainCharacteristics;
enum class TerrainTag;
class MonraceDefinition;
class DungeonDefinition {
public:
    std::string name; /* Name */
    std::string text; /* Description */

    POSITION dy{};
    POSITION dx{};

    std::array<TerrainProbabilityEntry, TERRAIN_PROBABILITY_NUM> floor{}; /* Floor probability */
    std::array<TerrainProbabilityEntry, TERRAIN_PROBABILITY_NUM> fill{}; /* Cave wall probability */
    short outer_wall{}; /* 外壁の地形ID */
    short inner_wall{}; /* 内壁の地形ID */
    FEAT_IDX stream1{}; /* stream tile */
    FEAT_IDX stream2{}; /* stream tile */

    DEPTH mindepth{}; /* Minimal depth */
    DEPTH maxdepth{}; /* Maximal depth */
    PLAYER_LEVEL min_plev{}; /* Minimal plev needed to enter -- it's an anti-cheating mesure */
    BIT_FLAGS16 pit{};
    BIT_FLAGS16 nest{};
    DungeonMode mode{}; /* Mode of combinaison of the monster flags */

    int min_m_alloc_level{}; /* Minimal number of monsters per level */
    int max_m_alloc_chance{}; /* There is a 1/max_m_alloc_chance chance per round of creating a new monster */

    EnumClassFlagGroup<DungeonFeatureType> flags{}; /* Dungeon Flags */

    EnumClassFlagGroup<MonsterAbilityType> mon_ability_flags;
    EnumClassFlagGroup<MonsterBehaviorType> mon_behavior_flags;
    EnumClassFlagGroup<MonsterVisualType> mon_visual_flags;
    EnumClassFlagGroup<MonsterKindType> mon_kind_flags;
    EnumClassFlagGroup<MonsterResistanceType> mon_resistance_flags;
    EnumClassFlagGroup<MonsterDropType> mon_drop_flags;
    EnumClassFlagGroup<MonsterWildernessType> mon_wilderness_flags;
    EnumClassFlagGroup<MonsterFeatureType> mon_feature_flags;
    EnumClassFlagGroup<MonsterPopulationType> mon_population_flags;
    EnumClassFlagGroup<MonsterSpeakType> mon_speak_flags;
    EnumClassFlagGroup<MonsterBrightnessType> mon_brightness_flags;
    EnumClassFlagGroup<MonsterSpecialType> mon_special_flags;
    EnumClassFlagGroup<MonsterMiscType> mon_misc_flags;
    MonsterSex mon_sex{};

    std::vector<char> r_chars; /* Monster symbols allowed */
    short final_object{}; /* The object you'll find at the bottom */
    FixedArtifactId final_artifact{}; /* The artifact you'll find at the bottom */
    MonraceId final_guardian{}; /* The artifact's guardian. If an artifact is specified, then it's NEEDED */

    PROB special_div{}; /* % of monsters affected by the flags/races allowed, to add some variety */
    int tunnel_percent{};
    int obj_great{};
    int obj_good{};

    bool has_river_flag() const;
    bool has_guardian() const;
    const MonraceDefinition &get_guardian() const;
    short convert_terrain_id(short terrain_id, TerrainCharacteristics action) const;
    short convert_terrain_id(short terrain_id) const;
    bool is_open(short terrain_id) const;
    bool is_conquered() const;
    std::string build_entrance_message() const;
    std::string describe_depth() const;
    int calc_cavern_terrains() const;
    std::optional<std::pair<TerrainTag, TerrainTag>> decide_river_terrains(int threshold) const;
    DoorKind select_door_kind() const;
    short select_floor_terrain_id() const;
    short select_wall_terrain_id() const;

    //!< @details ここから下は、地形など全ての定義ファイルを読み込んだ後に呼び出される初期化処理.
    void set_guardian_flag();
    void set_floor_terrain_ids();
    void set_wall_terrain_ids();

private:
    std::array<short, 100> floor_terrain_ids{};
    std::array<short, 100> wall_terrain_ids{};

    static std::array<short, 100> make_terrain_ids(const std::array<TerrainProbabilityEntry, TERRAIN_PROBABILITY_NUM> &prob_table);
    MonraceDefinition &get_guardian();
};
