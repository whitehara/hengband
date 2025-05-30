/*!
 * @brief effect_monster_type構造体の初期化処理
 * @date 2020/04/29
 * @author Hourier
 */

#include "effect/effect-monster-util.h"
#include "effect/attribute-types.h"
#include "floor/geometry.h"
#include "monster-floor/monster-death.h"
#include "monster/monster-info.h"
#include "monster/monster-status.h"
#include "monster/monster-util.h"
#include "system/angband-system.h"
#include "system/floor/floor-info.h"
#include "system/grid-type-definition.h"
#include "system/monrace/monrace-definition.h"
#include "system/monster-entity.h"
#include "system/player-type-definition.h"

/*!
 * @brief EffectMonster構造体のコンストラクタ
 * @param player_ptr プレイヤーへの参照ポインタ
 * @param src_idx 魔法を発動したモンスター (0ならばプレイヤー)
 * @param r 効果半径(ビーム/ボルト = 0 / ボール = 1以上) / Radius of explosion (0 = beam/bolt, 1 to 9 = ball)
 * @param y 目標y座標 / Target y location (or location to travel "towards")
 * @param x 目標x座標 / Target x location (or location to travel "towards")
 * @param dam 基本威力 / Base damage roll to apply to affected monsters (or player)
 * @param attribute 効果属性 / Type of damage to apply to monsters (and objects)
 * @param flag 効果フラグ
 * @param see_s_msg TRUEならばメッセージを表示する
 */
EffectMonster::EffectMonster(PlayerType *player_ptr, MONSTER_IDX src_idx, POSITION r, POSITION y, POSITION x, int dam, AttributeType attribute, BIT_FLAGS flag, bool see_s_msg)
    : src_idx(src_idx)
    , r(r)
    , y(y)
    , x(x)
    , dam(dam)
    , attribute(attribute)
    , flag(flag)
    , see_s_msg(see_s_msg)
{
    auto &floor = *player_ptr->current_floor_ptr;
    this->g_ptr = &floor.grid_array[this->y][this->x];
    this->m_ptr = &floor.m_list[this->g_ptr->m_idx];
    this->m_caster_ptr = this->is_monster() ? &floor.m_list[this->src_idx] : nullptr;
    this->r_ptr = &this->m_ptr->get_monrace();
    this->seen = this->m_ptr->ml;
    this->seen_msg = is_seen(player_ptr, *this->m_ptr);
    this->slept = this->m_ptr->is_asleep();
    this->known = (this->m_ptr->cdis <= MAX_PLAYER_SIGHT) || AngbandSystem::get_instance().is_phase_out();
    this->note_dies = this->m_ptr->get_died_message();
    this->caster_lev = (this->is_monster() && (this->m_caster_ptr != nullptr)) ? this->m_caster_ptr->get_monrace().level : (player_ptr->lev * 2);
}

bool EffectMonster::is_player() const
{
    return this->src_idx == 0;
}

bool EffectMonster::is_monster() const
{
    return this->src_idx > 0;
}

Pos2D EffectMonster::get_position() const
{
    return { this->y, this->x };
}
