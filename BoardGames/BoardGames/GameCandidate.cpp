#include "GameCandidate.h"

GameCandidate::GameCandidate() {}

GameCandidate::GameCandidate(const string& gameId, const string& gameName, int score, int support)
    :gameId(gameId),
     gameName(gameName),
     score(score),
     support(support)
{ }

const string& GameCandidate::getGameId() const {
    return this->gameId;
}

const string& GameCandidate::getGameName() const {
    return this->gameName;
}

int GameCandidate::getScore() const {
    return this->score;
}

int GameCandidate::getSupport() const {
    return this->support;
}

bool byNameAsc(const GameCandidate& a, const GameCandidate& b) {
    return a.getGameName() < b.getGameName();
}

bool byNameDesc(const GameCandidate& a, const GameCandidate& b) {
    return b.getGameName() < a.getGameName();
}

bool byScoreAsc(const GameCandidate& a, const GameCandidate& b) {
    return a.getScore() < b.getScore();
}

bool byScoreDesc(const GameCandidate& a, const GameCandidate& b) {
    return b.getScore() < a.getScore();
}

bool bySupportAsc(const GameCandidate& a, const GameCandidate& b) {
    return a.getSupport() < b.getSupport();
}

bool bySupportDesc(const GameCandidate& a, const GameCandidate& b) {
    return b.getSupport() < a.getSupport();
}

ostream& operator<<(ostream& os, const GameCandidate& c) {
    os << c.getGameId() << ", "
        << c.getScore() << ", "
        << c.getSupport() << "\n";

    return os;
}