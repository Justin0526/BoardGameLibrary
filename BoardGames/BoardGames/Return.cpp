/*********************************************************************************
 * Group         : T01
 * Team Member   : Khaleel Anis (S10270243)
 *
 * File Purpose:
 * - Implements the Return wrapper functions.
 *
 * Key Design Notes:
 * - This class contains no business logic.
 * - All checks (borrow ownership, availability, history updates)
 *   are performed inside User::returnGame().
 *
 * Constraints / Assumptions:
 * - Caller is responsible for handling user input and error messages.
 * - Return assumes the User instance represents the currently logged-in user.
 *********************************************************************************/
#include "Return.h"

bool Return::returnGame(
    User& user,
    List<Game>& games,
    HashTable<std::string, List<Game>::NodePtr>& gameTable,
    int gameId
) {
    // Delegate return logic directly to the User object.
    return user.returnGame(games, gameTable, gameId);
}
