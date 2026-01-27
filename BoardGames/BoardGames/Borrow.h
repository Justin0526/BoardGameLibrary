#pragma once
#include "User.h"
#include "Game.h"
#include "LinkedList.h"
#include <string>

class Borrow {
public:
    // wrapper that allows borrowing for any User type
    static bool borrowGame(User& user, List<Game>& games, const std::string& gameName);
};

