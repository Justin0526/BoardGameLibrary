/*********************************************************************************
 * Group         : T01
 * Team Member   : Khaleel Anis (S10270243)
 *
 * File Purpose:
 * - Declares the Member class, which represents a standard club member
 *   in the Tabletop Games Club system.
 *
 * Key Design Notes:
 * - Member is a concrete subclass of User.
 * - Reuses all borrowing, returning, and history-tracking logic from User.
 * - Does not introduce additional state beyond what User already manages.
 *
 * Constraints / Assumptions:
 * - Member accounts do not have administrative privileges.
 * - Borrowed and history data are maintained exclusively by the User base class.
 *********************************************************************************/
#ifndef MEMBER_H
#define MEMBER_H

#include "User.h"
#include <iostream>
using namespace std;
#include "LinkedList.h"
#include <string>

class Game; // Forward declaration to reduce header coupling

class Member : public User {
public:
    Member();
    Member(int id, string name);

    // Displays the list of games borrowed and returned by this member.
    // Internally delegates to User-owned history logic.
    void displayGamesBorrowedReturnedByMember() const;
};

#endif