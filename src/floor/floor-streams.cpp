/*!
 * @brief ダンジョン生成に利用する関数群 / Used by dungeon generation.
 * @date 2014/07/15
 * @author
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 * @details
 * Purpose:  This file holds all the
 * functions that are applied to a level after the rest has been
 * generated, ie streams and level destruction.
 */

#include "floor/floor-streams.h"
#include "flavor/flavor-describer.h"
#include "flavor/object-flavor-types.h"
#include "floor/floor-generator-util.h"
#include "floor/floor-object.h"
#include "game-option/birth-options.h"
#include "game-option/cheat-options.h"
#include "game-option/cheat-types.h"
#include "grid/grid.h"
#include "monster-floor/monster-remover.h"
#include "monster/monster-info.h"
#include "room/lake-types.h"
#include "spell-kind/spells-floor.h"
#include "system/artifact-type-definition.h"
#include "system/dungeon/dungeon-data-definition.h"
#include "system/dungeon/dungeon-definition.h"
#include "system/enums/terrain/terrain-tag.h"
#include "system/floor/floor-info.h"
#include "system/grid-type-definition.h"
#include "system/item-entity.h"
#include "system/monster-entity.h"
#include "system/player-type-definition.h"
#include "system/terrain/terrain-definition.h"
#include "system/terrain/terrain-list.h"
#include "view/display-messages.h"
#include "wizard/wizard-messages.h"
#include <cmath>

/*!
 * @brief 再帰フラクタルアルゴリズムによりダンジョン内に川を配置する
 * @param pos_start 起点座標
 * @param pos_end 終点座標
 * @param tag_deep 深い方の地形タグ
 * @param tag_shallow 浅い方の地形タグ
 * @param width 基本幅
 */
static void recursive_river(FloorType &floor, const Pos2D &pos_start, const Pos2D &pos_end, TerrainTag tag_deep, TerrainTag tag_shallow, int width)
{
    const auto length = Grid::calc_distance(pos_start, pos_end);
    if (length > 4) {
        const auto dy = (pos_end.y - pos_start.y) / 2;
        Pos2DVec vec_change(0, 0);
        if (dy != 0) {
            vec_change.x = randint1(std::abs(dy)) * 2 - std::abs(dy);
        }

        const auto dx = (pos_end.x - pos_start.x) / 2;
        if (dx != 0) {
            vec_change.y = randint1(std::abs(dx)) * 2 - std::abs(dx);
        }

        Pos2DVec vec_division(dy, dx);
        auto pos = pos_start + vec_division + vec_change;
        if (!floor.contains(pos)) {
            vec_change = { 0, 0 };
        }

        /* construct river out of two smaller ones */
        pos = pos_start + vec_division + vec_change;
        recursive_river(floor, pos_start, pos, tag_deep, tag_shallow, width);
        recursive_river(floor, pos, pos_end, tag_deep, tag_shallow, width);

        /* Split the river some of the time - junctions look cool */
        constexpr auto chance_river_junction = 50;
        if (one_in_(chance_river_junction) && (width > 0)) {
            vec_division *= 8;
            vec_change *= 8;
            const auto pos_junction = pos_start + vec_division + vec_change;
            recursive_river(floor, pos, pos_junction, tag_deep, tag_shallow, width - 1);
        }

        return;
    }

    /* Actually build the river */
    const auto &terrains = TerrainList::get_instance();
    for (auto l = 0; l < length; l++) {
        const auto x = pos_start.x + l * (pos_end.x - pos_start.x) / length;
        const auto y = pos_start.y + l * (pos_end.y - pos_start.y) / length;
        const Pos2D pos(y, x);
        auto done = false;
        while (!done) {
            for (auto ty = pos.y - width - 1; ty <= pos.y + width + 1; ty++) {
                for (auto tx = pos.x - width - 1; tx <= pos.x + width + 1; tx++) {
                    const Pos2D pos_target(ty, tx);
                    if (!floor.contains(pos_target, FloorBoundary::OUTER_WALL_INCLUSIVE)) {
                        continue;
                    }

                    auto &grid = floor.get_grid(pos_target);
                    if (grid.feat == terrains.get_terrain_id(tag_deep)) {
                        continue;
                    }

                    if (grid.feat == terrains.get_terrain_id(tag_shallow)) {
                        continue;
                    }

                    if (Grid::calc_distance(pos_target, pos) > rand_spread(width, 1)) {
                        continue;
                    }

                    /* Do not convert permanent features */
                    if (grid.has(TerrainCharacteristics::PERMANENT)) {
                        continue;
                    }

                    /*
                     * Clear previous contents, add feature
                     * The border mainly gets tag2, while the center gets tag1
                     */
                    if (Grid::calc_distance(pos_target, pos) > width) {
                        grid.set_terrain_id(tag_shallow);
                    } else {
                        grid.set_terrain_id(tag_deep);
                    }

                    /* Clear garbage of hidden trap or door */
                    grid.mimic = 0;

                    /* Lava terrain glows */
                    if (terrains.get_terrain(tag_deep).flags.has(TerrainCharacteristics::LAVA)) {
                        if (floor.get_dungeon_definition().flags.has_not(DungeonFeatureType::DARKNESS)) {
                            grid.info |= CAVE_GLOW;
                        }
                    }

                    /* Hack -- don't teleport here */
                    grid.info |= CAVE_ICKY;
                }
            }

            done = true;
        }
    }
}

