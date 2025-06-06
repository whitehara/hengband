#include "core/turn-compensator.h"
#include "floor/dungeon-feeling.h"
#include "player-info/race-types.h"
#include "store/store-owners.h"
#include "store/store-util.h"
#include "store/store.h"
#include "system/floor/floor-info.h"
#include "system/floor/town-info.h"
#include "system/floor/town-list.h"
#include "system/inner-game-data.h"
#include "system/item-entity.h"
#include "system/player-type-definition.h"
#include "world/world.h"

/*!
 * @brief ターンのオーバーフローに対する対処
 * @param player_ptr プレイヤーへの参照ポインタ
 * @details ターン及びターンを記録する変数をターンの限界の1日前まで巻き戻す.
 * @return 修正をかけた後のゲームターン
 */
void prevent_turn_overflow(PlayerType *player_ptr)
{
    const auto &igd = InnerGameData::get_instance();
    const auto game_turn_limit = igd.get_game_turn_limit();
    auto &world = AngbandWorld::get_instance();
    if (world.game_turn < game_turn_limit) {
        return;
    }

    int rollback_days = 1 + (world.game_turn - game_turn_limit) / (TURNS_PER_TICK * TOWN_DAWN);
    int32_t rollback_turns = TURNS_PER_TICK * TOWN_DAWN * rollback_days;

    if (world.game_turn > rollback_turns) {
        world.game_turn -= rollback_turns;
    } else {
        world.game_turn = 1;
    }
    auto &floor = *player_ptr->current_floor_ptr;
    if (floor.generated_turn > rollback_turns) {
        floor.generated_turn -= rollback_turns;
    } else {
        floor.generated_turn = 1;
    }
    if (world.arena_start_turn > rollback_turns) {
        world.arena_start_turn -= rollback_turns;
    } else {
        world.arena_start_turn = 1;
    }

    auto &df = DungeonFeeling::get_instance();
    if (df.get_turns() > rollback_turns) {
        df.mod_turns(-rollback_turns);
    } else {
        df.set_turns(1);
    }

    for (size_t i = 1; i < towns_info.size(); i++) {
        for (auto sst : STORE_SALE_TYPE_LIST) {
            auto &store = towns_info[i].get_store(sst);
            if (store.last_visit > -10L * TURNS_PER_TICK * STORE_TICKS) {
                store.last_visit -= rollback_turns;
                if (store.last_visit < -10L * TURNS_PER_TICK * STORE_TICKS) {
                    store.last_visit = -10L * TURNS_PER_TICK * STORE_TICKS;
                }
            }

            if (store.store_open) {
                store.store_open -= rollback_turns;
                if (store.store_open < 1) {
                    store.store_open = 1;
                }
            }
        }
    }
}
