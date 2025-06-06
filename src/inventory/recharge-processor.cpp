#include "inventory/recharge-processor.h"
#include "core/disturbance.h"
#include "core/window-redrawer.h"
#include "flavor/flavor-describer.h"
#include "flavor/object-flavor-types.h"
#include "hpmp/hp-mp-regenerator.h"
#include "inventory/inventory-slot-types.h"
#include "object/tval-types.h"
#include "system/floor/floor-info.h"
#include "system/item-entity.h"
#include "system/player-type-definition.h"
#include "system/redrawing-flags-updater.h"
#include "util/string-processor.h"
#include "view/display-messages.h"

/*!
 * @brief
 * !!を刻んだ魔道具の時間経過による再充填を知らせる処理 /
 * If player has inscribed the object with "!!", let him know when it's recharged. -LM-
 * @param o_ptr 対象オブジェクトの構造体参照ポインタ
 */
static void recharged_notice(PlayerType *player_ptr, const ItemEntity &item)
{
    if (!item.is_inscribed()) {
        return;
    }

    auto s = angband_strchr(item.inscription->data(), '!');
    while (s) {
        if (s[1] == '!') {
            const auto item_name = describe_flavor(player_ptr, item, (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
            msg_format("%sは再充填された。", item_name.data());
#else
            if (item.number > 1) {
                msg_format("Your %s are recharged.", item_name.data());
            } else {
                msg_format("Your %s is recharged.", item_name.data());
            }
#endif
            disturb(player_ptr, false, false);
            return;
        }

        s = angband_strchr(s + 1, '!');
    }
}

/*!
 * @brief 10ゲームターンが進行するごとに魔道具の自然充填を行う処理
 * / Handle recharging objects once every 10 game turns
 */
void recharge_magic_items(PlayerType *player_ptr)
{
    int i;
    bool changed;

    for (changed = false, i = INVEN_MAIN_HAND; i < INVEN_TOTAL; i++) {
        auto &item = *player_ptr->inventory[i];
        if (!item.is_valid()) {
            continue;
        }

        if (item.timeout > 0) {
            item.timeout--;
            if (!item.timeout) {
                recharged_notice(player_ptr, item);
                changed = true;
            }
        }
    }

    auto &rfu = RedrawingFlagsUpdater::get_instance();
    if (changed) {
        rfu.set_flag(SubWindowRedrawingFlag::EQUIPMENT);
        wild_regen = 20;
    }

    /*
     * Recharge rods.  Rods now use timeout to control charging status,
     * and each charging rod in a stack decreases the stack's timeout by
     * one per turn. -LM-
     */
    for (changed = false, i = 0; i < INVEN_PACK; i++) {
        auto &item = *player_ptr->inventory[i];
        if (!item.is_valid()) {
            continue;
        }

        if ((item.bi_key.tval() != ItemKindType::ROD) || (item.timeout == 0)) {
            continue;
        }

        const auto base_pval = item.get_baseitem_pval();
        short temp = (item.timeout + (base_pval - 1)) / base_pval;
        if (temp > item.number) {
            temp = static_cast<short>(item.number);
        }

        item.timeout -= temp;
        if (item.timeout < 0) {
            item.timeout = 0;
        }

        if (!(item.timeout)) {
            recharged_notice(player_ptr, item);
            changed = true;
        } else if (item.timeout % base_pval) {
            changed = true;
        }
    }

    if (changed) {
        rfu.set_flag(SubWindowRedrawingFlag::INVENTORY);
        wild_regen = 20;
    }

    for (const auto &item_ptr : player_ptr->current_floor_ptr->o_list) {
        if (!item_ptr->is_valid()) {
            continue;
        }

        if ((item_ptr->bi_key.tval() == ItemKindType::ROD) && (item_ptr->timeout)) {
            item_ptr->timeout -= (TIME_EFFECT)item_ptr->number;
            if (item_ptr->timeout < 0) {
                item_ptr->timeout = 0;
            }
        }
    }
}
