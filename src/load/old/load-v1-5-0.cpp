/*!
 * @brief 変愚蛮怒 v1.5.0以前の旧いセーブデータを読み込む処理
 * @date 2020/07/04
 * @author Hourier
 * @details 互換性を最大限に確保するため、基本的に関数分割は行わないものとする.
 */

#include "load/old/load-v1-5-0.h"
#include "artifact/fixed-art-types.h"
#include "floor/floor-object.h"
#include "game-option/birth-options.h"
#include "grid/grid.h"
#include "load/angband-version-comparer.h"
#include "load/item/item-loader-factory.h"
#include "load/item/item-loader-version-types.h"
#include "load/load-util.h"
#include "load/monster/monster-loader-factory.h"
#include "load/old-feature-types.h"
#include "load/old/item-loader-savefile50.h"
#include "load/old/monster-loader-savefile50.h"
#include "mind/mind-weaponsmith.h"
#include "monster-floor/monster-move.h"
#include "monster-race/race-flags-resistance.h"
#include "monster/monster-flag-types.h"
#include "monster/monster-info.h"
#include "monster/monster-list.h"
#include "object-enchant/object-ego.h"
#include "object-enchant/old-ego-extra-values.h"
#include "object-enchant/tr-types.h"
#include "object-enchant/trc-types.h"
#include "object-enchant/trg-types.h"
#include "sv-definition/sv-armor-types.h"
#include "sv-definition/sv-lite-types.h"
#include "system/angband-exceptions.h"
#include "system/artifact-type-definition.h"
#include "system/baseitem/baseitem-definition.h"
#include "system/baseitem/baseitem-list.h"
#include "system/enums/dungeon/dungeon-id.h"
#include "system/enums/monrace/monrace-id.h"
#include "system/enums/terrain/terrain-tag.h"
#include "system/floor/floor-info.h"
#include "system/grid-type-definition.h"
#include "system/item-entity.h"
#include "system/monrace/monrace-definition.h"
#include "system/monster-entity.h"
#include "system/player-type-definition.h"
#include "system/terrain/terrain-definition.h"
#include "system/terrain/terrain-list.h"
#include "util/bit-flags-calculator.h"
#include "util/enum-converter.h"
#include "world/world.h"

/* Old hidden trap flag */
static const BIT_FLAGS CAVE_TRAP = 0x8000;

const int OLD_QUEST_WATER_CAVE = 18; // 湖の洞窟.
const int QUEST_OLD_CASTLE = 27; // 古い城.
const int QUEST_ROYAL_CRYPT = 28; // 王家の墓.

/*!
 * @brief アイテムオブジェクト1件を読み込む / Read an object
 * @param o_ptr アイテムオブジェクト読み取り先ポインタ
 */
