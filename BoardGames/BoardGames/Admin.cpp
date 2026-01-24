#include "Admin.h"
#include <iostream>
#include <string>
#include <sstream>


using namespace std;

Admin::Admin() : User() {}

Admin::Admin(int id, string name, string password)
    : User(id, name, password, "Admin") {
}

void Admin::addGame(List<Game>& games, HashTable<string, List<Game>::NodePtr>& gameTable) {
    // Allow admin to fill in fields
    cout << "----Add a game----\n";

    // Game name
    string name;
    cout << "Name of Game: ";

    // cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, name);
    
    // min player of game
    int minPlayer;
    string input;
    cout << "Minimum player(s): ";

    while (true) {
        getline(cin, input);
        stringstream ss(input);

        if (!(ss >> minPlayer) || minPlayer <= 0 || !ss.eof()) {
            cout << "Invalid input. Please enter a whole number 1 or more.\n";
            cout << "Minimum player(s): ";
        }
        else {
            break;
        }
    }

    // max player
    int maxPlayer;
    cout << "Maximum player(s): ";
    
    while (true) {
        getline(cin, input);
        stringstream ss(input);

        if (!(ss >> maxPlayer) || maxPlayer < minPlayer || !ss.eof()) {
            cout << "Invalid input. Please enter a whole number that's >= " << minPlayer << " (minimum player)";
            cout << "Maximum player(s): ";
        }
        else {
            break;
        }
    }

    // min play time
    int minPlayTime;
    cout << "Minimum playtime: ";

    while (true) {
        getline(cin, input);
        stringstream ss(input);
        if (!(ss >> minPlayTime) || minPlayTime <= 0 || !ss.eof()) {
            cout << "Invalid input. Please enter a whole number that's more than 0\n";
            cout << "Minimum playtime: ";
        }
        else {
            break;
        }
    }

    // max play time
    int maxPlayTime;
    cout << "Maximum playtime: ";
    
    while (true) {
        getline(cin, input);
        stringstream ss(input);
        if (!(ss >> maxPlayTime) || maxPlayTime < minPlayTime || !ss.eof()) {
            cout << "Invalid input. Please enter a whole number that is >= " << minPlayTime << " (min play time)\n";
            cout << "Maximum playtime: ";
        }
        else {
            break;
        }
    }

    // yearpublished
    int yearPublished;
    cout << "Year published: ";
    while (true) {
        getline(cin, input);
        stringstream ss(input);
        if (!(ss >> yearPublished) || !ss.eof()) {
            cout << "Invalid input! Please enter  a whole number!\n";
            cout << "Year published: ";
        }
        else {
            break;
        }
    }

    Game g(name, minPlayer, maxPlayer, maxPlayTime, minPlayTime, yearPublished);
    List<Game>::NodePtr gamePtr = games.add(g);
    gameTable.add(name, gamePtr);

}

void Admin::removeGame(List<Game>& games, HashTable<string, List<Game>::NodePtr>& gameTable) {
    cout << "---- Remove a game ----\n";

    string name;
    cout << "Name of game: ";

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    getline(cin, name);
    List<Game>::NodePtr node = gameTable.get(name);
    if (node != nullptr) {
        cout << "Deleting " << node->item.getName() << "...\n";

        // Delete node from linked list
        games.remove(node);

        // Remove key from hash table
        gameTable.remove(name);

        cout << "Successfully deleted " << name << endl;
    }
    else {
        cout << "Invalid game name! Please enter a valid game name\n";
    }
}

void Admin::addMember(List<Member>& members, HashTable<string, List<Member>::NodePtr>& memberTable) {
    cout << "---- Add a new member ----\n" << endl;

}

void Admin::displayGameBorrowReturnSummary() {
    cout << "Displaying overall borrow/return summary" << endl;
}
