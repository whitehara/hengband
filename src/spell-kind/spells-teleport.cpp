/*!
 * @brief テレポート魔法全般
 * @date 2020/06/04
 * @author Hourier
 */

#include "spell-kind/spells-teleport.h"
#include "avatar/avatar.h"
#include "core/asking-player.h"
#include "core/speed-table.h"
#include "dungeon/quest.h"
#include "effect/effect-characteristics.h"
#include "floor/line-of-sight.h"
#include "grid/grid.h"
#include "inventory/inventory-slot-types.h"
#include "main/sound-definitions-table.h"
#include "main/sound-of-music.h"
#include "monster-race/race-brightness-mask.h"
#include "monster/monster-info.h"
#include "monster/monster-status-setter.h"
#include "monster/monster-update.h"
#include "monster/monster-util.h"
#include "player-base/player-class.h"
#include "player/player-move.h"
#include "player/player-status.h"
#include "spell-kind/spells-launcher.h"
#include "system/floor/floor-info.h"
#include "system/grid-type-definition.h"
#include "system/item-entity.h"
#include "system/monrace/monrace-definition.h"
#include "system/monster-entity.h"
#include "system/player-type-definition.h"
#include "system/redrawing-flags-updater.h"
#include "target/grid-selector.h"
#include "target/target-checker.h"
#include "util/bit-flags-calculator.h"
#include "view/display-messages.h"
#include "world/world.h"
#include <array>

/*!
 * @brief モンスターとの位置交換処理 / Switch position with a monster.
 * @param player_ptr プレイヤーへの参照ポインタ
 * @param dir 方向(5ならばグローバル変数 target_col/target_row の座標を目標にする)
 * @return 作用が実際にあった場合TRUEを返す
 */
bool teleport_swap(PlayerType *player_ptr, const Direction &dir)
{
    const auto pos = dir.get_target_position(player_ptr->get_position());
    if (player_ptr->anti_tele) {
        msg_print(_("不思議な力がテレポートを防いだ！", "A mysterious force prevents you from teleporting!"));
        return false;
    }

    const auto &grid = player_ptr->current_floor_ptr->get_grid(pos);
    if (!grid.has_monster() || player_ptr->current_floor_ptr->m_list[grid.m_idx].is_riding()) {
        msg_print(_("それとは場所を交換できません。", "You can't trade places with that!"));
        return false;
    }

    if ((grid.is_icky()) || (Grid::calc_distance(pos, player_ptr->get_position()) > player_ptr->lev * 3 / 2 + 10)) {
        msg_print(_("失敗した。", "Failed to swap."));
        return false;
    }

    const auto &monster = player_ptr->current_floor_ptr->m_list[grid.m_idx];
    auto &monrace = monster.get_monrace();

    (void)set_monster_csleep(player_ptr, grid.m_idx, 0);

    if (monrace.resistance_flags.has(MonsterResistanceType::RESIST_TELEPORT)) {
        msg_print(_("テレポートを邪魔された！", "Your teleportation is blocked!"));
        if (is_original_ap_and_seen(player_ptr, monster)) {
            monrace.r_resistance_flags.set(MonsterResistanceType::RESIST_TELEPORT);
        }
        return false;
    }

    sound(SoundKind::TELEPORT);
    (void)move_player_effect(player_ptr, pos.y, pos.x, MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP);
    return true;
}

/*!
 * @brief モンスター用テレポート処理
 * @param player_ptr プレイヤーへの参照ポインタ
 * @param dir 方向(5ならばグローバル変数 target_col/target_row の座標を目標にする)
 * @param distance 移動距離
 * @return 作用が実際にあった場合TRUEを返す
 */
bool teleport_monster(PlayerType *player_ptr, const Direction &dir, int distance)
{
    BIT_FLAGS flg = PROJECT_BEAM | PROJECT_KILL;
    return project_hook(player_ptr, AttributeType::AWAY_ALL, dir, distance, flg);
}

/*!
 * @brief モンスターのテレポートアウェイ処理 /
 * Teleport a monster, normally up to "dis" grids away.
 * @param player_ptr プレイヤーへの参照ポインタ
 * @param m_idx モンスターID
 * @param dis テレポート距離
 * @param mode オプション
 * @return テレポートが実際に行われたらtrue
 * @details
 * Attempt to move the monster at least "dis/2" grids away.
 * But allow variation to prevent infinite loops.
 */
