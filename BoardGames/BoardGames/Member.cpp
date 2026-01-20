#include "Member.h"
#include <iostream>
using namespace std;

Member::Member() : User() {}

Member::Member(int id, string name, string password)
    : User(id, name, password, "Member") {
}

bool Member::borrowGame(int gameId) {
    cout << "Borrowing game ID: " << gameId << endl;
    return true;
}

bool Member::returnGame(int gameId) {
    cout << "Returning game ID: " << gameId << endl;
    return true;
}

void Member::displayGamesBorrowedReturnedByMember() {
    cout << "Displaying member borrow/return summary" << endl;
}