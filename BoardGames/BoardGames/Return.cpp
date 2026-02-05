#include "Return.h"

bool Return::returnGame(
    User& user,
    List<Game>& games,
    HashTable<std::string, List<Game>::NodePtr>& gameTable,
    int gameId
) {
    return user.returnGame(games, gameTable, gameId);
}
