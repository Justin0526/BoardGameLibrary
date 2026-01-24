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
#include "Member.h"

using namespace std;

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

static void printAllGames(const List<Game>& games) {
    cout << "Available games:\n";
    games.print();
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

    // Demo member store (in a real app you'd load members from storage)
    Member demoMember(1, "Alice", "pw");

    while (true) {
        cout << "\nMain Menu\n";
        cout << "1) Admin Login\n";
        cout << "2) Member Login\n";
        cout << "3) Quit\n";
        cout << "Select option: ";
        string option;
        if (!getline(cin, option)) break;

        if (option == "2") {
            // require login
            cout << "Member Login\n";
            cout << "Username: ";
            string username;
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
				cout << "2) View all games\n";
                cout << "3) Return a game by id\n";
                cout << "4) View my borrowed/history\n";
                cout << "5) Logout\n";
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
					printAllGames(games);
                }
                else if (mopt == "3") {
                    cout << "Enter game id to return: ";
                    string idStr;
                    getline(cin, idStr);
                    try {
                        int id = stoi(idStr);
                        if (!demoMember.returnGame(games, id)) {
                            cout << "Return failed.\n";
                        }
                    } catch (...) {
                        cout << "Invalid id.\n";
                    }
                }
                else if (mopt == "4") {
                    demoMember.displayGamesBorrowedReturnedByMember();
                }
                else { // logout or any other input
                    cout << "Logging out...\n";
                    break;
                }
            }
        }
        else if (option == "3") {
            cout << "Exiting.\n";
            break;
        }
        else {
            cout << "Invalid option.\n";
        }
    }

    return 0;
}


