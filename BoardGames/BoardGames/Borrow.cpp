#include "Borrow.h"

bool Borrow::borrowGame(User& user, List<Game>& games, const std::string& gameName) {
    return user.borrowGame(games, gameName);
}