bool teleport_away(PlayerType *player_ptr, MONSTER_IDX m_idx, POSITION dis, teleport_flags mode)
{
    auto &floor = *player_ptr->current_floor_ptr;
    auto &monster = floor.m_list[m_idx];
    if (!monster.is_valid()) {
        return false;
    }

    if ((mode & TELEPORT_DEC_VALOUR) && (((player_ptr->chp * 10) / player_ptr->mhp) > 5) && (4 + randint1(5) < ((player_ptr->chp * 10) / player_ptr->mhp))) {
        chg_virtue(player_ptr, Virtue::VALOUR, -1);
    }

    const auto m_pos_orig = monster.get_position();
    auto min = dis / 2;
    auto tries = 0;
    Pos2D m_pos(0, 0);
    auto look = true;
    while (look) {
        tries++;
        if (dis > 200) {
            dis = 200;
        }

        for (auto i = 0; i < 500; i++) {
            while (true) {
                m_pos.y = rand_spread(m_pos_orig.y, dis);
                m_pos.x = rand_spread(m_pos_orig.x, dis);
                const auto d = Grid::calc_distance(m_pos_orig, m_pos);
                if ((d >= min) && (d <= dis)) {
                    break;
                }
            }

            if (!floor.contains(m_pos)) {
                continue;
            }
            if (!cave_monster_teleportable_bold(player_ptr, m_idx, m_pos.y, m_pos.x, mode)) {
                continue;
            }
            if (!floor.is_in_quest() && !floor.inside_arena) {
                if (floor.get_grid(m_pos).is_icky()) {
                    continue;
                }
            }

            look = false;
            break;
        }

        dis = dis * 2;
        min = min / 2;
        const int MAX_TELEPORT_TRIES = 100;
        if (tries > MAX_TELEPORT_TRIES) {
            return false;
        }
    }

    sound(SoundKind::TPOTHER);
    floor.get_grid(m_pos_orig).m_idx = 0;
    floor.get_grid(m_pos).m_idx = m_idx;
    monster.set_position(m_pos);
    monster.reset_target();
    update_monster(player_ptr, m_idx, true);
    lite_spot(player_ptr, m_pos_orig);
    lite_spot(player_ptr, m_pos);

    if (monster.get_monrace().brightness_flags.has_any_of(ld_mask)) {
        RedrawingFlagsUpdater::get_instance().set_flag(StatusRecalculatingFlag::MONSTER_LITE);
    }

    return true;
}

/*!
 * @brief モンスターを指定された座標付近にテレポートする /
 * Teleport monster next to a grid near the given location
 * @param player_ptr プレイヤーへの参照ポインタ
 * @param m_idx モンスターID
 * @param ty 目安Y座標
 * @param tx 目安X座標
 * @param power テレポート成功確率
 * @param mode オプション
 */
void teleport_monster_to(PlayerType *player_ptr, MONSTER_IDX m_idx, POSITION ty, POSITION tx, int power, teleport_flags mode)
{
    auto &floor = *player_ptr->current_floor_ptr;
    auto &monster = floor.m_list[m_idx];
    if (!monster.is_valid()) {
        return;
    }
    if (randint1(100) > power) {
        return;
    }

    const auto m_pos_orig = monster.get_position();
    Pos2D m_pos = m_pos_orig;
    auto dis = 2;
    auto min = dis / 2;
    auto attempts = 500;
    auto look = true;
    while (look && --attempts) {
        if (dis > 200) {
            dis = 200;
        }

        for (int i = 0; i < 500; i++) {
            while (true) {
                m_pos.y = rand_spread(ty, dis);
                m_pos.x = rand_spread(tx, dis);
                const auto d = Grid::calc_distance({ ty, tx }, m_pos);
                if ((d >= min) && (d <= dis)) {
                    break;
                }
            }

            if (!floor.contains(m_pos)) {
                continue;
            }
            if (!cave_monster_teleportable_bold(player_ptr, m_idx, m_pos.y, m_pos.x, mode)) {
                continue;
            }

            look = false;
            break;
        }

        dis = dis * 2;
        min = min / 2;
    }

    if (attempts < 1) {
        return;
    }

    sound(SoundKind::TPOTHER);
    floor.get_grid(m_pos_orig).m_idx = 0;
    floor.get_grid(m_pos).m_idx = m_idx;
    monster.set_position(m_pos);
    update_monster(player_ptr, m_idx, true);
    lite_spot(player_ptr, m_pos_orig);
    lite_spot(player_ptr, m_pos);

    if (monster.get_monrace().brightness_flags.has_any_of(ld_mask)) {
        RedrawingFlagsUpdater::get_instance().set_flag(StatusRecalculatingFlag::MONSTER_LITE);
    }
}

