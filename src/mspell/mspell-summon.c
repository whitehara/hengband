﻿#include "system/angband.h"
#include "mspell/mspell-summon.h"
#include "spell/spells-summon.h"
#include "mspell/monster-spell.h"
#include "spell/spells-type.h"
#include "effect/effect-characteristics.h"
#include "mspell/mspell-util.h"
#include "spell/spells2.h"
#include "spell/process-effect.h"
#include "player/player-move.h"
#include "monster/monster-status.h"

/*!
* @brief 特定条件のモンスター召喚のみPM_ALLOW_UNIQUEを許可する /
* @param floor_ptr 現在フロアへの参照ポインタ
* @param m_idx モンスターID
* @return 召喚可能であればPM_ALLOW_UNIQUEを返す。
*/
static BIT_FLAGS monster_u_mode(floor_type* floor_ptr, MONSTER_IDX m_idx)
{
    BIT_FLAGS u_mode = 0L;
    monster_type* m_ptr = &floor_ptr->m_list[m_idx];
    bool pet = is_pet(m_ptr);
    if (!pet)
        u_mode |= PM_ALLOW_UNIQUE;
    return u_mode;
}

/*!
* @brief 鷹召喚の処理。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param rlev 呪文を唱えるモンスターのレベル
* @param m_idx 呪文を唱えるモンスターID
* @return 召喚したモンスターの数を返す。
*/
MONSTER_NUMBER summon_EAGLE(player_type* target_ptr, POSITION y, POSITION x, int rlev, MONSTER_IDX m_idx)
{
    int count = 0;
    int num = 4 + randint1(3);
    for (int k = 0; k < num; k++) {
        count += summon_specific(target_ptr, m_idx, y, x, rlev, SUMMON_EAGLES, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
    }

    return count;
}

/*!
 * @brief インターネット・エクスプローダー召喚の処理。 /
 * @param target_ptr プレーヤーへの参照ポインタ
 * @param y 対象の地点のy座標
 * @param x 対象の地点のx座標
 * @param rlev 呪文を唱えるモンスターのレベル
 * @param m_idx 呪文を唱えるモンスターID
 * @return 召喚したモンスターの数を返す。
 */
MONSTER_NUMBER summon_IE(player_type* target_ptr, POSITION y, POSITION x, int rlev, MONSTER_IDX m_idx)
{
    BIT_FLAGS mode = 0L;
    int count = 0;
    int num = 2 + randint1(1 + rlev / 20);
    for (int k = 0; k < num; k++) {
        count += summon_named_creature(target_ptr, m_idx, y, x, MON_IE, mode);
    }

    return count;
}

/*!
 * @brief ダンジョンの主召喚の処理。 /
 * @param target_ptr プレーヤーへの参照ポインタ
 * @param y 対象の地点のy座標
 * @param x 対象の地点のx座標
 * @param rlev 呪文を唱えるモンスターのレベル
 * @param m_idx 呪文を唱えるモンスターID
 * @param t_idx 呪文を受けるモンスターID。プレイヤーの場合はdummyで0とする。
 * @param TARGET_TYPE プレイヤーを対象とする場合MONSTER_TO_PLAYER、モンスターを対象とする場合MONSTER_TO_MONSTER
 * @return 召喚したモンスターの数を返す。
 */
MONSTER_NUMBER summon_guardian(player_type* target_ptr, POSITION y, POSITION x, int rlev, MONSTER_IDX m_idx, MONSTER_IDX t_idx, int TARGET_TYPE)
{
    int num = 2 + randint1(3);
    bool mon_to_mon = (TARGET_TYPE == MONSTER_TO_MONSTER);
    bool mon_to_player = (TARGET_TYPE == MONSTER_TO_PLAYER);

    if (r_info[MON_JORMUNGAND].cur_num < r_info[MON_JORMUNGAND].max_num && one_in_(6)) {
        simple_monspell_message(target_ptr, m_idx, t_idx,
            _("地面から水が吹き出した！", "Water blew off from the ground!"),
            _("地面から水が吹き出した！", "Water blew off from the ground!"),
            TARGET_TYPE);

        if (mon_to_player)
            fire_ball_hide(target_ptr, GF_WATER_FLOW, 0, 3, 8);
        else if (mon_to_mon)
            project(target_ptr, t_idx, 8, y, x, 3, GF_WATER_FLOW, PROJECT_GRID | PROJECT_HIDE, -1);
    }

    int count = 0;
    for (int k = 0; k < num; k++) {
        count += summon_specific(target_ptr, m_idx, y, x, rlev, SUMMON_GUARDIANS, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
    }

    return count;
}

/*!
* @brief ロックのクローン召喚の処理。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param m_idx 呪文を唱えるモンスターID
* @return 召喚したモンスターの数を返す。
*/
MONSTER_NUMBER summon_LOCKE_CLONE(player_type* target_ptr, POSITION y, POSITION x, MONSTER_IDX m_idx)
{
    BIT_FLAGS mode = 0L;
    int count = 0;
    int num = randint1(3);
    for (int k = 0; k < num; k++) {
        count += summon_named_creature(target_ptr, m_idx, y, x, MON_LOCKE_CLONE, mode);
    }

    return count;
}

/*!
* @brief シラミ召喚の処理。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param rlev 呪文を唱えるモンスターのレベル
* @param m_idx 呪文を唱えるモンスターID
* @return 召喚したモンスターの数を返す。
*/
MONSTER_NUMBER summon_LOUSE(player_type* target_ptr, POSITION y, POSITION x, int rlev, MONSTER_IDX m_idx)
{
    int count = 0;
    int num = 2 + randint1(3);
    for (int k = 0; k < num; k++) {
        count += summon_specific(target_ptr, m_idx, y, x, rlev, SUMMON_LOUSE, PM_ALLOW_GROUP);
    }

    return count;
}

/*!
* @brief 救援召喚の通常処理。同シンボルのモンスターを召喚する。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param rlev 呪文を唱えるモンスターのレベル
* @param m_idx 呪文を唱えるモンスターID
* @return 召喚したモンスターの数を返す。
*/
MONSTER_NUMBER summon_Kin(player_type* target_ptr, POSITION y, POSITION x, int rlev, MONSTER_IDX m_idx)
{
    int count = 0;
    for (int k = 0; k < 4; k++) {
        count += summon_specific(target_ptr, m_idx, y, x, rlev, SUMMON_KIN, PM_ALLOW_GROUP);
    }

    return count;
}

/*!
* @brief RF6_S_KINの処理。救援召喚。使用するモンスターの種類により、実処理に分岐させる。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param m_idx 呪文を唱えるモンスターID
* @param t_idx 呪文を受けるモンスターID。プレイヤーの場合はdummyで0とする。
* @param TARGET_TYPE プレイヤーを対象とする場合MONSTER_TO_PLAYER、モンスターを対象とする場合MONSTER_TO_MONSTER
* @return 召喚したモンスターの数を返す。
*/
void spell_RF6_S_KIN(player_type* target_ptr, POSITION y, POSITION x, MONSTER_IDX m_idx, MONSTER_IDX t_idx, int TARGET_TYPE)
{
    floor_type* floor_ptr = target_ptr->current_floor_ptr;
    monster_type* m_ptr = &floor_ptr->m_list[m_idx];
    monster_race* r_ptr = &r_info[m_ptr->r_idx];
    DEPTH rlev = monster_level_idx(floor_ptr, m_idx);
    int count = 0;
    GAME_TEXT m_name[MAX_NLEN], t_name[MAX_NLEN], m_poss[80];
    monster_name(target_ptr, m_idx, m_name);
    monster_name(target_ptr, t_idx, t_name);
    monster_desc(target_ptr, m_poss, m_ptr, MD_PRON_VISIBLE | MD_POSSESSIVE);

    disturb(target_ptr, TRUE, TRUE);
    bool known = monster_near_player(floor_ptr, m_idx, t_idx);
    bool see_either = see_monster(floor_ptr, m_idx) || see_monster(floor_ptr, t_idx);
    bool mon_to_mon = (TARGET_TYPE == MONSTER_TO_MONSTER);
    bool mon_to_player = (TARGET_TYPE == MONSTER_TO_PLAYER);
    if (m_ptr->r_idx == MON_SERPENT || m_ptr->r_idx == MON_ZOMBI_SERPENT) {
        monspell_message(target_ptr, m_idx, t_idx,
            _("%^sが何かをつぶやいた。", "%^s mumbles."),
            _("%^sがダンジョンの主を召喚した。", "%^s magically summons guardians of dungeons."),
            _("%^sがダンジョンの主を召喚した。", "%^s magically summons guardians of dungeons."),
            TARGET_TYPE);
    } else {
        if (mon_to_player || (mon_to_mon && known && see_either))
            disturb(target_ptr, TRUE, TRUE);

        if (target_ptr->blind) {
            if (mon_to_player)
                msg_format(_("%^sが何かをつぶやいた。", "%^s mumbles."), m_name);
        } else {
            if (mon_to_player || (mon_to_mon && known && see_either)) {
                _(msg_format("%sが魔法で%sを召喚した。", m_name, ((r_ptr->flags1 & RF1_UNIQUE) ? "手下" : "仲間")),
                    msg_format("%^s magically summons %s %s.", m_name, m_poss, ((r_ptr->flags1 & RF1_UNIQUE) ? "minions" : "kin")));
            }
        }

        if (mon_to_mon && known && !see_either)
            floor_ptr->monster_noise = TRUE;
    }

    switch (m_ptr->r_idx) {
    case MON_MENELDOR:
    case MON_GWAIHIR:
    case MON_THORONDOR:
        count += summon_EAGLE(target_ptr, y, x, rlev, m_idx);
        break;

    case MON_BULLGATES:
        count += summon_IE(target_ptr, y, x, rlev, m_idx);
        break;

    case MON_SERPENT:
    case MON_ZOMBI_SERPENT:
        count += summon_guardian(target_ptr, y, x, rlev, m_idx, t_idx, TARGET_TYPE);
        break;

    case MON_CALDARM:
        count += summon_LOCKE_CLONE(target_ptr, y, x, m_idx);
        break;

    case MON_LOUSY:
        count += summon_LOUSE(target_ptr, y, x, rlev, m_idx);
        break;

    default:
        count += summon_Kin(target_ptr, y, x, rlev, m_idx);
        break;
    }

    if (target_ptr->blind && count && mon_to_player)
        msg_print(_("多くのものが間近に現れた音がする。", "You hear many things appear nearby."));

    if (known && !see_monster(floor_ptr, t_idx) && count && mon_to_mon)
        floor_ptr->monster_noise = TRUE;
}

/*!
* @brief RF6_S_CYBERの処理。サイバー・デーモン召喚。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param m_idx 呪文を唱えるモンスターID
* @param t_idx 呪文を受けるモンスターID。プレイヤーの場合はdummyで0とする。
* @param TARGET_TYPE プレイヤーを対象とする場合MONSTER_TO_PLAYER、モンスターを対象とする場合MONSTER_TO_MONSTER
* @return 召喚したモンスターの数を返す。
*/
void spell_RF6_S_CYBER(player_type* target_ptr, POSITION y, POSITION x, MONSTER_IDX m_idx, MONSTER_IDX t_idx, int TARGET_TYPE)
{
    int count = 0;
    floor_type* floor_ptr = target_ptr->current_floor_ptr;
    monster_type* m_ptr = &floor_ptr->m_list[m_idx];
    DEPTH rlev = monster_level_idx(floor_ptr, m_idx);
    bool mon_to_mon = (TARGET_TYPE == MONSTER_TO_MONSTER);
    bool mon_to_player = (TARGET_TYPE == MONSTER_TO_PLAYER);

    monspell_message(target_ptr, m_idx, t_idx,
        _("%^sが何かをつぶやいた。", "%^s mumbles."),
        _("%^sがサイバーデーモンを召喚した！", "%^s magically summons Cyberdemons!"),
        _("%^sがサイバーデーモンを召喚した！", "%^s magically summons Cyberdemons!"),
        TARGET_TYPE);

    if (is_friendly(m_ptr) && mon_to_mon) {
        count += summon_specific(target_ptr, m_idx, y, x, rlev, SUMMON_CYBER, (PM_ALLOW_GROUP));
    } else {
        count += summon_cyber(target_ptr, m_idx, y, x);
    }

    if (target_ptr->blind && count && mon_to_player)
        msg_print(_("重厚な足音が近くで聞こえる。", "You hear heavy steps nearby."));

    if (monster_near_player(floor_ptr, m_idx, t_idx) && !see_monster(floor_ptr, t_idx) && count && mon_to_mon)
        floor_ptr->monster_noise = TRUE;
}

/*!
* @brief RF6_S_MONSTERの処理。モンスター一体召喚。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param m_idx 呪文を唱えるモンスターID
* @param t_idx 呪文を受けるモンスターID。プレイヤーの場合はdummyで0とする。
* @param TARGET_TYPE プレイヤーを対象とする場合MONSTER_TO_PLAYER、モンスターを対象とする場合MONSTER_TO_MONSTER
* @return 召喚したモンスターの数を返す。
*/
void spell_RF6_S_MONSTER(player_type* target_ptr, POSITION y, POSITION x, MONSTER_IDX m_idx, MONSTER_IDX t_idx, int TARGET_TYPE)
{
    monspell_message(target_ptr, m_idx, t_idx,
        _("%^sが何かをつぶやいた。", "%^s mumbles."),
        _("%^sが魔法で仲間を召喚した！", "%^s magically summons help!"),
        _("%^sが魔法で仲間を召喚した！", "%^s magically summons help!"),
        TARGET_TYPE);

    floor_type* floor_ptr = target_ptr->current_floor_ptr;
    int count = 0;
    DEPTH rlev = monster_level_idx(floor_ptr, m_idx);
    bool mon_to_mon = (TARGET_TYPE == MONSTER_TO_MONSTER);
    bool mon_to_player = (TARGET_TYPE == MONSTER_TO_PLAYER);
    for (int k = 0; k < 1; k++) {
        if (mon_to_player)
            count += summon_specific(target_ptr, m_idx, y, x, rlev, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));

        if (mon_to_mon)
            count += summon_specific(target_ptr, m_idx, y, x, rlev, 0, (monster_u_mode(floor_ptr, m_idx)));
    }

    if (target_ptr->blind && count && mon_to_player)
        msg_print(_("何かが間近に現れた音がする。", "You hear something appear nearby."));

    if (monster_near_player(floor_ptr, m_idx, t_idx) && !see_monster(floor_ptr, t_idx) && count && mon_to_mon)
        floor_ptr->monster_noise = TRUE;
}

/*!
* @brief RF6_S_MONSTERSの処理。モンスター複数召喚。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param m_idx 呪文を唱えるモンスターID
* @param t_idx 呪文を受けるモンスターID。プレイヤーの場合はdummyで0とする。
* @param TARGET_TYPE プレイヤーを対象とする場合MONSTER_TO_PLAYER、モンスターを対象とする場合MONSTER_TO_MONSTER
* @return 召喚したモンスターの数を返す。
*/
void spell_RF6_S_MONSTERS(player_type* target_ptr, POSITION y, POSITION x, MONSTER_IDX m_idx, MONSTER_IDX t_idx, int TARGET_TYPE)
{
    monspell_message(target_ptr, m_idx, t_idx,
        _("%^sが何かをつぶやいた。", "%^s mumbles."),
        _("%^sが魔法でモンスターを召喚した！", "%^s magically summons monsters!"),
        _("%^sが魔法でモンスターを召喚した！", "%^s magically summons monsters!"),
        TARGET_TYPE);

    floor_type* floor_ptr = target_ptr->current_floor_ptr;
    int count = 0;
    DEPTH rlev = monster_level_idx(floor_ptr, m_idx);
    bool mon_to_mon = (TARGET_TYPE == MONSTER_TO_MONSTER);
    bool mon_to_player = (TARGET_TYPE == MONSTER_TO_PLAYER);
    for (int k = 0; k < S_NUM_6; k++) {
        if (mon_to_player)
            count += summon_specific(target_ptr, m_idx, y, x, rlev, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));

        if (mon_to_mon)
            count += summon_specific(target_ptr, m_idx, y, x, rlev, 0, (PM_ALLOW_GROUP | monster_u_mode(floor_ptr, m_idx)));
    }

    if (target_ptr->blind && count && mon_to_player)
        msg_print(_("多くのものが間近に現れた音がする。", "You hear many things appear nearby."));

    if (monster_near_player(floor_ptr, m_idx, t_idx) && !see_monster(floor_ptr, t_idx) && count && mon_to_mon)
        floor_ptr->monster_noise = TRUE;
}

