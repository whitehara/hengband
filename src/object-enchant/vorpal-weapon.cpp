#include "object-enchant/vorpal-weapon.h"
#include "artifact/fixed-art-types.h"
#include "inventory/inventory-slot-types.h"
#include "io/files-util.h"
#include "main/sound-definitions-table.h"
#include "main/sound-of-music.h"
#include "monster-race/race-flags-resistance.h"
#include "player-attack/player-attack.h"
#include "system/item-entity.h"
#include "system/monrace/monrace-definition.h"
#include "system/monster-entity.h"
#include "system/player-type-definition.h"
#include "view/display-messages.h"

/*!
 * @brief ヴォーパル武器で攻撃した時のメッセージ表示
 * @param pa_ptr 直接攻撃構造体への参照ポインタ
 * @param int 倍率
 */
static void print_vorpal_message(player_attack_type *pa_ptr, const int magnification)
{
    switch (magnification) {
    case 2:
        msg_format(_("%sを斬った！", "You gouge %s!"), pa_ptr->m_name);
        sound(SoundKind::GOUGE_HIT);
        break;
    case 3:
        msg_format(_("%sをぶった斬った！", "You maim %s!"), pa_ptr->m_name);
        sound(SoundKind::MAIM_HIT);
        break;
    case 4:
        msg_format(_("%sをメッタ斬りにした！", "You carve %s!"), pa_ptr->m_name);
        sound(SoundKind::CARVE_HIT);
        break;
    case 5:
        msg_format(_("%sをメッタメタに斬った！", "You cleave %s!"), pa_ptr->m_name);
        sound(SoundKind::CLEAVE_HIT);
        break;
    case 6:
        msg_format(_("%sを刺身にした！", "You smite %s!"), pa_ptr->m_name);
        sound(SoundKind::SMITE_HIT);
        break;
    case 7:
        msg_format(_("%sを斬って斬って斬りまくった！", "You eviscerate %s!"), pa_ptr->m_name);
        sound(SoundKind::EVISCERATE_HIT);
        break;
    default:
        msg_format(_("%sを細切れにした！", "You shred %s!"), pa_ptr->m_name);
        sound(SoundKind::SHRED_HIT);
        break;
    }
}

/*!
 * @brief チェンソーのノイズ音を表示する
 * @param o_ptr チェンソーへの参照ポインタ
 */
static void print_chainsword_noise(ItemEntity *o_ptr)
{
    if (!o_ptr->is_specific_artifact(FixedArtifactId::CHAINSWORD) || one_in_(2)) {
        return;
    }

    const auto chainsword_noise = get_random_line(_("chainswd_j.txt", "chainswd.txt"), 0);
    if (chainsword_noise) {
        msg_print(*chainsword_noise);
    }
}

/*!
 * @brief ヴォーパル武器利用時の処理メインルーチン
 * @param player_ptr プレイヤーへの参照ポインタ
 * @param pa_ptr 直接攻撃構造体への参照ポインタ
 * @param vorpal_cut メッタ斬りにできるかどうか
 * @param vorpal_chance ヴォーパル倍率上昇の機会値
 */
void process_vorpal_attack(PlayerType *player_ptr, player_attack_type *pa_ptr, const bool vorpal_cut, const int vorpal_chance)
{
    if (!vorpal_cut) {
        return;
    }

    auto *o_ptr = player_ptr->inventory[enum2i(INVEN_MAIN_HAND) + pa_ptr->hand].get();
    int vorpal_magnification = 2;
    print_chainsword_noise(o_ptr);
    if (o_ptr->is_specific_artifact(FixedArtifactId::VORPAL_BLADE)) {
        msg_print(_("目にも止まらぬヴォーパルブレード、手錬の早業！", "Your Vorpal Blade goes snicker-snack!"));
    } else {
        msg_format(_("%sをグッサリ切り裂いた！", "Your weapon cuts deep into %s!"), pa_ptr->m_name);
    }

    while (one_in_(vorpal_chance)) {
        vorpal_magnification++;
    }

    pa_ptr->attack_damage *= (int)vorpal_magnification;
    const auto &monrace = pa_ptr->m_ptr->get_monrace();
    if ((monrace.resistance_flags.has(MonsterResistanceType::RESIST_ALL) ? pa_ptr->attack_damage / 100 : pa_ptr->attack_damage) > pa_ptr->m_ptr->hp) {
        msg_format(_("%sを真っ二つにした！", "You cut %s in half!"), pa_ptr->m_name);
    } else {
        print_vorpal_message(pa_ptr, vorpal_magnification);
    }

    pa_ptr->drain_result = pa_ptr->drain_result * 3 / 2;
}