/*!
 * @brief プレイヤーのテレポート先選定と移動処理 /
 * Teleport the player to a location up to "dis" grids away.
 * @param player_ptr プレイヤーへの参照ポインタ
 * @param dis 基本移動距離
 * @param is_quantum_effect 量子的効果 (反テレポ無効)によるテレポートアウェイならばTRUE
 * @param mode オプション
 * @return 実際にテレポート処理が行われたらtrue
 * @details
 * <pre>
 * If no such spaces are readily available, the distance may increase.
 * Try very hard to move the player at least a quarter that distance.
 *
 * There was a nasty tendency for a long time; which was causing the
 * player to "bounce" between two or three different spots because
 * these are the only spots that are "far enough" way to satisfy the
 * algorithm.
 *
 * But this tendency is now removed; in the new algorithm, a list of
 * candidates is selected first, which includes at least 50% of all
 * floor grids within the distance, and any single grid in this list
 * of candidates has equal possibility to be choosen as a destination.
 * </pre>
 */
bool teleport_player_aux(PlayerType *player_ptr, POSITION dis, bool is_quantum_effect, teleport_flags mode)
{
    if (AngbandWorld::get_instance().is_wild_mode()) {
        return false;
    }

    if (!is_quantum_effect && player_ptr->anti_tele && !(mode & TELEPORT_NONMAGICAL)) {
        msg_print(_("不思議な力がテレポートを防いだ！", "A mysterious force prevents you from teleporting!"));
        return false;
    }

    constexpr auto max_distance = 200;
    std::array<int, max_distance + 1> candidates_at{};
    if (dis > max_distance) {
        dis = max_distance;
    }

    const auto &floor_ref = *player_ptr->current_floor_ptr;
    const auto left = std::max(1, player_ptr->x - dis);
    const auto right = std::min(floor_ref.width - 2, player_ptr->x + dis);
    const auto top = std::max(1, player_ptr->y - dis);
    const auto bottom = std::min(floor_ref.height - 2, player_ptr->y + dis);
    auto total_candidates = 0;
    for (auto y = top; y <= bottom; y++) {
        for (auto x = left; x <= right; x++) {
            if (!cave_player_teleportable_bold(player_ptr, y, x, mode)) {
                continue;
            }

            const auto d = Grid::calc_distance(player_ptr->get_position(), { y, x });
            if (d > dis) {
                continue;
            }

            total_candidates++;
            candidates_at[d]++;
        }
    }

    if (0 == total_candidates) {
        return false;
    }

    int cur_candidates;
    auto min = dis;
    for (cur_candidates = 0; min >= 0; min--) {
        cur_candidates += candidates_at[min];
        if (cur_candidates && (cur_candidates >= total_candidates / 2)) {
            break;
        }
    }

    auto pick = randint1(cur_candidates);

    /* Search again the choosen location */
    auto y = top;
    auto x = 0;
    for (; y <= bottom; y++) {
        for (x = left; x <= right; x++) {
            if (!cave_player_teleportable_bold(player_ptr, y, x, mode)) {
                continue;
            }

            const auto d = Grid::calc_distance(player_ptr->get_position(), { y, x });
            if (d > dis) {
                continue;
            }
            if (d < min) {
                continue;
            }

            pick--;
            if (!pick) {
                break;
            }
        }

        if (!pick) {
            break;
        }
    }

    const Pos2D pos(y, x);
    if (player_ptr->is_located_at(pos)) {
        return false;
    }

    sound(SoundKind::TELEPORT);
#ifdef JP
    if (is_echizen(player_ptr)) {
        msg_format("『こっちだぁ、%s』", player_ptr->name);
    }
#endif
    (void)move_player_effect(player_ptr, pos.y, pos.x, MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP);
    return true;
}

/*!
 * @brief プレイヤーのテレポート処理メインルーチン
 * @param player_ptr プレイヤーへの参照ポインタ
 * @param dis 基本移動距離
 * @param mode オプション
 */