/*!
* @brief RF6_S_ANTの処理。アリ召喚。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param m_idx 呪文を唱えるモンスターID
* @param t_idx 呪文を受けるモンスターID。プレイヤーの場合はdummyで0とする。
* @param TARGET_TYPE プレイヤーを対象とする場合MONSTER_TO_PLAYER、モンスターを対象とする場合MONSTER_TO_MONSTER
* @return 召喚したモンスターの数を返す。
*/
void spell_RF6_S_ANT(player_type* target_ptr, POSITION y, POSITION x, MONSTER_IDX m_idx, MONSTER_IDX t_idx, int TARGET_TYPE)
{
    monspell_message(target_ptr, m_idx, t_idx,
        _("%^sが何かをつぶやいた。", "%^s mumbles."),
        _("%^sが魔法でアリを召喚した。", "%^s magically summons ants."),
        _("%^sが魔法でアリを召喚した。", "%^s magically summons ants."),
        TARGET_TYPE);

    int count = 0;
    floor_type* floor_ptr = target_ptr->current_floor_ptr;
    DEPTH rlev = monster_level_idx(floor_ptr, m_idx);
    bool mon_to_mon = (TARGET_TYPE == MONSTER_TO_MONSTER);
    bool mon_to_player = (TARGET_TYPE == MONSTER_TO_PLAYER);
    for (int k = 0; k < S_NUM_6; k++) {
        count += summon_specific(target_ptr, m_idx, y, x, rlev, SUMMON_ANT, PM_ALLOW_GROUP);
    }

    if (target_ptr->blind && count && mon_to_player)
        msg_print(_("多くのものが間近に現れた音がする。", "You hear many things appear nearby."));

    if (monster_near_player(floor_ptr, m_idx, t_idx) && !see_monster(floor_ptr, t_idx) && count && mon_to_mon)
        floor_ptr->monster_noise = TRUE;
}

