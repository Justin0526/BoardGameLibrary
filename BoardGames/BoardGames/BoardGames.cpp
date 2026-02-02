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
#include "User.h"
#include "Admin.h"
#include "Member.h"

using namespace std;

void displayMenu() {
    cout << "\n--------Tabletop Games Club--------" << endl;
    cout << "1. Login as Administrator\n";
    cout << "2. Login as member\n";
    cout << "3. Display games\n";
    cout << "4. Display all members\n";
    cout << "0. EXIT\n";
}

void adminMenu() {
    cout << "\n--------Admin--------" << endl;
    cout << "1. Add a new board game\n";
    cout << "2. Remove a board game\n";
    cout << "3. Add a new member\n";
    cout << "0. EXIT\n";
}

void displayGameMenu() {
    cout << "\n--------Display Board games--------" << endl;
    cout << "1. Default\n";
    cout << "2. Display a list of games that can be played by a given number of players\n";
    cout << "0. EXIT\n";
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

    int nextId = 1; // assign incremental id per game copy

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
            g.setId(nextId++);
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
    // show files in current directory
    for (auto& p : filesystem::directory_iterator(filesystem::current_path())) {
        cout << p.path().filename().string() << endl;
    }

    List<Game> games;
    HashTable<string, List<Game>::NodePtr> gameTable; // Store address of the linked list node in the hash table
    loadGamesFromCSV("games.csv", games, gameTable);

    // Admins
    Admin admin(1, "Justin", "Justin");

    // Members
    List<Member> members;
    HashTable<string, List<Member>::NodePtr> memberTable;
    Member mem1(1, "Test member 1", "Test1");
    members.add(mem1);

    // Demo member store (in a real app you'd load members from storage)
    Member demoMember(1, "Alice", "pw");

    int option = -1;
    
    while (option != 0) {
        displayMenu();
        cout << "Enter your option: ";
        cin >> option;
        cout << endl;

        if (option == 0)
            cout << "Bye Bye!" << endl;

        else if (option == 1) {
            int adminOption = -1;

            while (adminOption != 0) {
                adminMenu();
                cout << "Enter your option: ";
                cin >> adminOption;
                cout << endl;

                if (adminOption == 0)
                    cout << "Exiting to main menu...\n";
                else if (adminOption == 1)
                    admin.addGame(games, gameTable);
                else if (adminOption == 2)
                    admin.removeGame(games, gameTable);
                else if (adminOption == 3)
                    admin.addMember(members, memberTable);
                else
                    cout << "Invalid admin operation!\n";
            }
        }
        else if (option == 2) {
            // require login
            cout << "Member Login\n";
            cout << "Username: ";
            string username;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin, username);
            cout << "Password: ";
            string password;
            getline(cin, password);

            if (!demoMember.login(username, password)) {
                cout << "Login failed. Returning to main menu.\n";
                continue;
            }

            cout << "Login successful. Welcome, " << demoMember.getName() << "!\n";

            // member menu
            while (true) {
                cout << "\nMember Menu\n";
                cout << "1) Borrow a game by name\n";
                cout << "2) Return a game by id\n";
                cout << "3) View my borrowed/history\n";
                cout << "4) Logout\n";
                cout << "Select option: ";
                string mopt;
                if (!getline(cin, mopt)) { mopt = "4"; }

                if (mopt == "1") {
                    cout << "Enter game name to borrow: ";
                    string gameName;
                    getline(cin, gameName);
                    if (!demoMember.borrowGame(games, gameName)) {
                        cout << "Borrow failed (no available copy or error).\n";
                    }
                }
                else if (mopt == "2") {
                    cout << "Enter game id to return: ";
                    string idStr;
                    getline(cin, idStr);
                    try {
                        int id = stoi(idStr);
                        if (!demoMember.returnGame(games, id)) {
                            cout << "Return failed.\n";
                        }
                    }
                    catch (...) {
                        cout << "Invalid id.\n";
                    }
                }
                else if (mopt == "3") {
                    demoMember.displayGamesBorrowedReturnedByMember();
                }
                else { // logout or any other input
                    cout << "Logging out...\n";
                    break;
                }
            }
        }

        else if (option == 3) {
            int displayOption = -1;
            while (displayOption != 0) {
                displayGameMenu();
                cout << "Enter your option: ";
                cin >> displayOption;
                cout << endl;

                if (displayOption == 0)
                    cout << "Exiting to main menu...\n";

                else if (displayOption == 1) {
                    cout << "Name | MinPlayer | MaxPlayer | MaxPlayTime | MinPlayTime | YearPublished\n";
                    games.print();
                }

                else if (displayOption == 2)
                    admin.displayGamesPlayableByNPlayers(games);
            }
        }
           
        else if (option == 4)
            members.print();
        else
            cout << "Invalid option!\n";
    }
}


