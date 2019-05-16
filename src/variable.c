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

s16b command_cmd;		/* Current "Angband Command" */

COMMAND_ARG command_arg;	/*!< 各種コマンドの汎用的な引数として扱う / Gives argument of current command */
COMMAND_NUM command_rep;	/*!< 各種コマンドの汎用的なリピート数として扱う / Gives repetition of current command */
DIRECTION command_dir;		/*!< 各種コマンドの汎用的な方向値処理として扱う/ Gives direction of current command */

s16b command_see;		/* See "object1.c" */
s16b command_wrk;		/* See "object1.c" */

TERM_LEN command_gap = 999;         /* See "object1.c" */

s16b command_new;		/* Command chaining from inven/equip view */

bool repair_monsters;	/* Hack -- optimize detect monsters */
bool repair_objects;	/* Hack -- optimize detect objects */

MONSTER_IDX hack_m_idx = 0;	/* Hack -- see "process_monsters()" */
MONSTER_IDX hack_m_idx_ii = 0;

int total_friends = 0;

bool reinit_wilderness = FALSE;

/*
 * Dungeon size info
 */

POSITION panel_row_min, panel_row_max;
POSITION panel_col_min, panel_col_max;
POSITION panel_col_prt, panel_row_prt;


/*
 * User info
 */
int player_uid;
int player_euid;
int player_egid;

/*
 * Buffer to hold the current savefile name
 * 'savefile' holds full path name. 'savefile_base' holds only base name.
 */
char savefile[1024];
char savefile_base[40];


pos_list tmp_pos;

/*
 * The number of quarks
 */
STR_OFFSET quark__num;

/*
 * The pointers to the quarks [QUARK_MAX]
 */
concptr *quark__str;

/*
 * The array of window pointers
 */
term *angband_term[8];

/*
 * Keymaps for each "mode" associated with each keypress.
 */
concptr keymap_act[KEYMAP_MODES][256];



/*** Player information ***/

/*
 * Static player info record
 */
player_type p_body;

/*
 * Pointer to the player info
 */
player_type *p_ptr = &p_body;

bool can_save = FALSE;        /* Game can be saved */


MONSTER_IDX pet_t_m_idx;
MONSTER_IDX riding_t_m_idx;

MONSTER_IDX today_mon;

u32b start_time;

bool sukekaku;
bool new_mane;

/*
 * Which dungeon ?
 */
DEPTH *max_dlv;

COMMAND_CODE now_message;
bool use_menu;

#ifdef CHUUKEI
bool chuukei_server;
bool chuukei_client;
char *server_name;
int server_port;
#endif

/* for movie */
bool browsing_movie;


/* for snipers */
bool reset_concent = FALSE;   /* Concentration reset flag */