void teleport_player(PlayerType *player_ptr, POSITION dis, BIT_FLAGS mode)
{
    const POSITION oy = player_ptr->y;
    const POSITION ox = player_ptr->x;

    if (!teleport_player_aux(player_ptr, dis, false, i2enum<teleport_flags>(mode))) {
        return;
    }

    /* Monsters with teleport ability may follow the player */
    for (POSITION xx = -1; xx < 2; xx++) {
        for (POSITION yy = -1; yy < 2; yy++) {
            MONSTER_IDX tmp_m_idx = player_ptr->current_floor_ptr->grid_array[oy + yy][ox + xx].m_idx;
            if (!is_monster(tmp_m_idx)) {
                continue;
            }
            const auto &monster = player_ptr->current_floor_ptr->m_list[tmp_m_idx];
            if (!monster.is_riding()) {
                const auto &monrace = monster.get_monrace();

                bool can_follow = monrace.ability_flags.has(MonsterAbilityType::TPORT);
                can_follow &= monrace.resistance_flags.has_not(MonsterResistanceType::RESIST_TELEPORT);
                can_follow &= !monster.is_asleep();
                if (can_follow) {
                    teleport_monster_to(player_ptr, tmp_m_idx, player_ptr->y, player_ptr->x, monrace.level, TELEPORT_SPONTANEOUS);
                }
            }
        }
    }
}

/*!
 * @brief プレイヤーのテレポートアウェイ処理 /
 * @param m_idx アウェイを試みたモンスターID
 * @param player_ptr プレイヤーへの参照ポインタ
 * @param dis テレポート距離
 * @param is_quantum_effect 量子的効果によるテレポートアウェイならばTRUE
 */
void teleport_player_away(MONSTER_IDX m_idx, PlayerType *player_ptr, POSITION dis, bool is_quantum_effect)
{
    if (AngbandSystem::get_instance().is_phase_out()) {
        return;
    }

    const POSITION oy = player_ptr->y;
    const POSITION ox = player_ptr->x;

    if (!teleport_player_aux(player_ptr, dis, is_quantum_effect, TELEPORT_PASSIVE)) {
        return;
    }
    const auto &floor = *player_ptr->current_floor_ptr;

    /* Monsters with teleport ability may follow the player */
    for (POSITION xx = -1; xx < 2; xx++) {
        for (POSITION yy = -1; yy < 2; yy++) {
            const auto tmp_m_idx = floor.grid_array[oy + yy][ox + xx].m_idx;
            auto is_teleportable = is_monster(tmp_m_idx) && !floor.m_list[tmp_m_idx].is_riding();
            is_teleportable &= m_idx != tmp_m_idx;
            if (!is_teleportable) {
                continue;
            }

            const auto &monster = player_ptr->current_floor_ptr->m_list[tmp_m_idx];
            const auto &monrace = monster.get_monrace();

            bool can_follow = monrace.ability_flags.has(MonsterAbilityType::TPORT);
            can_follow &= monrace.resistance_flags.has_not(MonsterResistanceType::RESIST_TELEPORT);
            can_follow &= !monster.is_asleep();
            if (can_follow) {
                teleport_monster_to(player_ptr, tmp_m_idx, player_ptr->y, player_ptr->x, monrace.level, TELEPORT_SPONTANEOUS);
            }
        }
    }
}

/*!
 * @brief プレイヤーを指定位置近辺にテレポートさせる
 * @param player_ptr プレイヤーへの参照ポインタ
 * @param ny 目標Y座標
 * @param nx 目標X座標
 * @param mode オプションフラグ
 * @details
 * <pre>
 * This function is slightly obsessive about correctness.
 * This function allows teleporting into vaults (!)
 * </pre>
 */
void teleport_player_to(PlayerType *player_ptr, POSITION ny, POSITION nx, teleport_flags mode)
{
    if (player_ptr->anti_tele && !(mode & TELEPORT_NONMAGICAL)) {
        msg_print(_("不思議な力がテレポートを防いだ！", "A mysterious force prevents you from teleporting!"));
        return;
    }

    const auto &floor = *player_ptr->current_floor_ptr;
    Pos2D pos(0, 0);
    auto dis = 0;
    auto ctr = 0;
    const auto &world = AngbandWorld::get_instance();
    while (true) {
        while (true) {
            pos.y = rand_spread(ny, dis);
            pos.x = rand_spread(nx, dis);
            if (floor.contains(pos)) {
                break;
            }
        }

        auto is_anywhere = world.wizard;
        is_anywhere &= (mode & TELEPORT_PASSIVE) == 0;
        const auto &grid = floor.get_grid(pos);
        is_anywhere &= grid.has_monster() || grid.m_idx == player_ptr->riding;
        if (is_anywhere) {
            break;
        }

        if (cave_player_teleportable_bold(player_ptr, pos.y, pos.x, mode)) {
            break;
        }

        if (++ctr > (4 * dis * dis + 4 * dis + 1)) {
            ctr = 0;
            dis++;
        }
    }

    sound(SoundKind::TELEPORT);
    (void)move_player_effect(player_ptr, pos.y, pos.x, MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP);
}