/*!
 * @brief ランダムに川/溶岩流をダンジョンに配置する
 * @param floor フロアへの参照
 * @param dd_ptr ダンジョン生成データへの参照ポインタ
 */
void add_river(FloorType &floor, DungeonData *dd_ptr)
{
    const auto &dungeon = floor.get_dungeon_definition();
    const auto tag_pair = dungeon.decide_river_terrains(floor.dun_level);
    if (!tag_pair) {
        return;
    }

    const auto &[tag_deep, tag_shallow] = *tag_pair;
    const auto &terrains = TerrainList::get_instance();
    if (tag_deep > TerrainTag::NONE) {
        const auto &terrain = terrains.get_terrain(tag_deep);
        auto is_lava = dd_ptr->laketype == LAKE_T_LAVA;
        is_lava &= terrain.flags.has(TerrainCharacteristics::LAVA);
        auto is_water = dd_ptr->laketype == LAKE_T_WATER;
        is_water &= terrain.flags.has(TerrainCharacteristics::WATER);
        const auto should_add_river = !is_lava && !is_water && (dd_ptr->laketype != 0);
        if (should_add_river) {
            return;
        }
    }

    /* Hack -- Choose starting point */
    const auto y2 = randint1(floor.height / 2 - 2) + floor.height / 2;
    const auto x2 = randint1(floor.width / 2 - 2) + floor.width / 2;

    /* Hack -- Choose ending point somewhere on boundary */
    auto y1 = 0;
    auto x1 = 0;
    switch (randint1(4)) {
    case 1: {
        /* top boundary */
        x1 = randint1(floor.width - 2) + 1;
        y1 = 1;
        break;
    }
    case 2: {
        /* left boundary */
        x1 = 1;
        y1 = randint1(floor.height - 2) + 1;
        break;
    }
    case 3: {
        /* right boundary */
        x1 = floor.width - 1;
        y1 = randint1(floor.height - 2) + 1;
        break;
    }
    case 4: {
        /* bottom boundary */
        x1 = randint1(floor.width - 2) + 1;
        y1 = floor.height - 1;
        break;
    }
    }

    constexpr auto width_rivers = 2;
    const auto wid = randint1(width_rivers);
    recursive_river(floor, { y1, x1 }, { y2, x2 }, tag_deep, tag_shallow, wid);

    /* Hack - Save the location as a "room" */
    if (dd_ptr->cent_n < dd_ptr->centers.size()) {
        dd_ptr->centers[dd_ptr->cent_n].y = y2;
        dd_ptr->centers[dd_ptr->cent_n].x = x2;
        dd_ptr->cent_n++;
    }
}

