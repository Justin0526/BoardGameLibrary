#pragma once
#include<string>
#include<iostream>
using namespace std;

class GameCandidate {
    private:
        string gameId;  
        string gameName;
        int score;
        int support;

    public:
        GameCandidate();
        GameCandidate(const string& id, const string& gameName, int s, int sup);

        const string& getGameId() const;
        const string& getGameName() const;
        int getScore() const;
        int getSupport() const;

};

bool byNameAsc(const GameCandidate& a, const GameCandidate& b);
bool byNameDesc(const GameCandidate& a, const GameCandidate& b);
bool byScoreAsc(const GameCandidate& a, const GameCandidate& b);
bool byScoreDesc(const GameCandidate& a, const GameCandidate& b);
bool bySupportAsc(const GameCandidate& a, const GameCandidate& b);
bool bySupportDesc(const GameCandidate& a, const GameCandidate& b);

ostream& operator<<(std::ostream& os, const GameCandidate& c);