/*!
* @brief RF6_S_SPIDERの処理。クモ召喚。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param m_idx 呪文を唱えるモンスターID
* @param t_idx 呪文を受けるモンスターID。プレイヤーの場合はdummyで0とする。
* @param TARGET_TYPE プレイヤーを対象とする場合MONSTER_TO_PLAYER、モンスターを対象とする場合MONSTER_TO_MONSTER
* @return 召喚したモンスターの数を返す。
*/
void spell_RF6_S_SPIDER(player_type* target_ptr, POSITION y, POSITION x, MONSTER_IDX m_idx, MONSTER_IDX t_idx, int TARGET_TYPE)
{
    monspell_message(target_ptr, m_idx, t_idx,
        _("%^sが何かをつぶやいた。", "%^s mumbles."),
        _("%^sが魔法でクモを召喚した。", "%^s magically summons spiders."),
        _("%^sが魔法でクモを召喚した。", "%^s magically summons spiders."),
        TARGET_TYPE);

    int count = 0;
    bool mon_to_mon = (TARGET_TYPE == MONSTER_TO_MONSTER);
    bool mon_to_player = (TARGET_TYPE == MONSTER_TO_PLAYER);
    floor_type* floor_ptr = target_ptr->current_floor_ptr;
    DEPTH rlev = monster_level_idx(floor_ptr, m_idx);
    for (int k = 0; k < S_NUM_6; k++) {
        count += summon_specific(target_ptr, m_idx, y, x, rlev, SUMMON_SPIDER, PM_ALLOW_GROUP);
    }

    if (target_ptr->blind && count && mon_to_player)
        msg_print(_("多くのものが間近に現れた音がする。", "You hear many things appear nearby."));

    if (monster_near_player(floor_ptr, m_idx, t_idx) && !see_monster(floor_ptr, t_idx) && count && mon_to_mon)
        floor_ptr->monster_noise = TRUE;
}

