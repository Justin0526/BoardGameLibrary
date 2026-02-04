#include "GameCandidate.h"

GameCandidate::GameCandidate() {}

GameCandidate::GameCandidate(const string& gameId, int score, int support)
    :gameId(gameId),
     score(score),
     support(support)
{ }

const string& GameCandidate::getGameId() const {
    return this->gameId;
}

int GameCandidate::getScore() const {
    return this->score;
}

int GameCandidate::getSupport() const {
    return this->support;
}

ostream& operator<<(ostream& os, const GameCandidate& c) {
    os << c.getGameId() << ", "
        << c.getScore() << ", "
        << c.getSupport() << "\n";

    return os;
}