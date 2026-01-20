#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <exception>
#include <vector>

#include "Game.h"
#include "LinkedList.h"
#include "HashTable.h"

using namespace std;

void displayMenu() {
    cout << "\n--------Tabletop Games Club--------" << endl;
    cout << "1. Login as Administrator\n";
    cout << "2. Login as member\n";
    cout << "0 to exit\n";
}

// proper CSV line parsing (handles quotes + commas)
// vector is a dynamic array in C++ , can grow as you push items into it
static vector<string> parseCsvLine(const string& line) {
    vector<string> fields;  // Where we store each column
    string cur;             // current column we are building character by character
    bool inQuotes = false;  // are we currently inside " ... " ?

    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];

        if (c == '"') {
            // handle escaped quotes: "" inside quoted field
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                cur.push_back('"');
                i++;
            }
            else {
                inQuotes = !inQuotes;
            }
        }
        else if (c == ',' && !inQuotes) {
            fields.push_back(cur);
            cur.clear();
        }
        else {
            cur.push_back(c);
        }
    }
    fields.push_back(cur);
    return fields;
  
}

static string trimCR(std::string s) {
    if (!s.empty() && s.back() == '\r')
        s.pop_back(); // remove Windows CR
    return s;
}

static string stripOuterQuotes(string s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
        return s.substr(1, s.size() - 2);
    return s;
}

bool loadGamesFromCSV(const string& filename, List<Game>& games, HashTable<string, List<Game>::NodePtr>& gameTable) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Failed to open file: " << filename << endl;
        return false;
    }

    string line;

    // 1) Skip header line
    if (!getline(file, line))
        return false;

    // 2) Read each data row
    while (getline(file, line)) {
        if (line.empty()) continue;

        vector<string> cols = parseCsvLine(line);
        if (cols.size() != 6) {
            cout << "Bad column count (" << cols.size() << "): [" << line << "]\n";
            continue;
        }

        string name = stripOuterQuotes(cols[0]);
        string minP = cols[1];
        string maxP = cols[2];
        string maxT = cols[3];
        string minT = cols[4];
        string year = trimCR(cols[5]);

        // Create object and add to list
        try {
            Game g(
                name,
                stoi(minP),
                stoi(maxP),
                stoi(maxT),
                stoi(minT),
                stoi(year)
            );
            List<Game>::NodePtr gamePtr = games.add(g);
            gameTable.add(name, gamePtr);
        }catch (const exception& e) {
            cout << "Parse error: " << e.what() << "\n";
            cout << "Bad line: [" << line << "]\n";
        }
    }
    file.close();
    return true;
}

int main()
{
    for (auto& p : filesystem::directory_iterator(filesystem::current_path())) {
        cout << p.path().filename().string() << endl;
    }

    List<Game> games;
    HashTable<string, List<Game>::NodePtr> gameTable; // Store address of the linked list node in the hash table
    loadGamesFromCSV("games.csv", games, gameTable);
    
    List<Game>::NodePtr node = gameTable.get("Zooloretto");
    if (node != nullptr) {
        cout << node << endl;
        cout << node->item.getName() << endl;

        // Delete node from linked list
        games.remove(node);

        // Remove key from hash table
        gameTable.remove("Zooloretto");

        node = gameTable.get("Zooloretto");
        if (node == nullptr) {
            cout << "deleted!";
        }
        else {
            cout << node << endl;
            cout << node->item.getName() << endl;
        }
    }
    games.print();

    
    int option = -1;
    
    while (option != 0) {
        displayMenu();
        cout << "Enter your option: ";
        cin >> option;
        if (option == 0)
            cout << "Bye Bye!" << endl;
        else if (option == 1)
            cout << "Admin options (Justin)\n";
        else if (option == 2)
            cout << "User options (Khaleel)\n";
        else
            cout << "Invalid option!\n";
    }
}


