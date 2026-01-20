#include "Admin.h"
#include <iostream>
using namespace std;

Admin::Admin() : User() {}

Admin::Admin(int id, string name, string password)
    : User(id, name, password, "Admin") {
}

void Admin::addGame() {
    cout << "Add game called" << endl;
}

void Admin::removeGame(int gameId) {
    cout << "Remove game ID: " << gameId << endl;
}

void Admin::addMember() {
    cout << "Add member called" << endl;
}

void Admin::displayGameBorrowReturnSummary() {
    cout << "Displaying overall borrow/return summary" << endl;
}
