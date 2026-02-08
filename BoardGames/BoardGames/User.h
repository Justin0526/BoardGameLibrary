/*********************************************************************************
 * Group         : T01
 * Team Member   : Khaleel Anis (S10270243)
 *
 * File Purpose:
 * - Declares the User base class.
 * - Encapsulates shared behaviour for all system users
 *   (Members and Administrators).
 *
 * Key Design Notes:
 * - Acts as the central authority for borrowing and returning logic.
 * - Maintains per-user state via borrowed and history lists.
 * - Designed to be extended by derived classes (e.g. Member, Admin).
 *
 * Constraints / Assumptions:
 * - User objects are created from CSV data and persist in memory.
 * - Borrowed state is enforced at runtime and mirrored to CSV logs.
 * - One physical copy per game is assumed.
 *********************************************************************************/
#ifndef USER_H
#define USER_H

#include <string>
#include <iostream>
#include <sstream>
#include "HashTable.h"
#include "LinkedList.h"
#include "Game.h"
#include "Rating.h"
#include "GameCandidate.h"
using namespace std;

class User {
protected:
    // ---- Identity & role ----
    int userId;
    string name;
    string role;

    // ---- Borrowing state ----
    // borrowed : games currently borrowed by this user
    // history  : audit trail of all borrow/return actions
    List<int> borrowed;
    List<int> history;

public:
    // ---- Constructors ----
    User();
    User(int id, string name, string role);

    // ---- Accessors ----
    int getUserId() const;
    string getName() const;
    string getRole() const;

    // ---- Game browsing ----
    void printActiveGames(List<Game>& games);

    // ---- Borrow / Return operations ----
    bool borrowGame(List<Game>& games, HashTable<std::string, List<Game>::NodePtr>& gameTable, const std::string& gameName);
    bool borrowGameById(
        List<Game>& games,
        HashTable<string, List<Game>::NodePtr>& gameTable,
        int gameId
    );
    bool returnGame(List<Game>& games, HashTable<std::string, List<Game>::NodePtr>& gameTable, int gameId);
    void displayBorrowedAndHistory() const;

    // Displays recommended game candidates and allows user-selected sorting.
    void displayGamesPlayableByNPlayers(List<Game>& games);

    // Generates game recommendations based on collaborative filtering.
    // Uses ratings from similar users to compute candidate scores and support.
    void recommendFromGame(
        int gameId,
        List<Rating>& ratings,
        List<GameCandidate>& outcandidates,
        HashTable<string, List<List<Rating>::NodePtr>*>& gameRatings,
        HashTable<string, List<List<Rating>::NodePtr>*>& memberRatings,
        List<Game>& games,
        HashTable<string, List<Game>::NodePtr>& gameTable);

    void printRecommendedGames(
        List<GameCandidate>& candidates,
        List<Game>& games, HashTable<string,
        List<Game>::NodePtr>& gameTable);

    void searchGameByNameOrId(List<Game>& games, HashTable<string, List<Game>::NodePtr>& gameTable);

};

ostream& operator<<(std::ostream& os, const User& u);

#endif