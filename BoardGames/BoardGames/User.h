#ifndef USER_H
#define USER_H

#include <string>
#include <iostream>
#include <sstream>
#include "HashTable.h"
#include "LinkedList.h"
#include "Game.h"
#include "HashTable.h"
#include "Rating.h"
#include "GameCandidate.h"
using namespace std;

class User {
protected:
    int userId;
    string name;
    string password;
    string role;

    List<int> borrowed;
    List<int> history;

public:
    User();
    User(int id, string name, string password, string role);
    User(int id, string name, string role);

    bool login(string inputName, string inputPassword) const;

    int getUserId() const;
    string getName() const;
    string getRole() const;

    void printActiveGames(List<Game>& games);
    // Features available to all users
    bool borrowGame(List<Game>& games, HashTable<std::string, List<Game>::NodePtr>& gameTable, const std::string& gameName);
    bool returnGame(List<Game>& games, HashTable<std::string, List<Game>::NodePtr>& gameTable, int gameId);
    void displayBorrowedAndHistory() const;

    void displayGamesPlayableByNPlayers(List<Game>& games);
    void recommendFromGame(
        int gameId, 
        List<Rating>& ratings, 
        List<GameCandidate>& outcandidates, 
        HashTable<string, List<List<Rating>::NodePtr>*>& gameRatings, 
        HashTable<string, List<List<Rating>::NodePtr>*>& memberRatings,
        List<Game>& games, 
        HashTable<string, List<Game>::NodePtr>& gameTable);

    void filterRecommendedCandidates(
        List<GameCandidate>& in, 
        List<Game>& games, 
        HashTable<string, List<Game>::NodePtr>& gameTable,
        int desiredPlayers, 
        int desiredMinutes, 
        List<GameCandidate>& out);

    void printRecommendedGames(
        List<GameCandidate>& candidates, 
        List<Game>& games, HashTable<string, 
        List<Game>::NodePtr>& gameTable);
};

ostream& operator<<(std::ostream& os, const User& u);

#endif