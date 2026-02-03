#include "Member.h"
#include "Game.h"
#include <iostream>
using namespace std;

Member::Member() : User() {}

Member::Member(int id, string name, string password)
    : User(id, name, password, "Member") {
}

Member::Member(int id, string name): User(id, name, "member") { }

void Member::displayGamesBorrowedReturnedByMember() const {
    // forward to the User implementation that now owns the data
    displayBorrowedAndHistory();
}
