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
#pragma once
#include<string>
#include<iostream>
using namespace std;

class GameCandidate {
    private:
        // ---- Candidate identification ----
        string gameId;  
        string gameName;

        // ---- Recommendation metrics ----
        int score;
        int support;

    public:
        GameCandidate();
        GameCandidate(const string& id, const string& gameName, int s, int sup);

        // ---- Accessors ----
        // All getters are const to prevent modification of candidate data.
        const string& getGameId() const;
        const string& getGameName() const;
        int getScore() const;
        int getSupport() const;

};

// Comparator functions for sorting GameCandidate objects.
// These functions are passed into List<GameCandidate>::sort().
// Asc  = ascending order
// Desc = descending order
bool byNameAsc(const GameCandidate& a, const GameCandidate& b);
bool byNameDesc(const GameCandidate& a, const GameCandidate& b);
bool byScoreAsc(const GameCandidate& a, const GameCandidate& b);
bool byScoreDesc(const GameCandidate& a, const GameCandidate& b);
bool bySupportAsc(const GameCandidate& a, const GameCandidate& b);
bool bySupportDesc(const GameCandidate& a, const GameCandidate& b);

// Stream output operator.
// Enables GameCandidate objects to be printed via List::print() and cout.
ostream& operator<<(std::ostream& os, const GameCandidate& c);