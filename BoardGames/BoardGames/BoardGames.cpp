#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <exception>
#include <vector>
#include <limits>

#include "BorrowRecord.h"
#include "Game.h"
#include "LinkedList.h"
#include "HashTable.h"
#include "User.h"
#include "Admin.h"
#include "Member.h"

using namespace std;

int borrowRecordCounter; // Remove = 1, will be set by getNextBorrowRecordId()

void displayMenu() {
    cout << "\n--------Tabletop Games Club--------" << endl;
    cout << "1. Login as Administrator\n";
    cout << "2. Login as Member\n";
    cout << "3. Display Games\n";
    cout << "4. Display All Members\n";
    cout << "0. EXIT\n";
}

List<Admin>::NodePtr adminLoginMenu(List<Admin>& admins) {
    cout << "\n---- Admin Login ----\n";
    cout << "Select admin by index:\n";
    admins.printWithIndex();
    cout << "0. EXIT\n";
    cout << "Choice: ";

    int choice;
    cin >> choice;

    if (choice <= 0) return nullptr;

    return admins.getNode(choice - 1); 
}

List<Member>::NodePtr memberLoginMenu(List<Member>& members) {
    cout << "\n---- Member Login ----\n";
    cout << "Select member by index:\n";
    members.printWithIndex();
    cout << "0. EXIT\n";
    cout << "Choice: ";

    int choice; 
    cin >> choice;
    if (choice <= 0) return nullptr;

    return members.getNode(choice - 1);
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

// Function to display detailed borrow history
void displayDetailedBorrowHistory(int memberId, List<Game>& games) {
    vector<BorrowHistoryRecord> borrowHistory;
    loadMemberBorrowHistoryDetailed(memberId, games, borrowHistory);
    
    cout << "\n============= YOUR DETAILED BORROW HISTORY =============\n";
    if (borrowHistory.empty()) {
        cout << "No borrow history found.\n";
    } else {
        for (size_t i = 0; i < borrowHistory.size(); ++i) {
            const BorrowHistoryRecord& record = borrowHistory[i];
            cout << "\n--- Record " << (i + 1) << " ---\n";
            cout << "Action: " << record.action << "\n";
            
            if (record.gameDetails != nullptr) {
                Game& g = *(record.gameDetails);
                cout << "Game Details:\n";
                cout << "  ID: " << g.getId() << "\n";
                cout << "  Name: " << g.getName() << "\n";
                cout << "  Players: " << g.getMinPlayer() << " - " << g.getMaxPlayer() << " players\n";
                cout << "  Play Time: " << g.getMinPlayTime() << " - " << g.getMaxPlayTime() << " minutes\n";
                cout << "  Year Published: " << g.getYearPublished() << "\n";
                cout << "  Current Status: " << (g.isBorrowed() ? "BORROWED" : "AVAILABLE") << "\n";
            } else {
                cout << "Game ID/Name: " << record.gameId << "\n";
                cout << "  Game details not found (may have been removed)\n";
            }
        }
    }
    cout << "========================================================\n";
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
        if (cols.size() != 9) {
            cout << "Bad column count (" << cols.size() << "): [" << line << "]\n";
            continue;
        }

        string gameId = cols[0];
        string name = stripOuterQuotes(cols[1]);
        string minP = cols[2];
        string maxP = cols[3];
        string minT = cols[4];
        string maxT = cols[5];
        string year = cols[6];
        string copy = trimCR(cols[7]);
        string isActive = cols[8];

        // Create object and add to list
        try {
            Game g(
                stoi(gameId),
                name,
                stoi(minP),
                stoi(maxP),
                stoi(minT),
                stoi(maxT),
                stoi(year),
                stoi(copy),
                isActive
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

bool loadUsersFromCSV(const string& filename, List<Admin>& admins, List<Member>& members, List<User>& users, 
    HashTable<string, List<Admin>::NodePtr>& adminTable, HashTable<string, List<Member>::NodePtr>& memberTable, HashTable<string, List<User>::NodePtr>& userTable) {
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
        stringstream ss(line);
        string id;
        getline(ss, id, ',');

        string name;
        getline(ss, name, ',');

        string role;
        getline(ss, role, ',');

        id = trimCR(stripOuterQuotes(id));
        name = trimCR(stripOuterQuotes(name));
        role = trimCR(stripOuterQuotes(role));

        if (role == "admin") {
            Admin a(stoi(id), name);
            List<Admin>::NodePtr adminPtr = admins.add(a);
            adminTable.add(name, adminPtr);
        }
        else {
            Member m(stoi(id), name);
            List<Member>::NodePtr memberPtr = members.add(m);
            memberTable.add(name, memberPtr);
        }

        User u(stoi(id), name, role);
        List<User>::NodePtr userPtr = users.add(u);
        userTable.add(name, userPtr);
    }

    file.close();
    return true;
}

int main()
{
  // Initialize borrow record counter from existing records
    borrowRecordCounter = getNextBorrowRecordId();  
    List<Game> games;
    HashTable<string, List<Game>::NodePtr> gameTable;
    loadGamesFromCSV("games.csv", games, gameTable);
    
    // Restore borrowed states from borrow records
    restoreGameBorrowStates(games);

    List<Admin> admins;
    List<Member> members;
    List<User> users;

    HashTable<string, List<Admin>::NodePtr> adminTable;
    HashTable<string, List<Member>::NodePtr> memberTable;
    HashTable<string, List<User>::NodePtr> userTable;
    loadUsersFromCSV("users.csv", admins, members, users, adminTable, memberTable, userTable);

    User globalUser(-1, "global user", "admin");

    int option = -1;
    
    while (option != 0) {
        displayMenu();
        cout << "Enter your option: ";
        cin >> option;
        cout << endl;

        if (option == 0)
            cout << "Bye Bye!" << endl;

        else if (option == 1) {
            List<Admin>::NodePtr loggedInAdmin = adminLoginMenu(admins);

            if (loggedInAdmin == nullptr) {
                cout << "Exiting to main menu...\n";
                continue;
            }

            int adminOption = -1;
            while (adminOption != 0) {
                adminMenu();
                cout << "Enter your option: ";
                cin >> adminOption;
                cout << endl;

                if (adminOption == 0)
                    cout << "Exiting to main menu...\n";
                else if (adminOption == 1)
                    loggedInAdmin->item.addGame(games, gameTable);
                else if (adminOption == 2)
                    loggedInAdmin->item.removeGame(games, gameTable);
                else if (adminOption == 3)
                    loggedInAdmin->item.addMember(members, memberTable, users, userTable);
                else
                    cout << "Invalid admin operation!\n";
            }
        }

        else if (option == 2) {
            List<Member>::NodePtr loggedInMember = memberLoginMenu(members);

            if (loggedInMember == nullptr) {
                cout << "Exiting to main menu...\n";
                continue;
            }

            cout << "Login successful. Welcome, " << loggedInMember->item.getName() << "!\n";

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
                    if (loggedInMember->item.borrowGame(games, gameName)){
                        writeBorrowRecord(borrowRecordCounter++, to_string(loggedInMember->item.getUserId()), gameName, "BORROW");
                        cout << "Borrow successful.\n";
                    }
                    else{
                        cout << "Borrow failed (no available copy or error).\n";
                    }
                }
                else if (mopt == "2") {
                    cout << "Enter game id to return: ";
                    string idStr;
                    getline(cin, idStr);
                    try {
                        int id = stoi(idStr);
                        if (!loggedInMember->item.returnGame(games, id)) {
                            cout << "Return successful.\n";

                            writeBorrowRecord(
                                borrowRecordCounter++,
                                to_string(loggedInMember->item.getUserId()),     // memberId (string)
                                to_string(id),          // gameId
                                "RETURN"
                            );
                        }
                        else {
                            cout << "Return failed.\n";
                        }
                    }
                    catch (...) {
                        cout << "Invalid id.\n";
                    }
                }
                else if (mopt == "3") {
                    // Display detailed borrow history with full game information
                    displayDetailedBorrowHistory(loggedInMember->item.getUserId(), games);
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
                    cout << "GameID | Name | MinPlayer | MaxPlayer | MaxPlayTime | MinPlayTime | YearPublished | No. of Copies\n";
                    globalUser.printActiveGames(games);
                }

                else if (displayOption == 2)
                    globalUser.displayGamesPlayableByNPlayers(games);

                else
                    cout << "Invalid option!\n";
            }
        }
           
        else if (option == 4)
            members.print();
        else
            cout << "Invalid option!\n";
    }
}