/*!
* @brief RF6_S_HOUNDの処理。ハウンド召喚。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param m_idx 呪文を唱えるモンスターID
* @param t_idx 呪文を受けるモンスターID。プレイヤーの場合はdummyで0とする。
* @param TARGET_TYPE プレイヤーを対象とする場合MONSTER_TO_PLAYER、モンスターを対象とする場合MONSTER_TO_MONSTER
* @return 召喚したモンスターの数を返す。
*/
void spell_RF6_S_HOUND(player_type* target_ptr, POSITION y, POSITION x, MONSTER_IDX m_idx, MONSTER_IDX t_idx, int TARGET_TYPE)
{
    monspell_message(target_ptr, m_idx, t_idx,
        _("%^sが何かをつぶやいた。", "%^s mumbles."),
        _("%^sが魔法でハウンドを召喚した。", "%^s magically summons hounds."),
        _("%^sが魔法でハウンドを召喚した。", "%^s magically summons hounds."),
        TARGET_TYPE);

    int count = 0;
    floor_type* floor_ptr = target_ptr->current_floor_ptr;
    DEPTH rlev = monster_level_idx(floor_ptr, m_idx);
    bool mon_to_mon = (TARGET_TYPE == MONSTER_TO_MONSTER);
    bool mon_to_player = (TARGET_TYPE == MONSTER_TO_PLAYER);
    for (int k = 0; k < S_NUM_4; k++) {
        count += summon_specific(target_ptr, m_idx, y, x, rlev, SUMMON_HOUND, PM_ALLOW_GROUP);
    }

    if (target_ptr->blind && count && mon_to_player)
        msg_print(_("多くのものが間近に現れた音がする。", "You hear many things appear nearby."));

    if (monster_near_player(floor_ptr, m_idx, t_idx) && !see_monster(floor_ptr, t_idx) && count && mon_to_mon)
        floor_ptr->monster_noise = TRUE;
}

