#ifndef ADMIN_H
#define ADMIN_H

#include "User.h"

class Admin : public User {
public:
    Admin();
    Admin(int id, string name, string password);

    void addGame();
    void removeGame(int gameId);
    void addMember();
    void displayGameBorrowReturnSummary();
};

#endif