#ifndef MEMBER_H
#define MEMBER_H

#include "User.h"
#include <iostream>
using namespace std;
#include "LinkedList.h"
#include <string>
using namespace std;

class Game; // forward

class Member : public User {
private:
    // track currently borrowed game IDs and history of borrowed IDs
    List<int> borrowed;
    List<int> history;
public:
    Member();
    Member(int id, string name, string password);
    Member(int id, string name);

    // borrow by game name (searches games list for available copy)
    bool borrowGame(List<Game>& games, const string& gameName);

    // return by game id
    bool returnGame(List<Game>& games, int gameId);

    void displayGamesBorrowedReturnedByMember();
};

#endif