void teleport_away_followable(PlayerType *player_ptr, MONSTER_IDX m_idx)
{
    const auto &floor = *player_ptr->current_floor_ptr;
    const auto &monster = floor.m_list[m_idx];
    const auto old_m_pos = monster.get_position();
    bool old_ml = monster.ml;
    POSITION old_cdis = monster.cdis;

    teleport_away(player_ptr, m_idx, MAX_PLAYER_SIGHT * 2 + 5, TELEPORT_SPONTANEOUS);

    bool is_followable = old_ml;
    is_followable &= old_cdis <= MAX_PLAYER_SIGHT;
    is_followable &= AngbandWorld::get_instance().timewalk_m_idx == 0;
    is_followable &= !AngbandSystem::get_instance().is_phase_out();
    is_followable &= los(floor, player_ptr->get_position(), old_m_pos);
    if (!is_followable) {
        return;
    }

    bool follow = false;
    if (player_ptr->muta.has(PlayerMutationType::VTELEPORT) || PlayerClass(player_ptr).equals(PlayerClassType::IMITATOR)) {
        follow = true;
    } else {
        ItemEntity *o_ptr;
        INVENTORY_IDX i;

        for (i = INVEN_MAIN_HAND; i < INVEN_TOTAL; i++) {
            o_ptr = player_ptr->inventory[i].get();
            if (o_ptr->is_valid() && !o_ptr->is_cursed() && o_ptr->get_flags().has(TR_TELEPORT)) {
                follow = true;
                break;
            }
        }
    }

    if (!follow) {
        return;
    }
    if (!input_check_strict(player_ptr, _("ついていきますか？", "Do you follow it? "), UserCheck::OKAY_CANCEL)) {
        return;
    }

    if (one_in_(3)) {
        teleport_player(player_ptr, 200, TELEPORT_PASSIVE);
        msg_print(_("失敗！", "Failed!"));
    } else {
        teleport_player_to(player_ptr, monster.fy, monster.fx, TELEPORT_SPONTANEOUS);
    }

    player_ptr->energy_need += ENERGY_NEED();
}

/*!
 * @brief 次元の扉 (Dimension door)処理
 * @param player_ptr プレイヤーへの参照ポインタ
 * @param pos テレポート先座標
 * @return 目標に指定通りテレポートできたか否か
 */
bool exe_dimension_door(PlayerType *player_ptr, const Pos2D &pos)
{
    PLAYER_LEVEL plev = player_ptr->lev;

    player_ptr->energy_need += static_cast<short>((60 - plev) * ENERGY_NEED() / 100);
    auto is_successful = cave_player_teleportable_bold(player_ptr, pos.y, pos.x, TELEPORT_SPONTANEOUS);
    is_successful &= Grid::calc_distance(pos, player_ptr->get_position()) <= plev / 2 + 10;
    is_successful &= !one_in_(plev / 10 + 10);
    if (!is_successful) {
        player_ptr->energy_need += static_cast<short>((60 - plev) * ENERGY_NEED() / 100);
        teleport_player(player_ptr, (plev + 2) * 2, TELEPORT_PASSIVE);
        return false;
    }

    teleport_player_to(player_ptr, pos.y, pos.x, TELEPORT_SPONTANEOUS);
    return true;
}

/*!
 * @brief 次元の扉処理のメインルーチン /
 * @param player_ptr プレイヤーへの参照ポインタ
 * Dimension Door
 * @return ターンを消費した場合TRUEを返す
 */
bool dimension_door(PlayerType *player_ptr)
{
    const auto pos = point_target(player_ptr);
    if (!pos) {
        return false;
    }

    if (exe_dimension_door(player_ptr, *pos)) {
        return true;
    }

    msg_print(_("精霊界から物質界に戻る時うまくいかなかった！", "You fail to exit the astral plane correctly!"));
    return true;
}
