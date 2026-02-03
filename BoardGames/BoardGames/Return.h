#pragma once
#include "User.h"
#include "Game.h"
#include "LinkedList.h"

class Return {
public:
    // wrapper that allows returning for any User type
    static bool returnGame(User& user, List<Game>& games, int gameId);
};