void rd_item_old(ItemEntity *o_ptr)
{
    o_ptr->bi_id = rd_s16b();

    o_ptr->iy = rd_byte();
    o_ptr->ix = rd_byte();

    const auto tval = i2enum<ItemKindType>(rd_byte());
    const auto sval = rd_byte();
    o_ptr->bi_key = BaseitemKey(tval, sval);

    if (h_older_than(0, 4, 4)) {
        if (tval == i2enum<ItemKindType>(100)) {
            o_ptr->bi_key = BaseitemKey(ItemKindType::GOLD, sval);
        }
        if (tval == i2enum<ItemKindType>(98)) {
            o_ptr->bi_key = BaseitemKey(ItemKindType::MUSIC_BOOK, sval);
        }
        if (tval == i2enum<ItemKindType>(110)) {
            o_ptr->bi_key = BaseitemKey(ItemKindType::HISSATSU_BOOK, sval);
        }
    }

    o_ptr->pval = rd_s16b();
    o_ptr->discount = rd_byte();
    o_ptr->number = rd_byte();

    o_ptr->weight = rd_s16b();

    o_ptr->fa_id = i2enum<FixedArtifactId>(rd_byte());

    o_ptr->ego_idx = i2enum<EgoType>(rd_byte());

    o_ptr->timeout = rd_s16b();
    o_ptr->to_h = rd_s16b();
    o_ptr->to_d = rd_s16b();

    o_ptr->to_a = rd_s16b();
    o_ptr->ac = rd_s16b();

    o_ptr->damage_dice.num = rd_byte();
    o_ptr->damage_dice.sides = rd_byte();

    o_ptr->ident = rd_byte();
    rd_FlagGroup_bytes(o_ptr->marked, rd_byte, 1);

    for (int i = 0, count = (h_older_than(1, 3, 0, 0) ? 3 : 4); i < count; i++) {
        auto tmp32u = rd_u32b();
        migrate_bitflag_to_flaggroup(o_ptr->art_flags, tmp32u, i * 32);
    }

    if (h_older_than(1, 3, 0, 0)) {
        if (o_ptr->ego_idx == EgoType::TELEPATHY) {
            o_ptr->art_flags.set(TR_TELEPATHY);
        }
    }

    if (h_older_than(1, 0, 11)) {
        // バージョン 1.0.11 以前は tr_type の 93, 94, 95 は現在と違い呪い等の別の用途で使用されていたので番号をハードコーディングする
        o_ptr->curse_flags.clear();
        if (o_ptr->ident & 0x40) {
            o_ptr->curse_flags.set(CurseTraitType::CURSED);
            if (o_ptr->art_flags.has(i2enum<tr_type>(94))) {
                o_ptr->curse_flags.set(CurseTraitType::HEAVY_CURSE);
            }
            if (o_ptr->art_flags.has(i2enum<tr_type>(95))) {
                o_ptr->curse_flags.set(CurseTraitType::PERMA_CURSE);
            }
            if (o_ptr->is_fixed_artifact()) {
                const auto &artifact = o_ptr->get_fixed_artifact();
                if (artifact.gen_flags.has(ItemGenerationTraitType::HEAVY_CURSE)) {
                    o_ptr->curse_flags.set(CurseTraitType::HEAVY_CURSE);
                }
                if (artifact.gen_flags.has(ItemGenerationTraitType::PERMA_CURSE)) {
                    o_ptr->curse_flags.set(CurseTraitType::PERMA_CURSE);
                }
            } else if (o_ptr->is_ego()) {
                const auto &ego = o_ptr->get_ego();
                if (ego.gen_flags.has(ItemGenerationTraitType::HEAVY_CURSE)) {
                    o_ptr->curse_flags.set(CurseTraitType::HEAVY_CURSE);
                }
                if (ego.gen_flags.has(ItemGenerationTraitType::PERMA_CURSE)) {
                    o_ptr->curse_flags.set(CurseTraitType::PERMA_CURSE);
                }
            }
        }
        o_ptr->art_flags.reset({ i2enum<tr_type>(93), i2enum<tr_type>(94), i2enum<tr_type>(95) });
    } else {
        auto tmp32u = rd_u32b();
        migrate_bitflag_to_flaggroup(o_ptr->curse_flags, tmp32u);
    }

    o_ptr->held_m_idx = rd_s16b();
    auto xtra1 = rd_byte(); // かつてエゴアイテムの情報を格納していた名残.
    o_ptr->activation_id = i2enum<RandomArtActType>(rd_byte());

    if (h_older_than(1, 0, 10)) {
        if (xtra1 == enum2i<OldEgoType>(OldEgoType::XTRA_SUSTAIN)) {
            switch (enum2i(o_ptr->activation_id) % 6) {
            case 0:
                o_ptr->art_flags.set(TR_SUST_STR);
                break;
            case 1:
                o_ptr->art_flags.set(TR_SUST_INT);
                break;
            case 2:
                o_ptr->art_flags.set(TR_SUST_WIS);
                break;
            case 3:
                o_ptr->art_flags.set(TR_SUST_DEX);
                break;
            case 4:
                o_ptr->art_flags.set(TR_SUST_CON);
                break;
            case 5:
                o_ptr->art_flags.set(TR_SUST_CHR);
                break;
            }
            o_ptr->activation_id = i2enum<RandomArtActType>(0);
        } else if (xtra1 == enum2i<OldEgoType>(OldEgoType::XTRA_POWER)) {
            switch (enum2i(o_ptr->activation_id) % 11) {
            case 0:
                o_ptr->art_flags.set(TR_RES_BLIND);
                break;
            case 1:
                o_ptr->art_flags.set(TR_RES_CONF);
                break;
            case 2:
                o_ptr->art_flags.set(TR_RES_SOUND);
                break;
            case 3:
                o_ptr->art_flags.set(TR_RES_SHARDS);
                break;
            case 4:
                o_ptr->art_flags.set(TR_RES_NETHER);
                break;
            case 5:
                o_ptr->art_flags.set(TR_RES_NEXUS);
                break;
            case 6:
                o_ptr->art_flags.set(TR_RES_CHAOS);
                break;
            case 7:
                o_ptr->art_flags.set(TR_RES_DISEN);
                break;
            case 8:
                o_ptr->art_flags.set(TR_RES_POIS);
                break;
            case 9:
                o_ptr->art_flags.set(TR_RES_DARK);
                break;
            case 10:
                o_ptr->art_flags.set(TR_RES_LITE);
                break;
            }
            o_ptr->activation_id = i2enum<RandomArtActType>(0);
        } else if (xtra1 == enum2i<OldEgoType>(OldEgoType::XTRA_ABILITY)) {
            switch (enum2i(o_ptr->activation_id) % 8) {
            case 0:
                o_ptr->art_flags.set(TR_LEVITATION);
                break;
            case 1:
                o_ptr->art_flags.set(TR_LITE_1);
                break;
            case 2:
                o_ptr->art_flags.set(TR_SEE_INVIS);
                break;
            case 3:
                o_ptr->art_flags.set(TR_WARNING);
                break;
            case 4:
                o_ptr->art_flags.set(TR_SLOW_DIGEST);
                break;
            case 5:
                o_ptr->art_flags.set(TR_REGEN);
                break;
            case 6:
                o_ptr->art_flags.set(TR_FREE_ACT);
                break;
            case 7:
                o_ptr->art_flags.set(TR_HOLD_EXP);
                break;
            }
            o_ptr->activation_id = i2enum<RandomArtActType>(0);
        }

        xtra1 = 0;
    }

    if (h_older_than(0, 2, 3)) {
        o_ptr->fuel = 0;
        o_ptr->captured_monster_current_hp = 0;
        o_ptr->smith_hit = 0;
        o_ptr->smith_damage = 0;
        o_ptr->captured_monster_max_hp = 0;
        if (tval == ItemKindType::CHEST) {
            o_ptr->chest_level = xtra1;
        } else if (tval == ItemKindType::CAPTURE) {
            o_ptr->captured_monster_speed = xtra1;
        }

        if (tval == ItemKindType::CAPTURE) {
            const auto &monrace = o_ptr->get_monrace();
            if (monrace.misc_flags.has(MonsterMiscType::FORCE_MAXHP)) {
                o_ptr->captured_monster_max_hp = static_cast<short>(monrace.hit_dice.maxroll());
            } else {
                o_ptr->captured_monster_max_hp = static_cast<short>(monrace.hit_dice.roll());
            }
            if (ironman_nightmare) {
                o_ptr->captured_monster_max_hp = std::min<short>(MONSTER_MAXHP, o_ptr->captured_monster_max_hp * 2L);
            }

            o_ptr->captured_monster_current_hp = o_ptr->captured_monster_max_hp;
        }
    } else {
        auto xtra3 = rd_byte();
        if (h_older_than(1, 3, 0, 1)) {
            if (o_ptr->is_smith() && (xtra3 >= 1 + 96)) {
                auto mes = _("古いバージョンで鍛冶師をプレイしたデータは読み込めません。", "The save data from playing a weaponsmith on versions older than v3.0.0 Aplha38 can't be read.");
                throw SaveDataNotSupportedException(mes);
            }
        }

        auto xtra4 = rd_s16b();
        if (tval == ItemKindType::LITE) {
            o_ptr->fuel = xtra4;
        } else if (tval == ItemKindType::CAPTURE) {
            o_ptr->captured_monster_current_hp = xtra4;
        } else {
            o_ptr->smith_hit = static_cast<byte>(xtra4 >> 8);
            o_ptr->smith_damage = static_cast<byte>(xtra4 & 0x000f);
        }

        o_ptr->captured_monster_max_hp = rd_s16b();
    }

    if (h_older_than(1, 0, 5) && o_ptr->is_fuel()) {
        o_ptr->fuel = o_ptr->pval;
        o_ptr->pval = 0;
    }

    o_ptr->feeling = rd_byte();

    if (auto insc = rd_string();
        !insc.empty()) {
        o_ptr->inscription = std::move(insc);
    }

    /*!< @todo 元々このif文には末尾に";"が付いていた、バグかもしれない */
    if (auto name = rd_string();
        !name.empty()) {
        o_ptr->randart_name = std::move(name);
    }
    {
        auto tmp32s = rd_s32b();
        strip_bytes(tmp32s);
    }

    if ((o_ptr->bi_id >= 445) && (o_ptr->bi_id <= 479)) {
        return;
    }

    if (h_older_than(0, 4, 10) && (o_ptr->ego_idx == EgoType::TWILIGHT)) {
        o_ptr->bi_id = BaseitemList::get_instance().lookup_baseitem_id({ ItemKindType::SOFT_ARMOR, SV_TWILIGHT_ROBE });
    }

    if (h_older_than(0, 4, 9)) {
        if (o_ptr->art_flags.has(TR_MAGIC_MASTERY)) {
            o_ptr->art_flags.reset(TR_MAGIC_MASTERY);
            o_ptr->art_flags.set(TR_DEC_MANA);
        }
    }

    if (o_ptr->is_ego()) {
        const auto &ego = o_ptr->get_ego();
        if (ego.name.empty()) {
            o_ptr->ego_idx = EgoType::NONE;
        }
    }
}

