﻿/*!
 * @file variable.c
 * @brief グローバル変数定義 / Angband variables
 * @date 2014/10/05
 * @author
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke<br>
 * <br>
 * This software may be copied and distributed for educational, research,<br>
 * and not for profit purposes provided that this copyright and statement<br>
 * are included in all such copies.  Other copyrights may also apply.<br>
 */

#include "angband.h"
#include "geometry.h"


bool repair_monsters;	/* Hack -- optimize detect monsters */
bool repair_objects;	/* Hack -- optimize detect objects */


/*
 * The number of quarks
 */
STR_OFFSET quark__num;

/*
 * The pointers to the quarks [QUARK_MAX]
 */
concptr *quark__str;



/*** Player information ***/

/*
 * Static player info record
 */
player_type p_body;

/*
 * Pointer to the player info
 */
player_type *p_ptr = &p_body;


MONSTER_IDX pet_t_m_idx;
MONSTER_IDX riding_t_m_idx;

MONSTER_IDX today_mon;
