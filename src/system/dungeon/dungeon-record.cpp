/*!
 * @brief ダンジョンに関するプレイ記録実装
 * @author Hourier
 * @date 2024/12/01
 */

#include "system/dungeon/dungeon-record.h"
#include "locale/language-switcher.h"
#include "system/angband-exceptions.h"
#include "system/enums/dungeon/dungeon-id.h"
#include "term/z-rand.h"
#include "util/enum-converter.h"
#include "util/enum-range.h"

bool DungeonRecord::has_entered() const
{
    return this->max_level.has_value();
}

int DungeonRecord::get_max_level() const
{
    return this->max_level.value_or(0);
}

int DungeonRecord::get_max_max_level() const
{
    return this->max_max_level.value_or(0);
}

void DungeonRecord::set_max_level(int level)
{
    if (!this->max_max_level || (level > this->max_max_level)) {
        this->max_max_level = level;
    }

    this->max_level = level;
}

void DungeonRecord::reset()
{
    this->max_level = std::nullopt;
    this->max_max_level = std::nullopt;
}

DungeonRecords DungeonRecords::instance{};

DungeonRecords::DungeonRecords()
{
    for (auto dungeon_id : DUNGEON_IDS) {
        this->records.emplace(dungeon_id, std::make_shared<DungeonRecord>());
    }
}

DungeonRecords &DungeonRecords::get_instance()
{
    return instance;
}

DungeonRecord &DungeonRecords::get_record(DungeonId dungeon_id)
{
    return *this->records.at(dungeon_id);
}

const DungeonRecord &DungeonRecords::get_record(DungeonId dungeon_id) const
{
    return *this->records.at(dungeon_id);
}

std::shared_ptr<DungeonRecord> DungeonRecords::get_record_shared(DungeonId dungeon_id)
{
    return this->records.at(dungeon_id);
}

std::shared_ptr<const DungeonRecord> DungeonRecords::get_record_shared(DungeonId dungeon_id) const
{
    return this->records.at(dungeon_id);
}

std::map<DungeonId, std::shared_ptr<DungeonRecord>>::iterator DungeonRecords::begin()
{
    return this->records.begin();
}

std::map<DungeonId, std::shared_ptr<DungeonRecord>>::const_iterator DungeonRecords::begin() const
{
    return this->records.cbegin();
}

std::map<DungeonId, std::shared_ptr<DungeonRecord>>::iterator DungeonRecords::end()
{
    return this->records.end();
}

std::map<DungeonId, std::shared_ptr<DungeonRecord>>::const_iterator DungeonRecords::end() const
{
    return this->records.cend();
}

std::map<DungeonId, std::shared_ptr<DungeonRecord>>::reverse_iterator DungeonRecords::rbegin()
{
    return this->records.rbegin();
}

std::map<DungeonId, std::shared_ptr<DungeonRecord>>::const_reverse_iterator DungeonRecords::rbegin() const
{
    return this->records.crbegin();
}

std::map<DungeonId, std::shared_ptr<DungeonRecord>>::reverse_iterator DungeonRecords::rend()
{
    return this->records.rend();
}

std::map<DungeonId, std::shared_ptr<DungeonRecord>>::const_reverse_iterator DungeonRecords::rend() const
{
    return this->records.crend();
}

size_t DungeonRecords::size() const
{
    return this->records.size();
}

bool DungeonRecords::empty() const
{
    return this->records.empty();
}

void DungeonRecords::reset_all()
{
    for (auto &[_, record] : this->records) {
        record->reset();
    }
}

std::vector<DungeonId> DungeonRecords::collect_entered_dungeon_ids() const
{
    std::vector<DungeonId> entered_dungeons;
    for (const auto &[dungeon_id, record] : this->records) {
        if (record->has_entered()) {
            entered_dungeons.push_back(dungeon_id);
        }
    }

    return entered_dungeons;
}