/*!
* @brief RF6_S_HYDRAの処理。ヒドラ召喚。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param m_idx 呪文を唱えるモンスターID
* @param t_idx 呪文を受けるモンスターID。プレイヤーの場合はdummyで0とする。
* @param TARGET_TYPE プレイヤーを対象とする場合MONSTER_TO_PLAYER、モンスターを対象とする場合MONSTER_TO_MONSTER
* @return 召喚したモンスターの数を返す。
*/
void spell_RF6_S_HYDRA(player_type* target_ptr, POSITION y, POSITION x, MONSTER_IDX m_idx, MONSTER_IDX t_idx, int TARGET_TYPE)
{
    monspell_message(target_ptr, m_idx, t_idx,
        _("%^sが何かをつぶやいた。", "%^s mumbles."),
        _("%^sが魔法でヒドラを召喚した。", "%^s magically summons hydras."),
        _("%^sが魔法でヒドラを召喚した。", "%^s magically summons hydras."),
        TARGET_TYPE);

    int count = 0;
    floor_type* floor_ptr = target_ptr->current_floor_ptr;
    DEPTH rlev = monster_level_idx(floor_ptr, m_idx);
    bool mon_to_mon = (TARGET_TYPE == MONSTER_TO_MONSTER);
    bool mon_to_player = (TARGET_TYPE == MONSTER_TO_PLAYER);
    for (int k = 0; k < S_NUM_4; k++) {
        count += summon_specific(target_ptr, m_idx, y, x, rlev, SUMMON_HYDRA, PM_ALLOW_GROUP);
    }

    if (target_ptr->blind && count && mon_to_player)
        msg_print(_("多くのものが間近に現れた音がする。", "You hear many things appear nearby."));

    if (monster_near_player(floor_ptr, m_idx, t_idx) && !see_monster(floor_ptr, t_idx) && count && mon_to_mon)
        floor_ptr->monster_noise = TRUE;
}

/*!
* @brief RF6_S_ANGELの処理。天使一体召喚。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param m_idx 呪文を唱えるモンスターID
* @param t_idx 呪文を受けるモンスターID。プレイヤーの場合はdummyで0とする。
* @param TARGET_TYPE プレイヤーを対象とする場合MONSTER_TO_PLAYER、モンスターを対象とする場合MONSTER_TO_MONSTER
* @return 召喚したモンスターの数を返す。
*/
void spell_RF6_S_ANGEL(player_type* target_ptr, POSITION y, POSITION x, MONSTER_IDX m_idx, MONSTER_IDX t_idx, int TARGET_TYPE)
{
    monspell_message(target_ptr, m_idx, t_idx,
        _("%^sが何かをつぶやいた。", "%^s mumbles."),
        _("%^sが魔法で天使を召喚した！", "%^s magically summons an angel!"),
        _("%^sが魔法で天使を召喚した！", "%^s magically summons an angel!"),
        TARGET_TYPE);

    floor_type* floor_ptr = target_ptr->current_floor_ptr;
    monster_type* m_ptr = &floor_ptr->m_list[m_idx];
    monster_race* r_ptr = &r_info[m_ptr->r_idx];
    int num = 1;
    if ((r_ptr->flags1 & RF1_UNIQUE) && !easy_band) {
        num += r_ptr->level / 40;
    }

    DEPTH rlev = monster_level_idx(floor_ptr, m_idx);
    int count = 0;
    for (int k = 0; k < num; k++) {
        count += summon_specific(target_ptr, m_idx, y, x, rlev, SUMMON_ANGEL, PM_ALLOW_GROUP);
    }

    if (count < 2) {
        if (target_ptr->blind && count)
            msg_print(_("何かが間近に現れた音がする。", "You hear something appear nearby."));
    } else {
        if (target_ptr->blind)
            msg_print(_("多くのものが間近に現れた音がする。", "You hear many things appear nearby."));
    }

    bool mon_to_mon = (TARGET_TYPE == MONSTER_TO_MONSTER);
    if (monster_near_player(floor_ptr, m_idx, t_idx) && !see_monster(floor_ptr, t_idx) && count && mon_to_mon)
        floor_ptr->monster_noise = TRUE;
}

/*!
* @brief RF6_S_DEMONの処理。デーモン一体召喚。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param m_idx 呪文を唱えるモンスターID
* @param t_idx 呪文を受けるモンスターID。プレイヤーの場合はdummyで0とする。
* @param TARGET_TYPE プレイヤーを対象とする場合MONSTER_TO_PLAYER、モンスターを対象とする場合MONSTER_TO_MONSTER
* @return 召喚したモンスターの数を返す。
*/
void spell_RF6_S_DEMON(player_type* target_ptr, POSITION y, POSITION x, MONSTER_IDX m_idx, MONSTER_IDX t_idx, int TARGET_TYPE)
{
    monspell_message(target_ptr, m_idx, t_idx,
        _("%^sが何かをつぶやいた。", "%^s mumbles."),
        _("%^sは魔法で混沌の宮廷から悪魔を召喚した！", "%^s magically summons a demon from the Courts of Chaos!"),
        _("%^sは魔法で混沌の宮廷から悪魔を召喚した！", "%^s magically summons a demon from the Courts of Chaos!"),
        TARGET_TYPE);

    floor_type* floor_ptr = target_ptr->current_floor_ptr;
    DEPTH rlev = monster_level_idx(floor_ptr, m_idx);
    int count = 0;
    for (int k = 0; k < 1; k++) {
        count += summon_specific(target_ptr, m_idx, y, x, rlev, SUMMON_DEMON, PM_ALLOW_GROUP);
    }

    if (target_ptr->blind && count)
        msg_print(_("何かが間近に現れた音がする。", "You hear something appear nearby."));

    bool mon_to_mon = (TARGET_TYPE == MONSTER_TO_MONSTER);
    if (monster_near_player(floor_ptr, m_idx, t_idx) && !see_monster(floor_ptr, t_idx) && count && mon_to_mon)
        floor_ptr->monster_noise = TRUE;
}

