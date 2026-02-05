#include "Borrow.h"

bool Borrow::borrowGame(
    User& user,
    List<Game>& games,
    HashTable<std::string, List<Game>::NodePtr>& gameTable,
    const std::string& gameName
) {
    return user.borrowGame(games, gameTable, gameName);
}