/*!
 * @brief ダンジョンの壁部にストリーマー（地質の変化）を与える /
 * Places "streamers" of rock through dungeon
 * @param player_ptr プレイヤーへの参照ポインタ
 * @param feat ストリーマー地形ID
 * @param chance 生成密度
 * @details
 * <pre>
 * Note that their are actually six different terrain features used
 * to represent streamers.  Three each of magma and quartz, one for
 * basic vein, one with hidden gold, and one with known gold.  The
 * hidden gold types are currently unused.
 * </pre>
 */
void build_streamer(PlayerType *player_ptr, FEAT_IDX feat, int chance)
{
    const auto &streamer = TerrainList::get_instance().get_terrain(feat);
    bool streamer_is_wall = streamer.flags.has(TerrainCharacteristics::WALL) && streamer.flags.has_not(TerrainCharacteristics::PERMANENT);
    bool streamer_may_have_gold = streamer.flags.has(TerrainCharacteristics::MAY_HAVE_GOLD);

    /* Hack -- Choose starting point */
    auto &floor = *player_ptr->current_floor_ptr;
    auto y = rand_spread(floor.height / 2, floor.height / 6);
    auto x = rand_spread(floor.width / 2, floor.width / 6);

    /* Choose a random compass direction */
    Direction dir = rand_choice(Direction::directions_8());

    /* Place streamer into dungeon */
    auto dummy = 0;
    while (dummy < SAFE_MAX_ATTEMPTS) {
        dummy++;

        /* One grid per density */
        constexpr auto stream_density = 5;
        for (auto i = 0; i < stream_density; i++) {
            constexpr auto stream_width = 5;
            const auto d = stream_width;
            Pos2D pos(y, x);
            while (true) {
                pos.y = rand_spread(y, d);
                pos.x = rand_spread(x, d);
                if (!floor.contains(pos, FloorBoundary::OUTER_WALL_INCLUSIVE)) {
                    continue;
                }

                break;
            }

            auto &grid = floor.get_grid(pos);
            const auto &terrain = grid.get_terrain();
            if (terrain.flags.has(TerrainCharacteristics::MOVE) && terrain.flags.has_any_of({ TerrainCharacteristics::WATER, TerrainCharacteristics::LAVA })) {
                continue;
            }

            /* Do not convert permanent features */
            if (terrain.flags.has(TerrainCharacteristics::PERMANENT)) {
                continue;
            }

            /* Only convert "granite" walls */
            if (streamer_is_wall) {
                if (!grid.is_extra() && !grid.is_inner() && !grid.is_outer() && !grid.is_solid()) {
                    continue;
                }
                if (floor.has_closed_door_at(pos)) {
                    continue;
                }
            }

            const auto &monrace = floor.m_list[grid.m_idx].get_monrace();
            if (grid.has_monster() && !(streamer.flags.has(TerrainCharacteristics::PLACE) && monster_can_cross_terrain(player_ptr, feat, monrace, 0))) {
                /* Delete the monster (if any) */
                delete_monster(player_ptr, pos);
            }

            if (!grid.o_idx_list.empty() && streamer.flags.has_not(TerrainCharacteristics::DROP)) {

                /* Scan all objects in the grid */
                for (const auto this_o_idx : grid.o_idx_list) {
                    auto &item = *floor.o_list[this_o_idx];

                    /* Hack -- Preserve unknown artifacts */
                    if (item.is_fixed_artifact()) {
                        item.get_fixed_artifact().is_generated = false;
                        if (cheat_peek) {
                            const auto item_name = describe_flavor(player_ptr, item, (OD_NAME_ONLY | OD_STORE));
                            msg_format(_("伝説のアイテム (%s) はストリーマーにより削除された。", "Artifact (%s) was deleted by streamer."), item_name.data());
                        }
                    } else if (cheat_peek && item.is_random_artifact()) {
                        msg_print(_("ランダム・アーティファクトの1つはストリーマーにより削除された。", "One of the random artifacts was deleted by streamer."));
                    }
                }

                delete_all_items_from_floor(player_ptr, pos);
            }

            /* Clear previous contents, add proper vein type */
            grid.feat = feat;

            /* Paranoia: Clear mimic field */
            grid.mimic = 0;

            if (streamer_may_have_gold) {
                /* Hack -- Add some known treasure */
                if (one_in_(chance)) {
                    cave_alter_feat(player_ptr, pos.y, pos.x, TerrainCharacteristics::MAY_HAVE_GOLD);
                }

                /* Hack -- Add some hidden treasure */
                else if (one_in_(chance / 4)) {
                    cave_alter_feat(player_ptr, pos.y, pos.x, TerrainCharacteristics::MAY_HAVE_GOLD);
                    cave_alter_feat(player_ptr, pos.y, pos.x, TerrainCharacteristics::ENSECRET);
                }
            }
        }

        if (dummy >= SAFE_MAX_ATTEMPTS) {
            msg_print_wizard(player_ptr, CHEAT_DUNGEON, _("地形のストリーマー処理に失敗しました。", "Failed to place streamer."));
            return;
        }

        /* Advance the streamer */
        y += dir.vec().y;
        x += dir.vec().x;

        if (one_in_(10)) {
            dir = dir.rotated_45degree(one_in_(2) ? 1 : -1);
        }

        /* Quit before leaving the dungeon */
        if (!floor.contains({ y, x })) {
            break;
        }
    }
}