/*!
 * @brief モンスターを読み込む / Read a monster
 * @param player_ptr プレイヤーへの参照ポインタ
 * @param m_ptr モンスター保存先ポインタ
 */
void rd_monster_old(PlayerType *player_ptr, MonsterEntity &monster)
{
    monster.r_idx = i2enum<MonraceId>(rd_s16b());

    if (h_older_than(1, 0, 12)) {
        monster.ap_r_idx = monster.r_idx;
    } else {
        monster.ap_r_idx = i2enum<MonraceId>(rd_s16b());
    }

    if (h_older_than(1, 0, 14)) {
        const auto &monrace = monster.get_monrace();

        monster.sub_align = SUB_ALIGN_NEUTRAL;
        if (monrace.kind_flags.has(MonsterKindType::EVIL)) {
            monster.sub_align |= SUB_ALIGN_EVIL;
        }
        if (monrace.kind_flags.has(MonsterKindType::GOOD)) {
            monster.sub_align |= SUB_ALIGN_GOOD;
        }
    } else {
        monster.sub_align = rd_byte();
    }

    monster.fy = rd_byte();
    monster.fx = rd_byte();
    monster.current_floor_ptr = player_ptr->current_floor_ptr;

    monster.hp = rd_s16b();
    monster.maxhp = rd_s16b();

    if (h_older_than(1, 0, 5)) {
        monster.max_maxhp = monster.maxhp;
    } else {
        monster.max_maxhp = rd_s16b();
    }
    if (h_older_than(2, 1, 2, 1)) {
        monster.dealt_damage = 0;
    } else {
        monster.dealt_damage = rd_s32b();
    }

    monster.mtimed[MonsterTimedEffect::SLEEP] = rd_s16b();
    monster.mspeed = rd_byte();

    if (h_older_than(0, 4, 2)) {
        monster.energy_need = rd_byte();
    } else {
        monster.energy_need = rd_s16b();
    }

    if (h_older_than(1, 0, 13)) {
        monster.energy_need = 100 - monster.energy_need;
    }

    if (h_older_than(0, 0, 7)) {
        monster.mtimed[MonsterTimedEffect::FAST] = 0;
        monster.mtimed[MonsterTimedEffect::SLOW] = 0;
    } else {
        monster.mtimed[MonsterTimedEffect::FAST] = rd_byte();
        monster.mtimed[MonsterTimedEffect::SLOW] = rd_byte();
    }

    monster.mtimed[MonsterTimedEffect::STUN] = rd_byte();
    monster.mtimed[MonsterTimedEffect::CONFUSION] = rd_byte();
    monster.mtimed[MonsterTimedEffect::FEAR] = rd_byte();

    if (h_older_than(0, 0, 10)) {
        monster.reset_target();
    } else if (h_older_than(0, 0, 11)) {
        strip_bytes(2);
        monster.reset_target();
    } else {
        monster.target_y = rd_s16b();
        monster.target_x = rd_s16b();
    }

    monster.mtimed[MonsterTimedEffect::INVULNERABILITY] = rd_byte();

    auto tmp32u = rd_u32b();
    migrate_bitflag_to_flaggroup(monster.smart, tmp32u);

    // 3.0.0Alpha10以前のSM_CLONED(ビット位置22)、SM_PET(23)、SM_FRIEDLY(28)をMFLAG2に移行する
    // ビット位置の定義はなくなるので、ビット位置の値をハードコードする。
    std::bitset<32> rd_bits_smart(tmp32u);
    monster.mflag2[MonsterConstantFlagType::CLONED] = rd_bits_smart[22];
    monster.mflag2[MonsterConstantFlagType::PET] = rd_bits_smart[23];
    monster.mflag2[MonsterConstantFlagType::FRIENDLY] = rd_bits_smart[28];
    monster.smart.reset(i2enum<MonsterSmartLearnType>(22)).reset(i2enum<MonsterSmartLearnType>(23)).reset(i2enum<MonsterSmartLearnType>(28));

    if (h_older_than(0, 4, 5)) {
        monster.exp = 0;
    } else {
        monster.exp = rd_u32b();
    }

    if (h_older_than(0, 2, 2)) {
        if (enum2i(monster.r_idx) < 0) {
            monster.r_idx = i2enum<MonraceId>(0 - enum2i(monster.r_idx));
            monster.mflag2.set(MonsterConstantFlagType::KAGE);
        }
    } else {
        auto tmp8u = rd_byte();
        constexpr auto base = enum2i(MonsterConstantFlagType::KAGE);
        migrate_bitflag_to_flaggroup(monster.mflag2, tmp8u, base, 7);
    }

    if (h_older_than(1, 0, 12)) {
        if (monster.mflag2.has(MonsterConstantFlagType::KAGE)) {
            monster.ap_r_idx = MonraceId::KAGE;
        }
    }

    if (h_older_than(0, 1, 3)) {
        monster.nickname.clear();
    } else {
        auto nickname = rd_string();
        if (!nickname.empty()) {
            monster.nickname = std::move(nickname);
        }
    }

    strip_bytes(1);
}