/*!
* @brief RF6_S_UNDEADの処理。アンデッド一体召喚。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param m_idx 呪文を唱えるモンスターID
* @param t_idx 呪文を受けるモンスターID。プレイヤーの場合はdummyで0とする。
* @param TARGET_TYPE プレイヤーを対象とする場合MONSTER_TO_PLAYER、モンスターを対象とする場合MONSTER_TO_MONSTER
* @return 召喚したモンスターの数を返す。
*/
void spell_RF6_S_UNDEAD(player_type* target_ptr, POSITION y, POSITION x, MONSTER_IDX m_idx, MONSTER_IDX t_idx, int TARGET_TYPE)
{
    monspell_message(target_ptr, m_idx, t_idx,
        _("%^sが何かをつぶやいた。", "%^s mumbles."),
        _("%^sが魔法でアンデッドの強敵を召喚した！", "%^s magically summons an undead adversary!"),
        _("%sが魔法でアンデッドを召喚した。", "%^s magically summons undead."),
        TARGET_TYPE);

    floor_type* floor_ptr = target_ptr->current_floor_ptr;
    DEPTH rlev = monster_level_idx(floor_ptr, m_idx);
    int count = 0;
    for (int k = 0; k < 1; k++) {
        count += summon_specific(target_ptr, m_idx, y, x, rlev, SUMMON_UNDEAD, PM_ALLOW_GROUP);
    }

    if (target_ptr->blind && count)
        msg_print(_("何かが間近に現れた音がする。", "You hear something appear nearby."));

    bool mon_to_mon = (TARGET_TYPE == MONSTER_TO_MONSTER);
    if (monster_near_player(floor_ptr, m_idx, t_idx) && !see_monster(floor_ptr, t_idx) && count && mon_to_mon)
        floor_ptr->monster_noise = TRUE;
}

/*!
* @brief RF6_S_DRAGONの処理。ドラゴン一体召喚。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param m_idx 呪文を唱えるモンスターID
* @param t_idx 呪文を受けるモンスターID。プレイヤーの場合はdummyで0とする。
* @param TARGET_TYPE プレイヤーを対象とする場合MONSTER_TO_PLAYER、モンスターを対象とする場合MONSTER_TO_MONSTER
* @return 召喚したモンスターの数を返す。
*/
void spell_RF6_S_DRAGON(player_type* target_ptr, POSITION y, POSITION x, MONSTER_IDX m_idx, MONSTER_IDX t_idx, int TARGET_TYPE)
{
    monspell_message(target_ptr, m_idx, t_idx,
        _("%^sが何かをつぶやいた。", "%^s mumbles."),
        _("%^sが魔法でドラゴンを召喚した！", "%^s magically summons a dragon!"),
        _("%^sが魔法でドラゴンを召喚した！", "%^s magically summons a dragon!"),
        TARGET_TYPE);

    floor_type* floor_ptr = target_ptr->current_floor_ptr;
    DEPTH rlev = monster_level_idx(floor_ptr, m_idx);
    int count = 0;
    for (int k = 0; k < 1; k++) {
        count += summon_specific(target_ptr, m_idx, y, x, rlev, SUMMON_DRAGON, PM_ALLOW_GROUP);
    }

    if (target_ptr->blind && count)
        msg_print(_("何かが間近に現れた音がする。", "You hear something appear nearby."));

    bool mon_to_mon = (TARGET_TYPE == MONSTER_TO_MONSTER);
    if (monster_near_player(floor_ptr, m_idx, t_idx) && !see_monster(floor_ptr, t_idx) && count && mon_to_mon)
        floor_ptr->monster_noise = TRUE;
}

