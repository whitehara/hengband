#include "object/object-index-list.h"
#include "system/floor/floor-info.h"
#include "system/item-entity.h"

#include <algorithm>

void ObjectIndexList::add(FloorType &floor, OBJECT_IDX o_idx, IDX stack_idx)
{
    if (stack_idx <= 0) {
        stack_idx = o_idx_list_.empty() ? 1 : floor.o_list[o_idx_list_.front()]->stack_idx + 1;
    }

    auto it = std::partition_point(
        o_idx_list_.begin(), o_idx_list_.end(), [&floor, stack_idx](IDX idx) { return floor.o_list[idx]->stack_idx > stack_idx; });

    o_idx_list_.insert(it, o_idx);
    floor.o_list[o_idx]->stack_idx = stack_idx;
}

void ObjectIndexList::remove(OBJECT_IDX o_idx)
{
    o_idx_list_.remove(o_idx);
}

void ObjectIndexList::rotate(FloorType &floor)
{
    if (o_idx_list_.size() < 2) {
        return;
    }

    o_idx_list_.push_back(o_idx_list_.front());
    o_idx_list_.pop_front();

    for (const auto o_idx : o_idx_list_) {
        floor.o_list[o_idx]->stack_idx++;
    }

    floor.o_list[o_idx_list_.back()]->stack_idx = 1;
}
