#include "Member.h"
#include "Game.h"
#include <iostream>
using namespace std;

Member::Member() : User() {}

Member::Member(int id, string name, string password)
    : User(id, name, password, "Member") {
}

bool Member::borrowGame(List<Game>& games, const string& gameName) {
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
            cout << "Member " << getName() << " borrowed game [" << g.getId() << "] " << g.getName() << endl;
            return true;
        }
    }
    cout << "No available copy found for \"" << gameName << "\"\n";
    return false;
}

bool Member::returnGame(List<Game>& games, int gameId) {
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

            cout << "Member " << getName() << " returned game [" << g.getId() << "] " << g.getName() << endl;
            return true;
        }
    }
    cout << "Game ID " << gameId << " not found or not currently borrowed by anyone.\n";
    return false;
}

void Member::displayGamesBorrowedReturnedByMember() {
    cout << "Displaying member borrow/return summary" << endl;
}
void Member::displayGamesBorrowedReturnedByMember() const {
    cout << "Member: " << getName() << " (ID:" << getUserId() << ")\n";
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