static void move_RF3_to_RFR(MonraceDefinition &monrace, BIT_FLAGS f3, const BIT_FLAGS rf3, const MonsterResistanceType rfr)
{
    if (f3 & rf3) {
        monrace.resistance_flags.set(rfr);
    }
}

static void move_RF4_BR_to_RFR(MonraceDefinition &monrace, BIT_FLAGS f4, const BIT_FLAGS rf4_br, const MonsterResistanceType rfr)
{
    if (f4 & rf4_br) {
        monrace.resistance_flags.set(rfr);
    }
}

/*!
 * @brief モンスターの思い出を読み込む
 * @param r_ptr モンスター種族情報への参照ポインタ
 * @param r_idx モンスター種族ID
 * @details 本来はr_idxからr_ptrを決定可能だが、互換性を優先するため元コードのままとする
 */
void set_old_lore(MonraceDefinition &monrace, BIT_FLAGS f3, BIT_FLAGS f4, const MonraceId r_idx)
{
    monrace.r_resistance_flags.clear();
    move_RF3_to_RFR(monrace, f3, RF3_IM_ACID, MonsterResistanceType::IMMUNE_ACID);
    move_RF3_to_RFR(monrace, f3, RF3_IM_ELEC, MonsterResistanceType::IMMUNE_ELEC);
    move_RF3_to_RFR(monrace, f3, RF3_IM_FIRE, MonsterResistanceType::IMMUNE_FIRE);
    move_RF3_to_RFR(monrace, f3, RF3_IM_COLD, MonsterResistanceType::IMMUNE_COLD);
    move_RF3_to_RFR(monrace, f3, RF3_IM_POIS, MonsterResistanceType::IMMUNE_POISON);
    move_RF3_to_RFR(monrace, f3, RF3_RES_TELE, MonsterResistanceType::RESIST_TELEPORT);
    move_RF3_to_RFR(monrace, f3, RF3_RES_NETH, MonsterResistanceType::RESIST_NETHER);
    move_RF3_to_RFR(monrace, f3, RF3_RES_WATE, MonsterResistanceType::RESIST_WATER);
    move_RF3_to_RFR(monrace, f3, RF3_RES_PLAS, MonsterResistanceType::RESIST_PLASMA);
    move_RF3_to_RFR(monrace, f3, RF3_RES_NEXU, MonsterResistanceType::RESIST_NEXUS);
    move_RF3_to_RFR(monrace, f3, RF3_RES_DISE, MonsterResistanceType::RESIST_DISENCHANT);
    move_RF3_to_RFR(monrace, f3, RF3_RES_ALL, MonsterResistanceType::RESIST_ALL);

    move_RF4_BR_to_RFR(monrace, f4, RF4_BR_LITE, MonsterResistanceType::RESIST_LITE);
    move_RF4_BR_to_RFR(monrace, f4, RF4_BR_DARK, MonsterResistanceType::RESIST_DARK);
    move_RF4_BR_to_RFR(monrace, f4, RF4_BR_SOUN, MonsterResistanceType::RESIST_SOUND);
    move_RF4_BR_to_RFR(monrace, f4, RF4_BR_CHAO, MonsterResistanceType::RESIST_CHAOS);
    move_RF4_BR_to_RFR(monrace, f4, RF4_BR_TIME, MonsterResistanceType::RESIST_TIME);
    move_RF4_BR_to_RFR(monrace, f4, RF4_BR_INER, MonsterResistanceType::RESIST_INERTIA);
    move_RF4_BR_to_RFR(monrace, f4, RF4_BR_GRAV, MonsterResistanceType::RESIST_GRAVITY);
    move_RF4_BR_to_RFR(monrace, f4, RF4_BR_SHAR, MonsterResistanceType::RESIST_SHARDS);
    move_RF4_BR_to_RFR(monrace, f4, RF4_BR_WALL, MonsterResistanceType::RESIST_FORCE);

    if (f4 & RF4_BR_CONF) {
        monrace.r_resistance_flags.set(MonsterResistanceType::NO_CONF);
    }

    if (r_idx == MonraceId::STORMBRINGER) {
        monrace.r_resistance_flags.set(MonsterResistanceType::RESIST_CHAOS);
    }

    if (monrace.r_kind_flags.has(MonsterKindType::ORC)) {
        monrace.r_resistance_flags.set(MonsterResistanceType::RESIST_DARK);
    }
}