/*!
* @brief ナズグル戦隊召喚の処理。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param m_idx 呪文を唱えるモンスターID
* @return 召喚したモンスターの数を返す。
*/
MONSTER_NUMBER summon_NAZGUL(player_type* target_ptr, POSITION y, POSITION x, MONSTER_IDX m_idx)
{
    BIT_FLAGS mode = 0L;
    POSITION cy = y;
    POSITION cx = x;
    GAME_TEXT m_name[MAX_NLEN];
    monster_name(target_ptr, m_idx, m_name);

    if (target_ptr->blind)
        msg_format(_("%^sが何かをつぶやいた。", "%^s mumbles."), m_name);
    else
        msg_format(_("%^sが魔法で幽鬼戦隊を召喚した！", "%^s magically summons rangers of Nazgul!"), m_name);

    msg_print(NULL);

    int count = 0;
    for (int k = 0; k < 30; k++) {
        if (!summon_possible(target_ptr, cy, cx) || !is_cave_empty_bold(target_ptr, cy, cx)) {
            int j;
            for (j = 100; j > 0; j--) {
                scatter(target_ptr, &cy, &cx, y, x, 2, 0);
                if (is_cave_empty_bold(target_ptr, cy, cx))
                    break;
            }

            if (!j)
                break;
        }

        if (!is_cave_empty_bold(target_ptr, cy, cx))
            continue;

        if (!summon_named_creature(target_ptr, m_idx, cy, cx, MON_NAZGUL, mode))
            continue;

        y = cy;
        x = cx;
        count++;
        if (count == 1)
            msg_format(_("「幽鬼戦隊%d号、ナズグル・ブラック！」",
                           "A Nazgul says 'Nazgul-Rangers Number %d, Nazgul-Black!'"),
                count);
        else
            msg_format(_("「同じく%d号、ナズグル・ブラック！」",
                           "Another one says 'Number %d, Nazgul-Black!'"),
                count);

        msg_print(NULL);
    }

    msg_format(_("「%d人そろって、リングレンジャー！」",
                   "They say 'The %d meets! We are the Ring-Ranger!'."),
        count);
    msg_print(NULL);
    return count;
}

/*!
* @brief RF6_S_HI_UNDEADの処理。強力なアンデッド召喚。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param m_idx 呪文を唱えるモンスターID
* @param t_idx 呪文を受けるモンスターID。プレイヤーの場合はdummyで0とする。
* @param TARGET_TYPE プレイヤーを対象とする場合MONSTER_TO_PLAYER、モンスターを対象とする場合MONSTER_TO_MONSTER
* @return 召喚したモンスターの数を返す。
*/
void spell_RF6_S_HI_UNDEAD(player_type* target_ptr, POSITION y, POSITION x, MONSTER_IDX m_idx, MONSTER_IDX t_idx, int TARGET_TYPE)
{
    GAME_TEXT m_name[MAX_NLEN];
    monster_name(target_ptr, m_idx, m_name);

    disturb(target_ptr, TRUE, TRUE);

    floor_type* floor_ptr = target_ptr->current_floor_ptr;
    monster_type* m_ptr = &floor_ptr->m_list[m_idx];
    bool mon_to_player = (TARGET_TYPE == MONSTER_TO_PLAYER);
    bool mon_to_mon = (TARGET_TYPE == MONSTER_TO_MONSTER);
    int count = 0;
    if (((m_ptr->r_idx == MON_MORGOTH) || (m_ptr->r_idx == MON_SAURON) || (m_ptr->r_idx == MON_ANGMAR)) && ((r_info[MON_NAZGUL].cur_num + 2) < r_info[MON_NAZGUL].max_num) && mon_to_player) {
        count += summon_NAZGUL(target_ptr, y, x, m_idx);
    } else {
        monspell_message(target_ptr, m_idx, t_idx,
            _("%^sが何かをつぶやいた。", "%^s mumbles."),
            _("%^sが魔法で強力なアンデッドを召喚した！", "%^s magically summons greater undead!"),
            _("%sが魔法でアンデッドを召喚した。", "%^s magically summons undead."),
            TARGET_TYPE);

        DEPTH rlev = monster_level_idx(floor_ptr, m_idx);
        for (int k = 0; k < S_NUM_6; k++) {
            if (mon_to_player)
                count += summon_specific(target_ptr, m_idx, y, x, rlev, SUMMON_HI_UNDEAD, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));

            if (mon_to_mon)
                count += summon_specific(target_ptr, m_idx, y, x, rlev, SUMMON_HI_UNDEAD, (PM_ALLOW_GROUP | monster_u_mode(floor_ptr, m_idx)));
        }
    }

    if (target_ptr->blind && count && mon_to_player) {
        msg_print(_("間近で何か多くのものが這い回る音が聞こえる。", "You hear many creepy things appear nearby."));
    }

    if (monster_near_player(floor_ptr, m_idx, t_idx) && !see_monster(floor_ptr, t_idx) && count && mon_to_mon)
        floor_ptr->monster_noise = TRUE;
}

/*!
* @brief RF6_S_HI_DRAGONの処理。古代ドラゴン召喚。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param m_idx 呪文を唱えるモンスターID
* @param t_idx 呪文を受けるモンスターID。プレイヤーの場合はdummyで0とする。
* @param TARGET_TYPE プレイヤーを対象とする場合MONSTER_TO_PLAYER、モンスターを対象とする場合MONSTER_TO_MONSTER
* @return 召喚したモンスターの数を返す。
*/
void spell_RF6_S_HI_DRAGON(player_type* target_ptr, POSITION y, POSITION x, MONSTER_IDX m_idx, MONSTER_IDX t_idx, int TARGET_TYPE)
{
    monspell_message(target_ptr, m_idx, t_idx,
        _("%^sが何かをつぶやいた。", "%^s mumbles."),
        _("%^sが魔法で古代ドラゴンを召喚した！", "%^s magically summons ancient dragons!"),
        _("%^sが魔法で古代ドラゴンを召喚した！", "%^s magically summons ancient dragons!"),
        TARGET_TYPE);

    floor_type* floor_ptr = target_ptr->current_floor_ptr;
    DEPTH rlev = monster_level_idx(floor_ptr, m_idx);
    bool mon_to_mon = (TARGET_TYPE == MONSTER_TO_MONSTER);
    bool mon_to_player = (TARGET_TYPE == MONSTER_TO_PLAYER);
    int count = 0;
    for (int k = 0; k < S_NUM_4; k++) {
        if (mon_to_player)
            count += summon_specific(target_ptr, m_idx, y, x, rlev, SUMMON_HI_DRAGON, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));

        if (mon_to_mon)
            count += summon_specific(target_ptr, m_idx, y, x, rlev, SUMMON_HI_DRAGON, (PM_ALLOW_GROUP | monster_u_mode(floor_ptr, m_idx)));
    }

    if (target_ptr->blind && count && mon_to_player) {
        msg_print(_("多くの力強いものが間近に現れた音が聞こえる。", "You hear many powerful things appear nearby."));
    }

    if (monster_near_player(floor_ptr, m_idx, t_idx) && !see_monster(floor_ptr, t_idx) && count && mon_to_mon)
        floor_ptr->monster_noise = TRUE;
}

