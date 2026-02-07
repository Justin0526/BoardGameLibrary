/*********************************************************************************
 * Group         : T01
 * Team Member   : Khaleel Anis (S10270243)
 *
 * File Purpose:
 * - Declares the Return utility class, which provides a wrapper interface
 *   for returning borrowed games.
 *
 * Key Design Notes:
 * - Return does not store or manage state.
 * - Delegates all return validation and ownership checks to User.
 * - Exists to keep menu / controller code decoupled from User internals.
 *
 * Constraints / Assumptions:
 * - gameId refers to a valid game indexed in gameTable.
 * - User::returnGame() enforces borrower ownership and availability rules.
 *********************************************************************************/
#pragma once
#include "User.h"
#include "Game.h"
#include "LinkedList.h"

class Return {
public:
    // Wrapper function that forwards a return request to the User object.
    // Centralises return handling while keeping business logic inside User.
    static bool returnGame(
        User& user,
        List<Game>& games,
        HashTable<std::string, List<Game>::NodePtr>& gameTable,
        int gameId
    );
};