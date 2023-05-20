﻿#pragma once

#include "effect/attribute-types.h"
#include "monster-floor/monster-death-util.h"
#include "system/angband.h"

enum class FixedArtifactId : short;
class PlayerType;
void monster_death(PlayerType *player_ptr, MONSTER_IDX m_idx, bool drop_item, AttributeFlags attribute_flags);
void monster_death(PlayerType *player_ptr, MONSTER_IDX m_idx, bool drop_item, AttributeType type);
bool drop_single_artifact(PlayerType *player_ptr, monster_death_type *md_ptr, FixedArtifactId a_idx);