/*!
 * @brief ダンジョン情報を読み込む / Read the dungeon (old method)
 * @param player_ptr プレイヤーへの参照ポインタ
 * @details
 * The monsters/objects must be loaded in the same order
 * that they were stored, since the actual indexes matter.
 */
errr rd_dungeon_old(PlayerType *player_ptr)
{
    auto &floor = *player_ptr->current_floor_ptr;
    floor.dun_level = rd_s16b();
    if (h_older_than(0, 3, 8)) {
        floor.set_dungeon_index(DungeonId::ANGBAND);
    } else {
        floor.set_dungeon_index(i2enum<DungeonId>(rd_byte()));
    }

    floor.base_level = floor.dun_level;
    floor.base_level = rd_s16b();

    floor.num_repro = rd_s16b();
    player_ptr->y = rd_s16b();
    player_ptr->x = rd_s16b();
    if (h_older_than(0, 3, 13) && !floor.is_underground() && !floor.inside_arena) {
        player_ptr->y = 33;
        player_ptr->x = 131;
    }
    floor.height = rd_s16b();
    floor.width = rd_s16b();
    strip_bytes(2); /* max_panel_rows */
    strip_bytes(2); /* max_panel_cols */

    int ymax = floor.height;
    int xmax = floor.width;

    for (int x = 0, y = 0; y < ymax;) {
        uint16_t info;
        auto count = rd_byte();
        if (h_older_than(0, 3, 6)) {
            info = rd_byte();
        } else {
            info = rd_u16b();
            info &= ~(CAVE_LITE | CAVE_VIEW | CAVE_MNLT | CAVE_MNDK);
        }

        for (int i = count; i > 0; i--) {
            auto &grid = floor.get_grid({ y, x });
            grid.info = info;
            if (++x >= xmax) {
                x = 0;
                if (++y >= ymax) {
                    break;
                }
            }
        }
    }

    for (int x = 0, y = 0; y < ymax;) {
        auto count = rd_byte();
        auto tmp8u = rd_byte();
        for (int i = count; i > 0; i--) {
            auto &grid = floor.get_grid({ y, x });
            grid.feat = (int16_t)tmp8u;
            if (++x >= xmax) {
                x = 0;
                if (++y >= ymax) {
                    break;
                }
            }
        }
    }

    for (int x = 0, y = 0; y < ymax;) {
        auto count = rd_byte();
        auto tmp8u = rd_byte();
        for (int i = count; i > 0; i--) {
            auto &grid = floor.get_grid({ y, x });
            grid.mimic = (int16_t)tmp8u;
            if (++x >= xmax) {
                x = 0;
                if (++y >= ymax) {
                    break;
                }
            }
        }
    }

    for (int x = 0, y = 0; y < ymax;) {
        auto count = rd_byte();
        auto tmp16s = rd_s16b();
        for (int i = count; i > 0; i--) {
            auto &grid = floor.get_grid({ y, x });
            grid.special = tmp16s;
            if (++x >= xmax) {
                x = 0;
                if (++y >= ymax) {
                    break;
                }
            }
        }
    }

    if (h_older_than(1, 0, 99)) {
        for (int y = 0; y < ymax; y++) {
            for (int x = 0; x < xmax; x++) {
                floor.get_grid({ y, x }).info &= ~(CAVE_MASK);
            }
        }
    }

    if (h_older_than(1, 1, 1, 0)) {
        for (int y = 0; y < ymax; y++) {
            for (int x = 0; x < xmax; x++) {
                auto &grid = floor.get_grid({ y, x });

                /* Very old */
                if (grid.feat == OLD_FEAT_INVIS) {
                    grid.set_terrain_id(TerrainTag::FLOOR);
                    grid.info |= CAVE_TRAP;
                }

                /* Older than 1.1.1 */
                if (grid.feat == OLD_FEAT_MIRROR) {
                    grid.set_terrain_id(TerrainTag::FLOOR);
                    grid.info |= CAVE_OBJECT;
                }
            }
        }
    }

    if (h_older_than(1, 3, 1, 0)) {
        for (int y = 0; y < ymax; y++) {
            for (int x = 0; x < xmax; x++) {
                auto &grid = floor.get_grid({ y, x });

                /* Old CAVE_IN_MIRROR flag */
                if (grid.info & CAVE_OBJECT) {
                    grid.set_terrain_id(TerrainTag::MIRROR, TerrainKind::MIMIC);
                } else if ((grid.feat == OLD_FEAT_RUNE_EXPLOSION) || (grid.feat == OLD_FEAT_RUNE_PROTECTION)) {
                    grid.info |= CAVE_OBJECT;
                    grid.mimic = grid.feat;
                    grid.set_terrain_id(TerrainTag::FLOOR);
                } else if (grid.info & CAVE_TRAP) {
                    grid.info &= ~CAVE_TRAP;
                    grid.set_terrain_id(TerrainTag::FLOOR, TerrainKind::MIMIC);
                    grid.set_terrain_id(floor.select_random_trap());
                } else if (grid.feat == OLD_FEAT_INVIS) {
                    grid.mimic = grid.feat;
                    grid.set_terrain_id(TerrainTag::TRAP_OPEN);
                }
            }
        }
    }

    /* Quest 18 was removed */
    if (!vanilla_town) {
        for (int y = 0; y < ymax; y++) {
            for (int x = 0; x < xmax; x++) {
                auto &grid = floor.get_grid({ y, x });
                if ((grid.special == OLD_QUEST_WATER_CAVE) && !floor.is_underground()) {
                    if (grid.feat == OLD_FEAT_QUEST_ENTER) {
                        grid.set_terrain_id(TerrainTag::TREE);
                        grid.special = 0;
                    } else if (grid.feat == OLD_FEAT_BLDG_1) {
                        grid.special = lite_town ? QUEST_OLD_CASTLE : QUEST_ROYAL_CRYPT;
                    }
                } else if ((grid.feat == OLD_FEAT_QUEST_EXIT) && (floor.quest_number == i2enum<QuestId>(OLD_QUEST_WATER_CAVE))) {
                    grid.set_terrain_id(TerrainTag::UP_STAIR);
                    grid.special = 0;
                }
            }
        }
    }

    uint16_t limit = rd_u16b();
    if (limit > MAX_FLOOR_ITEMS) {
        load_note(format(_("アイテムの配列が大きすぎる(%d)！", "Too many (%d) object entries!"), limit));
        return 151;
    }

    auto item_loader = ItemLoaderFactory::create_loader();
    for (int i = 1; i < limit; i++) {
        const auto item_idx = floor.pop_empty_index_item();
        if (i != item_idx) {
            load_note(format(_("アイテム配置エラー (%d <> %d)", "Object allocation error (%d <> %d)"), i, item_idx));
            return 152;
        }

        auto &item = *floor.o_list[item_idx];
        item_loader->rd_item(&item);
        auto &list = get_o_idx_list_contains(floor, item_idx);
        list.add(floor, item_idx);
    }

    limit = rd_u16b();
    if (limit > MAX_FLOOR_MONSTERS) {
        load_note(format(_("モンスターの配列が大きすぎる(%d)！", "Too many (%d) monster entries!"), limit));
        return 161;
    }

    auto monster_loader = MonsterLoaderFactory::create_loader(player_ptr);
    for (int i = 1; i < limit; i++) {
        const auto m_idx = floor.pop_empty_index_monster();
        if (i != m_idx) {
            load_note(format(_("モンスター配置エラー (%d <> %d)", "Monster allocation error (%d <> %d)"), i, m_idx));
            return 162;
        }

        auto &monster = floor.m_list[m_idx];
        monster_loader->rd_monster(monster);
        auto &grid = floor.get_grid(monster.get_position());
        grid.m_idx = m_idx;
        monster.get_real_monrace().increment_current_numbers();
    }

    auto &world = AngbandWorld::get_instance();
    if (h_older_than(0, 3, 13) && !floor.is_underground() && !floor.inside_arena) {
        world.character_dungeon = false;
    } else {
        world.character_dungeon = true;
    }

    return 0;
}
