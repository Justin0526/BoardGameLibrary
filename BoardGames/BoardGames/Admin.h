/*********************************************************************************
 * Group         : T01
 * Team Member   : Justin Tang Jia Ze (S10269496B)
 *
 * File Purpose:
 * - Declares the Admin class, which extends User with administrative privileges
 *   such as managing games and members.
 *
 * Key Design Notes:
 * - Admin inherits from User to reuse common user functionality.
 * - Provides management operations that modify system-wide data structures.
 *
 *********************************************************************************/
#ifndef ADMIN_H
#define ADMIN_H

#include "User.h"
#include "Game.h"
#include "Member.h"
#include "LinkedList.h"
#include "HashTable.h"

 // BorrowRow:
 // Represents a single row from borrow_records.csv.
 // Used for administrative inspection or management of borrow records.
struct BorrowRow {
    int recordId;
    string memberId;
    string gameId;
    string action;
    string borrowDate;
    string returnDate;
};

// Admin:
// Derived from User. Provides additional privileges for managing games
// and members within the system.
class Admin : public User {
public:
    // Default constructor
    Admin();

    // Parameterised constructor
    Admin(int id, string name);

    // Adds a new game into the system.
    // Updates both the game list and game hash table index.
    void addGame(List<Game>& games, HashTable<string, List<Game>::NodePtr>& gameTable);

    // Removes a game from the system using soft-delete logic.
    // Updates game list and hash table accordingly.
    void removeGame(List<Game>& games, HashTable<string, List<Game>::NodePtr>& gameTable);

    // Adds a new member to the system.
    // Updates member list, user list, and their corresponding hash tables.
    void addMember(List<Member>& members, HashTable<string, List<Member>::NodePtr>& memberTable, List<User>& users, HashTable<string, List<User>::NodePtr>& userTable);
};

#endif