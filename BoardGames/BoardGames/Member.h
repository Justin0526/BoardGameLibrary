#ifndef MEMBER_H
#define MEMBER_H

#include "User.h"
#include <iostream>
using namespace std;
#include "LinkedList.h"
#include <string>

class Game; // forward

class Member : public User {
public:
    Member();
    Member(int id, string name, string password);
    Member(int id, string name);

    void displayGamesBorrowedReturnedByMember() const;
};

#endif