#ifndef USER_H
#define USER_H

#include <string>
#include <iostream>
#include <sstream>
#include "LinkedList.h"
#include "Game.h"
using namespace std;

class User {
protected:
    int userId;
    string name;
    string password;
    string role;

public:
    User();
    User(int id, string name, string password, string role);

    bool login(string inputName, string inputPassword) const;

    int getUserId() const;
    string getName() const;
    string getRole() const;

    void displayGamesPlayableByNPlayers(List<Game>& games);
};

ostream& operator<<(std::ostream& os, const User& u);

#endif