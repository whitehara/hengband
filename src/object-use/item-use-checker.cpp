#include "object-use/item-use-checker.h"
#include "system/player-type-definition.h"
#include "timed-effect/timed-effects.h"
#include "view/display-messages.h"

ItemUseChecker::ItemUseChecker(PlayerType *player_ptr)
    : player_ptr(player_ptr)
{
}

bool ItemUseChecker::check_stun(std::string_view mes) const
{
    const auto penalty = this->player_ptr->effects()->stun().get_item_chance_penalty();
    if (penalty >= randint1(100)) {
        msg_print(mes);
        return false;
    }

    return true;
}