/*!
 * @brief ダンジョンの指定位置近辺に森林を配置する
 * @param pos 指定座標
 * @details
 * <pre>
 * Put trees near a hole in the dungeon roof  (rubble on ground + up stairway)
 * This happens in real world lava tubes.
 * </pre>
 */
void place_trees(PlayerType *player_ptr, const Pos2D &pos)
{
    /* place trees/ rubble in ovalish distribution */
    auto &floor = *player_ptr->current_floor_ptr;
    for (auto x = pos.x - 3; x < pos.x + 4; x++) {
        for (auto y = pos.y - 3; y < pos.y + 4; y++) {
            const Pos2D pos_neighbor(y, x);
            if (!floor.contains(pos_neighbor)) {
                continue;
            }

            auto &grid = floor.get_grid(pos_neighbor);
            if (any_bits(grid.info, CAVE_ICKY) || !grid.o_idx_list.empty()) {
                continue;
            }

            /* Want square to be in the circle and accessable. */
            if ((Grid::calc_distance(pos_neighbor, pos) < 4) && !grid.has(TerrainCharacteristics::PERMANENT)) {
                /*
                 * Clear previous contents, add feature
                 * The border mainly gets trees, while the center gets rubble
                 */
                if ((Grid::calc_distance(pos_neighbor, pos) > 1) || (randint1(100) < 25)) {
                    if (randint1(100) < 75) {
                        grid.set_terrain_id(TerrainTag::TREE);
                    }
                } else {
                    grid.set_terrain_id(TerrainTag::RUBBLE);
                }

                grid.mimic = 0;
                if (floor.get_dungeon_definition().flags.has_not(DungeonFeatureType::DARKNESS)) {
                    grid.info |= (CAVE_GLOW | CAVE_ROOM);
                }
            }
        }
    }

    if (!ironman_downward && one_in_(3)) {
        floor.get_grid(pos).set_terrain_id(TerrainTag::UP_STAIR);
    }
}

/*!
 * @brief ダンジョンに＊破壊＊済み地形ランダムに施す /
 * Build a destroyed level
 */
void destroy_level(PlayerType *player_ptr)
{
    msg_print_wizard(player_ptr, CHEAT_DUNGEON, _("階に*破壊*の痕跡を生成しました。", "Destroyed Level."));

    /* Drop a few epi-centers (usually about two) */
    POSITION y1, x1;
    const auto &floor = *player_ptr->current_floor_ptr;
    for (int n = 0; n < randint1(5); n++) {
        /* Pick an epi-center */
        x1 = rand_range(5, floor.width - 1 - 5);
        y1 = rand_range(5, floor.height - 1 - 5);

        (void)destroy_area(player_ptr, y1, x1, 15, true);
    }
}
