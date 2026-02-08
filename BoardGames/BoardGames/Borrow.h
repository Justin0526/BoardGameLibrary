/*********************************************************************************
 * Group         : T01
 * Team Member   : Khaleel Anis (S10270243)
 *
 * File Purpose:
 * - Declares the Borrow helper class.
 * - Provides a unified interface for borrowing games across different User types.
 *
 * Key Design Notes:
 * - Uses a static method to avoid unnecessary object instantiation.
 * - Promotes loose coupling between UI/controller code and User implementation.
 *
 * Constraints / Assumptions:
 * - Borrow does not store state.
 * - All borrowing rules and side effects are enforced by the User class.
 *********************************************************************************/
#pragma once
#include "User.h"
#include "Game.h"
#include "LinkedList.h"
#include <string>

class Borrow {
public:
    // wrapper that allows borrowing for any User type
    static bool borrowGame(
        User& user,
        List<Game>& games,
        HashTable<std::string, List<Game>::NodePtr>& gameTable,
        const std::string& gameName
    );
};