/*********************************************************************************
 * Group         : T01
 * Team Member   : Khaleel Anis (S10270243)
 *
 * File Purpose:
 * - Implements the Borrow helper class.
 * - Acts as a thin wrapper that delegates borrowing logic to the User class.
 *
 * Key Design Notes:
 * - Centralises borrow-related calls to avoid exposing User internals directly
 *   to higher-level modules (e.g. BoardGames.cpp).
 * - Preserves single-responsibility: Borrow does not contain business rules,
 *   only forwards validated requests.
 *
 * Constraints / Assumptions:
 * - Actual borrow validation (availability, ownership, state updates) is handled
 *   entirely by User::borrowGame().
 * - gameTable must already be populated with valid Game node pointers.
 *********************************************************************************/
#include "Borrow.h"

bool Borrow::borrowGame(
    User& user,
    List<Game>& games,
    HashTable<std::string, List<Game>::NodePtr>& gameTable,
    const std::string& gameName
) {
    // Delegate borrowing logic to User
    return user.borrowGame(games, gameTable, gameName);
}