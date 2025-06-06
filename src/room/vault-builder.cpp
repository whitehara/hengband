#include "room/vault-builder.h"
#include "floor/floor-generator-util.h"
#include "floor/floor-util.h"
#include "game-option/cheat-options.h"
#include "grid/object-placer.h"
#include "monster-floor/monster-generator.h"
#include "monster-floor/place-monster-types.h"
#include "system/enums/terrain/terrain-characteristics.h"
#include "system/floor/floor-info.h"
#include "system/grid-type-definition.h"
#include "system/player-type-definition.h"
#include "view/display-messages.h"

/*
 * Grid based version of "creature_bold()"
 */
static bool player_grid(PlayerType *player_ptr, const Grid &grid)
{
    return &grid == &player_ptr->current_floor_ptr->grid_array[player_ptr->y][player_ptr->x];
}

/*
 * Grid based version of "cave_empty_bold()"
 */
static bool is_cave_empty_grid(PlayerType *player_ptr, const Grid &grid)
{
    bool is_empty_grid = grid.has(TerrainCharacteristics::PLACE);
    is_empty_grid &= !grid.has_monster();
    is_empty_grid &= !player_grid(player_ptr, grid);
    return is_empty_grid;
}

/*!
 * @brief 特殊な部屋地形向けにモンスターを配置する
 * @param player_ptr プレイヤーへの参照ポインタ
 * @param pos_center 配置したい中心座標
 * @param num 配置したいモンスターの数
 */
void vault_monsters(PlayerType *player_ptr, const Pos2D &pos_center, int num)
{
    auto &floor = *player_ptr->current_floor_ptr;
    for (auto k = 0; k < num; k++) {
        for (auto i = 0; i < 9; i++) {
            const auto d = 1;
            const auto pos = scatter(player_ptr, pos_center, d, 0);
            auto &grid = floor.get_grid(pos);
            if (!is_cave_empty_grid(player_ptr, grid)) {
                continue;
            }

            floor.monster_level = floor.base_level + 2;
            const auto has_placed = place_random_monster(player_ptr, pos.y, pos.x, PM_ALLOW_SLEEP | PM_ALLOW_GROUP);
            floor.monster_level = floor.base_level;
            if (has_placed) {
                break;
            }
        }
    }
}

/*!
 * @brief 特殊な部屋向けに各種アイテムを配置する
 * @param player_ptr プレイヤーへの参照ポインタ
 * @param pos_center 配置したい中心座標
 * @param num 配置したい数
 */
void vault_objects(PlayerType *player_ptr, const Pos2D &pos_center, int num)
{
    auto &floor = *player_ptr->current_floor_ptr;
    for (; num > 0; --num) {
        Pos2D pos = pos_center;
        int dummy = 0;
        for (int i = 0; i < 11; ++i) {
            while (dummy < SAFE_MAX_ATTEMPTS) {
                pos.y = rand_spread(pos_center.y, 2);
                pos.x = rand_spread(pos_center.x, 3);
                dummy++;
                if (!floor.contains(pos)) {
                    continue;
                }

                break;
            }

            if (dummy >= SAFE_MAX_ATTEMPTS && cheat_room) {
                msg_print(_("警告！地下室のアイテムを配置できません！", "Warning! Could not place vault object!"));
            }

            const auto &grid = floor.get_grid(pos);
            if (!grid.is_floor() || !grid.o_idx_list.empty()) {
                continue;
            }

            if (evaluate_percent(75)) {
                place_object(player_ptr, pos, 0);
            } else {
                place_gold(player_ptr, pos);
            }

            break;
        }
    }
}

/*!
 * @brief 特殊な部屋向けに各種アイテムを配置する
 * @param pos_center トラップを配置したいマスの中心座標
 * @param distribution 配置分散
 */
static void vault_trap_aux(FloorType &floor, const Pos2D &pos_center, const Pos2DVec &distribution)
{
    Pos2D pos = pos_center;
    auto dummy = 0;
    for (auto count = 0; count <= 5; count++) {
        while (dummy < SAFE_MAX_ATTEMPTS) {
            pos.y = rand_spread(pos_center.y, distribution.y);
            pos.x = rand_spread(pos_center.x, distribution.x);
            dummy++;
            if (!floor.contains(pos)) {
                continue;
            }
            break;
        }

        if (dummy >= SAFE_MAX_ATTEMPTS && cheat_room) {
            msg_print(_("警告！地下室のトラップを配置できません！", "Warning! Could not place vault trap!"));
        }

        const auto &grid = floor.get_grid(pos);
        if (!grid.is_floor() || !grid.o_idx_list.empty() || grid.has_monster()) {
            continue;
        }

        floor.place_trap_at(pos);
        break;
    }
}

/*!
 * @brief 特殊な部屋向けに各種アイテムを配置する
 * @param player_ptr プレイヤーへの参照ポインタ
 * @param pos_center トラップを配置したいマスの中心座標
 * @param distribution 配置分散
 * @param num 配置したいトラップの数
 * @todo rooms-normal からしか呼ばれていない、要調整
 */
void vault_traps(FloorType &floor, const Pos2D &pos_center, const Pos2DVec &distribution, int num)
{
    for (int i = 0; i < num; i++) {
        vault_trap_aux(floor, pos_center, distribution);
    }
}
