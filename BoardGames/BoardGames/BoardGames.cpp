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
#include "Rating.h"
#include "GameCandidate.h"
#include "Reviews.h"

using namespace std;

int borrowRecordCounter; // Remove = 1, will be set by getNextBorrowRecordId()

void displayMenu() {
    cout << "\n--------Tabletop Games Club--------" << endl;
    cout << "1. Login as Administrator\n";
    cout << "2. Login as Member\n";
    cout << "3. Display Games\n";
    cout << "4. Recommend Games\n";
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
    cout << "4. Borrow/Return Summary\n";
    cout << "0. EXIT\n";
}

void displayGameMenu() {
    cout << "\n--------Display Board games--------" << endl;
    cout << "1. Default\n";
    cout << "2. Display a list of games that can be played by a given number of players\n";
    cout << "3. Search game by name or id\n";
    cout << "0. EXIT\n";
}

void recommendGameMenu() {
    cout << "\n--------Recommend Games--------" << endl;
    cout << "1. Recommend games\n";
    cout << "0. Exit\n";
}

void searchGameByNameOrId(List<Game>& games, HashTable<string, List<Game>::NodePtr>& gameTable) {
    cout << "Enter game name or ID to search: ";
    string input;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, input);

    if (!gameTable.containsKey(input)) {
        cout << "Game not found. \n";
        return;
    }

    List<Game>::NodePtr gamePtr = gameTable.get(input);
    Game& g = games.getItem(gamePtr);
    cout << "\n--- Game Details ---\n";
    cout << "ID: " << g.getGameId() << "\n";
    cout << "Name: " << g.getName() << "\n";
    cout << "Players: " << g.getMinPlayer() << " - " << g.getMaxPlayer() << "\n";
    cout << "Play Time: " << g.getMinPlayTime() << " - " << g.getMaxPlayTime() << " minutes\n";
    cout << "Year Published: " << g.getYearPublished() << "\n";
    cout << "Status: " << (g.isBorrowed() ? "BORROWED" : "AVAILABLE") << "\n";
    cout << "Active: " << g.getIsActive() << "\n";
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
                cout << "  ID: " << g.getGameId() << "\n";
                cout << "  Name: " << g.getName() << "\n";
                cout << "  Players: " << g.getMinPlayer() << " - " << g.getMaxPlayer() << " players\n";
                cout << "  Play Time: " << g.getMinPlayTime() << " - " << g.getMaxPlayTime() << " minutes\n";
                cout << "  Year Published: " << g.getYearPublished() << "\n";
                cout << "  Current Status: " << (g.isBorrowed() ? "BORROWED" : "AVAILABLE") << "\n";
                if (record.action == "BORROW") {
                    cout << "Borrow Date: " << record.borrowDate << "\n";
                }
                else if (record.action == "RETURN") {
                    cout << "Return Date: " << record.returnDate << "\n";
                }
            } else {
                cout << "Game ID/Name: " << record.gameId << "\n";
                cout << "  Game details not found (may have been removed)\n";
            }
        }
    }
    cout << "========================================================\n";
}

