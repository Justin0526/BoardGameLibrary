/*********************************************************************************
 * Group         : T01
 * Team Member   : Justin Tang Jia Ze (S10269496B)
 *
 * File Purpose:
 * - Implements GameCandidate: a lightweight model used by the recommendation system
 *   to store and display candidate games with computed ranking metrics.
 *
 * Key Design Notes:
 * - Stores both identifiers (gameId, gameName) and ranking signals (score, support).
 * - Provides comparator functions (byName / byScore / bySupport) for List::sort().
 * - Overloads operator<< so List<GameCandidate>::print() can display candidates.
 *
 * Constraints / Assumptions:
 * - gameId is treated as a stable unique identifier.
 * - score/support are computed externally by the recommendation logic.
 *********************************************************************************/
#include "GameCandidate.h"

// Basic constructor
GameCandidate::GameCandidate() {}

// Parameterised constructor
GameCandidate::GameCandidate(const string& gameId, const string& gameName, int score, int support)
    :gameId(gameId),
     gameName(gameName),
     score(score),
     support(support)
{ }

// Accessors
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

// Comparator functions for sorting GameCandidate lists.
// These are passed into List<GameCandidate>::sort(cmp).
// Convention: Asc = increasing order, Desc = decreasing order.
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

// Stream output operator.
// Enables List<GameCandidate>::print() / printWithIndex() to display candidates
// consistently in the console (ID, score, support).
ostream& operator<<(ostream& os, const GameCandidate& c) {
    os << c.getGameId() << ", "
        << c.getScore() << ", "
        << c.getSupport() << "\n";

    return os;
}