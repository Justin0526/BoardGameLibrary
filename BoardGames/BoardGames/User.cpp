#include "User.h"
#include "Game.h"
#include <limits>
#include <sstream>
#include <iostream>

User::User() {
    userId = 0;
    name = "";
    password = "";
    role = "";
}

User::User(int id, string name, string password, string role) {
    this->userId = id;
    this->name = name;
    this->password = password;
    this->role = role;
}

bool User::login(string inputName, string inputPassword) const {
    return (name == inputName && password == inputPassword);
}

int User::getUserId() const {
    return userId;
}

string User::getName() const {
    return name;
}

string User::getRole() const {
    return role;
}

bool User::borrowGame(List<Game>& games, const string& gameName) {
    int n = games.getLength();
    // find first not-borrowed copy
    for (int i = 0; i < n; ++i) {
        auto node = games.getNode(i);
        if (node == nullptr) continue;
        Game& g = games.getItem(node);
        if (g.getName() == gameName && !g.isBorrowed()) {
            g.setBorrowed(true);
            borrowed.add(g.getId());
            history.add(g.getId());
            cout << role << " " << getName() << " borrowed game [" << g.getId() << "] " << g.getName() << endl;
            return true;
        }
    }
    cout << "No available copy found for \"" << gameName << "\"\n";
    return false;
}

bool User::returnGame(List<Game>& games, int gameId) {
    int n = games.getLength();
    for (int i = 0; i < n; ++i) {
        auto node = games.getNode(i);
        if (node == nullptr) continue;
        Game& g = games.getItem(node);
        if (g.getId() == gameId && g.isBorrowed()) {
            g.setBorrowed(false);
            // remove gameId from borrowed list by rebuilding as List lacks removeAt
            List<int> newBorrowed;
            int len = borrowed.getLength();
            bool removed = false;
            for (int j = 0; j < len; ++j) {
                int id = borrowed.get(j);
                if (!removed && id == gameId) { removed = true; continue; }
                newBorrowed.add(id);
            }
            borrowed = List<int>();
            for (int j = 0; j < newBorrowed.getLength(); ++j) borrowed.add(newBorrowed.get(j));

            cout << role << " " << getName() << " returned game [" << g.getId() << "] " << g.getName() << endl;
            return true;
        }
    }
    cout << "Game ID " << gameId << " not found or not currently borrowed by anyone.\n";
    return false;
}

void User::displayBorrowedAndHistory() const {
    cout << role << ": " << getName() << " (ID:" << getUserId() << ")\n";
    cout << "Currently borrowed (" << borrowed.getLength() << "):\n";
    if (borrowed.getLength() == 0) {
        cout << "  None\n";
    } else {
        for (int i = 0; i < borrowed.getLength(); ++i) {
            cout << "  Game ID: " << borrowed.get(i) << "\n";
        }
    }

    cout << "Borrow history (" << history.getLength() << "):\n";
    if (history.getLength() == 0) {
        cout << "  None\n";
    } else {
        for (int i = 0; i < history.getLength(); ++i) {
            cout << "  Game ID: " << history.get(i) << "\n";
        }
    }
}

void User::displayGamesPlayableByNPlayers(List<Game>& games) {
    cout << "---- Display a list of games that can be played by a given number of players ----\n";
    // Ask user for number of players
    string input;
    int n;
    cout << "Number of player(s): ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    while (true) {
        getline(cin, input);
        stringstream ss(input);

        if (!(ss >> n) || n <= 0 || !ss.eof()) {
            cout << "Invalid input. Please enter a whole number 1 or more.\n";
            cout << "Number of player(s): ";
        }
        else {
            break;
        }
    }

    // Ask user for sorting choice
    int option = -1;
    bool (*cmp)(const Game&, const Game&) = nullptr;

    while (true) {
        cout << "\n-- Sort By --" << endl;
        cout << "0. Default\n";
        cout << "1. Name in Ascending order\n";
        cout << "2. Name in Descending order\n";
        cout << "3. Published year in Ascending order\n";
        cout << "4. Published year in Descending order\n";

        cout << "Enter your option: ";
        cin >> option;
        cout << endl;

        if (option == 0) {
            cout << "Loading in default view...\n";
            break;
        }

        else if (option == 1)
        {
            cout << "Loading NAME in ascending order...\n";
            cmp = byNameAsc;
            break;
        }

        else if (option == 2) {
            cout << "Loading NAME in descending order...\n";
            cmp = byNameDesc;
            break;
        }

        else if (option == 3) {
            cout << "Loading PUBLISHED YEAR in ascending order...\n";
            cmp = byYearAsc;
            break;
        }
            
        else if (option == 4) {
            cout << "loading PUBLISHED YEAR in descendig order...\n";
            cmp = byYearDesc;
            break;
        }
           
        else
            cout << "Invalid option!";

    }
  
    // Filter games into new list
    // [&]: lambda to capture variables from surrounding scope by reference, in this case it is int n
    List<Game> filtered = games.filter([&](const Game& g) {
        return n >= g.getMinPlayer() && n <= g.getMaxPlayer();
    });
   
    // sort filtered list using merge sort
    if (cmp != nullptr) {
        filtered.sort(cmp);
    }
    cout << "Name | MinPlayer | MaxPlayer | MaxPlayTime | MinPlayTime | YearPublished\n";
    // print
    filtered.print();
}

ostream& operator<<(ostream& os, const User& u) {
    os << u.getName() << " (" << u.getUserId() << ")" << endl;
    return os;
}