static filesystem::path getDataCSVPath(const string& fileName) {
    return filesystem::path(__FILE__).parent_path() / "data" / fileName;
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
    auto path = getDataCSVPath(filename);
    ifstream file(path);
    if (!file.is_open()) {
        cout << "Failed to open file: " << path << endl;
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
        if (cols.size() != 8) {
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
        string isActive = trimCR(cols[7]);

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
                isActive
            );
            List<Game>::NodePtr gamePtr = games.add(g);
            // index by BOTH id and name
            gameTable.add(gameId, gamePtr);   // for return / ID lookups
            gameTable.add(name, gamePtr);     // for borrow-by-name
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
    auto path = getDataCSVPath(filename);
    ifstream file(path);
    if (!file.is_open()) {
        cout << "Failed to open file: " << path << endl;
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

bool loadRatingsFromCSV(const string& filename, List<Rating>& ratings, HashTable<string, List<Rating>::NodePtr>& ratingTable,
    HashTable<string, List<List<Rating>::NodePtr>*>& gameRatings, HashTable<string, List<List<Rating>::NodePtr>*>& memberRatings) {
    auto path = getDataCSVPath(filename);
    ifstream file(path);
    if (!file.is_open()) {
        cout << "Failed to open file: " << path << endl;
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
        if (cols.size() != 8) {
            cout << "Bad column count (" << cols.size() << "): [" << line << "]\n";
            continue;
        }

        string id = cols[0];
        string gameId = cols[1];
        string gameName = stripOuterQuotes(cols[2]);
        string userId = cols[3];
        string username = stripOuterQuotes(cols[4]);
        string rating = cols[5];
        string review = stripOuterQuotes(cols[6]);
        string createdAt = trimCR(stripOuterQuotes(cols[7]));

        
        // Create object and add to list
        try {
            Rating r(
                stoi(id),
                stoi(gameId),
                gameName,
                stoi(userId),
                username,
                stoi(rating),
                review,
                createdAt
            );
            List<Rating>::NodePtr ratingPtr = ratings.add(r);
            ratingTable.addOrUpdate(id, ratingPtr);

            // gameId -> List<RatingNodePtr>*
            List<List<Rating>::NodePtr>* gameListPtr = nullptr;
            if (gameRatings.containsKey(gameId)) {
                gameListPtr = gameRatings.get(gameId);
            }
            else {
                gameListPtr = new List<List<Rating>::NodePtr>();
                gameRatings.addOrUpdate(gameId, gameListPtr);
            }
            gameListPtr->add(ratingPtr);

            // userId -> List<RatingNodePtr>*
            List<List<Rating>::NodePtr>* memberListPtr = nullptr;
            if (memberRatings.containsKey(userId)) {
                memberListPtr = memberRatings.get(userId);
            }
            else {
                memberListPtr = new List<List<Rating>::NodePtr>();
                memberRatings.addOrUpdate(userId, memberListPtr);
            }
            memberListPtr->add(ratingPtr);
        }
        catch (const exception& e) {
            cout << "Parse error: " << e.what() << "\n";
            cout << "Bad line: [" << line << "]\n";
        }
    }
    file.close();

    /*printAllRatings(ratings);
    printGameRatings(gameRatings, ratings);*/
    // printMemberRatings(memberRatings, ratings);
    return true;
}

int main()
{
    initializeBorrowRecordsCSV();
  // Initialize borrow record counter from existing records
    borrowRecordCounter = getNextBorrowRecordId();  

    // ---- Game ----
    List<Game> games;
    HashTable<string, List<Game>::NodePtr> gameTable;
    loadGamesFromCSV("games.csv", games, gameTable);
    
    // Restore borrowed states from borrow records
    restoreGameBorrowStates(games);

    // ---- Users ----
    List<Admin> admins;
    List<Member> members;
    List<User> users;

    HashTable<string, List<Admin>::NodePtr> adminTable;
    HashTable<string, List<Member>::NodePtr> memberTable;
    HashTable<string, List<User>::NodePtr> userTable;
    loadUsersFromCSV("users.csv", admins, members, users, adminTable, memberTable, userTable);

    // ---- Ratings ----
    List<Rating> ratings;
    HashTable<string, List<Rating>::NodePtr> ratingTable;
    HashTable<string, List<List<Rating>::NodePtr>*> gameRatings;
    HashTable<string, List<List<Rating>::NodePtr>*> memberRatings;
    loadRatingsFromCSV("ratings.csv", ratings, ratingTable, gameRatings, memberRatings);

    // ---- GameBorrowStat ----
    HashTable<string, GameBorrowStat> stats;

    //cout << "\n=== DEBUG TEST A: VERIFY MEMBER INDEX ===\n";
    //debugVerifyMemberIndex(ratings, memberRatings);

    //cout << "\n=== DEBUG TEST B: VALIDATE MEMBER POINTERS ===\n";
    //debugValidateMemberPointers(ratings, memberRatings);

    //printCwdAndFile("ratings.csv");
    //printCwdAndFile("games.csv");
    
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

                else if (adminOption == 4) {
                    cout << "----Display Summary of Games borrowed/returned----\n";
                    cout << "1. Display Overall Summary\n";
                    cout << "2. Display Summary for a Game\n";

                    int summaryOption = -1;
                    cout << "Enter your option: ";
                    cin >> summaryOption;
                    cout << endl;
                    
                    if (summaryOption == 1)
                        displayOverallBorrowSummary();

                    else if (summaryOption == 2) {
                        cout << "Enter Game ID: ";
                        string gameId;
                        cin >> gameId;
                        cout << endl;

                        displayGameBorrowSummary(gameId, games);
                    }

                    else
                        break;
                }
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

            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            // member menu
            while (true) {
                cout << "\nMember Menu\n";
                cout << "1) Borrow a game by name\n";
                cout << "2) Return a game by id\n";
                cout << "3) View my borrowed/history\n";
				cout << "4) Write a review for a game\n";
				cout << "5) View reviews for a game\n";
                cout << "0) Logout\n";
                cout << "Select option: ";
                string mopt;
                if (!getline(cin, mopt)) { mopt = "0"; }

                if (mopt == "1") {
                    cout << "Enter game name to borrow: ";
                    string gameName;
                    getline(cin, gameName);
                    if (loggedInMember->item.borrowGame(games, gameTable, gameName)) {
                        // Get game directly from hash table
                        if (!gameTable.containsKey(gameName)) {
                            cout << "Borrow failed.\n";
                            continue;
                        }

                        auto node = gameTable.get(gameName);
                        int gameId = games.getItem(node).getGameId();

                        writeBorrowRecord(
                            borrowRecordCounter++,
                            to_string(loggedInMember->item.getUserId()),
                            to_string(gameId),
                            "BORROW",
                            getCurrentDate(),
                            ""
                        );
                    }
                    else {
                        cout << "Borrow failed (no available copy or error).\n";
                    }
                }
                else if (mopt == "2") {
                    cout << "Enter game id to return: ";
                    string idStr;
                    getline(cin, idStr);
                    try {
                        int id = stoi(idStr);
                        if (loggedInMember->item.returnGame(games, gameTable, id)) {
                            cout << "Return successful.\n";

                            writeBorrowRecord(
                                borrowRecordCounter++,
                                to_string(loggedInMember->item.getUserId()),
                                to_string(id),
                                "RETURN",
                                "",                 // borrowDate
                                getCurrentDate()    // returnDate
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
                else if (mopt == "4") {
                    writeReview(ratings, gameRatings, members, games, loggedInMember->item.getUserId());
                }
                else if (mopt == "5") {
                    displayReviewsForGame(ratings, gameRatings, games);
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
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    globalUser.printActiveGames(games);
                }

                else if (displayOption == 2) {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    globalUser.displayGamesPlayableByNPlayers(games);
                }

                else if (displayOption == 3)
                    searchGameByNameOrId(games, gameTable); 

                else
                    cout << "Invalid option!\n";
            }
        }

        else if (option == 4) {
            int recommendOption = -1;
            while (recommendOption != 0) {
                recommendGameMenu();
                cout << "Enter your option: ";
                cin >> recommendOption;
                cout << endl;

                if (recommendOption == 0) {
                    cout << "Exiting to main menu...\n";
                }

                else if (recommendOption == 1) {
                    cout << "Enter a game ID to see similar recommended games: ";
                    int gameId;

                    if (!(cin >> gameId)) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid game ID.\n";
                        continue;
                    }


                    List<GameCandidate> candidates;
                    globalUser.recommendFromGame(gameId, ratings,candidates, gameRatings, memberRatings, games, gameTable);
                    
                    if (candidates.getLength() <= 0) {
                        cout << "No user recommend this game yet... Be the first to recommend it\n";
                        continue;
                    }

                    globalUser.printRecommendedGames(candidates, games, gameTable);

                }
                else {
                    cout << "Invalid option!\n";
                }
            }
        }
        else
            cout << "Invalid option!\n";
    }
}

// -------- DEBUG --------
void debugVerifyMemberIndex(
    List<Rating>& ratings,
    HashTable<string, List<List<Rating>::NodePtr>*>& memberRatings
) {
    int totalRatings = ratings.getLength();
    int missing = 0;

    for (auto n = ratings.getNode(0); n != nullptr; n = n->next) {
        Rating& r = n->item;
        string userKey = to_string(r.getUserId());

        if (!memberRatings.containsKey(userKey)) {
            cout << "[MISSING KEY] userId=" << userKey
                << " ratingId=" << r.getId()
                << " gameId=" << r.getGameId() << "\n";
            missing++;
            continue;
        }

        auto listPtr = memberRatings.get(userKey);
        if (listPtr == nullptr) {
            cout << "[NULL LIST] userId=" << userKey << "\n";
            missing++;
        }
    }

    cout << "\nMaster ratings: " << totalRatings
        << " | Missing member index entries: " << missing << "\n";
}

void debugValidateMemberPointers(
    List<Rating>& ratings,
    HashTable<string, List<List<Rating>::NodePtr>*>& memberRatings
) {
    int bad = 0;

    memberRatings.forEach([&](const string& userId, List<List<Rating>::NodePtr>* ptrList) {
        if (!ptrList) {
            cout << "[NULL PTRLIST] userId=" << userId << "\n";
            bad++;
            return;
        }

        for (auto n = ptrList->getNode(0); n != nullptr; n = n->next) {
            auto ratingPtr = n->item; // List<Rating>::NodePtr

            // Safest: try to access it
            try {
                Rating& r = ratings.getItem(ratingPtr);
                // sanity: userId should match
                if (to_string(r.getUserId()) != userId) {
                    cout << "[MISMATCH] key userId=" << userId
                        << " but rating.userId=" << r.getUserId()
                        << " ratingId=" << r.getId() << "\n";
                    bad++;
                }
            }
            catch (...) {
                cout << "[BAD PTR] userId=" << userId << "\n";
                bad++;
            }
        }
        });

    cout << "\nBad pointer/mismatch count: " << bad << "\n";
}

void printAllRatings(List<Rating>& ratings) {
    cout << "\n=== ALL RATINGS (Master List) ===\n";
    int index = 0;
    for (auto node = ratings.getNode(0); node != nullptr; node = node->next) {
        cout << node->item << endl;
    }
}

void printGameRatings(
    HashTable<string, List<List<Rating>::NodePtr>*>& gameRatings,
    List<Rating>& ratings
) {
    cout << "\n=== GAME RATINGS INDEX ===\n";

    gameRatings.forEach([&](const string& gameId, List<List<Rating>::NodePtr>* ratingPtrs) {
        cout << "\nGame ID: " << gameId << "\n";

        if (ratingPtrs == nullptr) {
            cout << "  (null list)\n";
            return;
        }

        int i = 0;
        for (auto n = ratingPtrs->getNode(0); n != nullptr; n = n->next) {
            auto ratingNodePtr = n->item;          // List<Rating>::NodePtr
            Rating& r = ratings.getItem(ratingNodePtr);
            cout << "  [" << i++ << "] " << r << "\n";
        }
        });
}

void printMemberRatings(
    HashTable<string, List<List<Rating>::NodePtr>*>& memberRatings,
    List<Rating>& ratings
) {
    cout << "\n=== MEMBER RATINGS INDEX ===\n";

    memberRatings.forEach([&](const string& userId, List<List<Rating>::NodePtr>* ratingPtrs) {
        cout << "\nUser ID: " << userId << "\n";

        if (ratingPtrs == nullptr) {
            cout << "  (null list)\n";
            return;
        }

        int i = 0;
        for (auto n = ratingPtrs->getNode(0); n != nullptr; n = n->next) {
            auto ratingNodePtr = n->item;
            Rating& r = ratings.getItem(ratingNodePtr);
            cout << "  [" << i++ << "] " << r << "\n";
        }
        });
}


void printCwdAndFile(const string& filename) {
    namespace fs = std::filesystem;
    cout << "[CWD] " << fs::current_path() << "\n";
    cout << "[FILE] " << fs::absolute(filename) << "\n";
    if (fs::exists(filename))
        cout << "[SIZE] " << fs::file_size(filename) << " bytes\n";
    else
        cout << "[MISSING] file not found\n";
}
