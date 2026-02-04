#pragma once
#include<string>
#include<iostream>
using namespace std;

class GameCandidate {
    private:
        string gameId;  
        int score;
        int support;

    public:
        GameCandidate();
        GameCandidate(const string& id, int s, int sup);

        const string& getGameId() const;
        int getScore() const;
        int getSupport() const;

};

ostream& operator<<(std::ostream& os, const GameCandidate& c);