#pragma once
/*!
 * @file blue-magic-util.h
 * @brief 青魔法の構造体、初期化処理ヘッダ
 */

#include "system/angband.h"

enum class MonsterAbilityType;

// Blue Magic Cast.
struct bmc_type {
    MonsterAbilityType spell;
    PLAYER_LEVEL plev;
    PLAYER_LEVEL summon_lev;
    bool pet;
    bool no_trump;
    BIT_FLAGS p_mode;
    BIT_FLAGS u_mode;
    BIT_FLAGS g_mode;
};

class PlayerType;
typedef PLAYER_LEVEL (*get_pseudo_monstetr_level_pf)(PlayerType *player_ptr);
bmc_type *initialize_blue_magic_type(
    PlayerType *player_ptr, bmc_type *bmc_ptr, MonsterAbilityType spell, bool success, get_pseudo_monstetr_level_pf get_pseudo_monstetr_level);
