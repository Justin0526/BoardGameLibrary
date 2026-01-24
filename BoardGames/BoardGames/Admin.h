#ifndef ADMIN_H
#define ADMIN_H

#include "User.h"
#include "Game.h"
#include "Member.h"
#include "LinkedList.h"
#include "HashTable.h"

class Admin : public User {
public:
    Admin();
    Admin(int id, string name, string password);

    void addGame(List<Game>& games, HashTable<string, List<Game>::NodePtr>& gameTable);
    void removeGame(List<Game>& games, HashTable<string, List<Game>::NodePtr>& gameTable);
    void addMember(List<Member>& members, HashTable<string, List<Member>::NodePtr>& memberTable));
    void displayGameBorrowReturnSummary();
};

#endif