#include "User.h"
#include "Game.h"
#include "LinkedList.h"
#include "HashTable.h"
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

User::User(int id, string name, string role) {
    this->userId = id;
    this->name = name;
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

void User::printActiveGames(List<Game>& games) {
    for (int i = 0; i < games.getLength(); i++) {
        List<Game>::NodePtr node = games.getNode(i);
        Game& g = node->item;

        if (g.getIsActive() == "TRUE") {
            cout << g;
        }
    }
}

bool User::borrowGame(List<Game>& games, HashTable<string, List<Game>::NodePtr>& gameTable, const string& gameName) {
    // Use hash table for fast lookup
    auto node = gameTable.get(gameName);
    if (node && !games.getItem(node).isBorrowed()) {
        Game& g = games.getItem(node);
        g.setBorrowed(true);
        // If you have a borrowed/history list, update it here
        cout << role << " " << name << " borrowed game [" << g.getGameId() << "] " << g.getName() << endl;
        return true;
    }
    cout << "No available copy found for \"" << gameName << "\"\n";
    return false;
}

bool User::returnGame(List<Game>& games, HashTable<std::string, List<Game>::NodePtr>& gameTable, int gameId) {
    // Try to find by ID as string
    auto node = gameTable.get(std::to_string(gameId));
    if (node && games.getItem(node).isBorrowed()) {
        Game& g = games.getItem(node);
        g.setBorrowed(false);
        cout << role << " " << name << " returned game [" << g.getGameId() << "] " << g.getName() << endl;
        return true;
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
    os << u.getName() << " [User ID: " << u.getUserId() << "]" << endl;
    return os;
}