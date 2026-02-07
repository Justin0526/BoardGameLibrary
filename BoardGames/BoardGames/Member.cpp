/*********************************************************************************
 * Group         : T01
 * Team Member   : Khaleel Anis (S10270243)
 *
 * File Purpose:
 * - Implements the Member class, a concrete User subtype for regular members.
 *
 * Key Design Notes:
 * - Constructors forward initialization to the User base class.
 * - Member-specific behaviour is minimal and relies on User functionality.
 * - Borrow/return history is owned and managed by User to avoid duplication.
 *
 * Constraints / Assumptions:
 * - The role string is fixed as "member" for access control and menu logic.
 *********************************************************************************/
#include "Member.h"
#include "Game.h"
#include <iostream>
using namespace std;

// Default constructor
Member::Member() : User() {}

// Constructor
Member::Member(int id, string name): User(id, name, "member") { }

void Member::displayGamesBorrowedReturnedByMember() const {
    // Forward to the User implementation that owns borrow/history data
    displayBorrowedAndHistory();
}
