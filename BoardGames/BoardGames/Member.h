#ifndef MEMBER_H
#define MEMBER_H

#include "User.h"

class Member : public User {
public:
    Member();
    Member(int id, string name, string password);

    bool borrowGame(int gameId);
    bool returnGame(int gameId);
    void displayGamesBorrowedReturnedByMember();
};

#endif