/*!
* @brief RF6_S_AMBERITESの処理。アンバーの王族召喚。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param m_idx 呪文を唱えるモンスターID
* @param t_idx 呪文を受けるモンスターID。プレイヤーの場合はdummyで0とする。
* @param TARGET_TYPE プレイヤーを対象とする場合MONSTER_TO_PLAYER、モンスターを対象とする場合MONSTER_TO_MONSTER
* @return 召喚したモンスターの数を返す。
*/
void spell_RF6_S_AMBERITES(player_type* target_ptr, POSITION y, POSITION x, MONSTER_IDX m_idx, MONSTER_IDX t_idx, int TARGET_TYPE)
{
    monspell_message(target_ptr, m_idx, t_idx,
        _("%^sが何かをつぶやいた。", "%^s mumbles."),
        _("%^sがアンバーの王族を召喚した！", "%^s magically summons Lords of Amber!"),
        _("%^sがアンバーの王族を召喚した！", "%^s magically summons Lords of Amber!"),
        TARGET_TYPE);

    int count = 0;
    floor_type* floor_ptr = target_ptr->current_floor_ptr;
    DEPTH rlev = monster_level_idx(floor_ptr, m_idx);
    bool mon_to_mon = (TARGET_TYPE == MONSTER_TO_MONSTER);
    bool mon_to_player = (TARGET_TYPE == MONSTER_TO_PLAYER);
    for (int k = 0; k < S_NUM_4; k++) {
        count += summon_specific(target_ptr, m_idx, y, x, rlev, SUMMON_AMBERITES, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
    }

    if (target_ptr->blind && count && mon_to_player) {
        msg_print(_("不死の者が近くに現れるのが聞こえた。", "You hear immortal beings appear nearby."));
    }

    if (monster_near_player(floor_ptr, m_idx, t_idx) && !see_monster(floor_ptr, t_idx) && count && mon_to_mon)
        floor_ptr->monster_noise = TRUE;
}

/*!
* @brief RF6_S_UNIQUEの処理。ユニーク・モンスター召喚。 /
* @param target_ptr プレーヤーへの参照ポインタ
* @param y 対象の地点のy座標
* @param x 対象の地点のx座標
* @param m_idx 呪文を唱えるモンスターID
* @param t_idx 呪文を受けるモンスターID。プレイヤーの場合はdummyで0とする。
* @param TARGET_TYPE プレイヤーを対象とする場合MONSTER_TO_PLAYER、モンスターを対象とする場合MONSTER_TO_MONSTER
* @return 召喚したモンスターの数を返す。
*/
void spell_RF6_S_UNIQUE(player_type* target_ptr, POSITION y, POSITION x, MONSTER_IDX m_idx, MONSTER_IDX t_idx, int TARGET_TYPE)
{
    monspell_message(target_ptr, m_idx, t_idx,
        _("%^sが何かをつぶやいた。", "%^s mumbles."),
        _("%^sが魔法で特別な強敵を召喚した！", "%^s magically summons special opponents!"),
        _("%^sが魔法で特別な強敵を召喚した！", "%^s magically summons special opponents!"),
        TARGET_TYPE);

    floor_type* floor_ptr = target_ptr->current_floor_ptr;
    monster_type* m_ptr = &floor_ptr->m_list[m_idx];
    DEPTH rlev = monster_level_idx(floor_ptr, m_idx);
    bool mon_to_mon = (TARGET_TYPE == MONSTER_TO_MONSTER);
    bool mon_to_player = (TARGET_TYPE == MONSTER_TO_PLAYER);
    bool uniques_are_summoned = FALSE;
    int count = 0;
    for (int k = 0; k < S_NUM_4; k++) {
        count += summon_specific(target_ptr, m_idx, y, x, rlev, SUMMON_UNIQUE, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
    }

    if (count)
        uniques_are_summoned = TRUE;

    int non_unique_type = SUMMON_HI_UNDEAD;
    if ((m_ptr->sub_align & (SUB_ALIGN_GOOD | SUB_ALIGN_EVIL)) == (SUB_ALIGN_GOOD | SUB_ALIGN_EVIL))
        non_unique_type = 0;
    else if (m_ptr->sub_align & SUB_ALIGN_GOOD)
        non_unique_type = SUMMON_ANGEL;

    for (int k = count; k < S_NUM_4; k++) {
        count += summon_specific(target_ptr, m_idx, y, x, rlev, non_unique_type, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
    }

    if (target_ptr->blind && count && mon_to_player) {
        msg_format(_("多くの%sが間近に現れた音が聞こえる。", "You hear many %s appear nearby."),
            uniques_are_summoned ? _("力強いもの", "powerful things") : _("もの", "things"));
    }

    if (monster_near_player(floor_ptr, m_idx, t_idx) && !see_monster(floor_ptr, t_idx) && count && mon_to_mon)
        floor_ptr->monster_noise = TRUE;
}