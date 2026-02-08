/*********************************************************************************
 * Group         : T01
 * Team Member   : Justin Tang Jia Ze (S10269496B)
 *
 * File Purpose:
 * - Implements Admin features for managing games and members.
 *
 * Key Features:
 * - Add game: validates inputs, appends to in-memory list + gameTable index, and
 *   persists the new record into games.csv.
 * - Remove game: performs soft-delete (isActive="FALSE") and rewrites games.csv.
 * - Add member: creates new User + Member records, updates hash indexes, and
 *   persists into users.csv.
 *
 * Notes:
 * - CSV escaping is used to safely store names that may contain quotes/commas.
 *********************************************************************************/

#include "Admin.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>
#include <filesystem>

using namespace std;

static filesystem::path getDataCSVPath(const string& fileName) {
    return filesystem::path(__FILE__).parent_path() / "data" / fileName;
}

Admin::Admin() : User() {}

Admin::Admin(int id, string name)
    : User(id, name, "admin") {
}

// Helper: Validates that a name contains only alphabets and spaces.
// Used for member name input sanitisation.
bool checkInputCharacter(string s) {
    bool valid = true;
    for (char c : s) {
        if (!isalpha(c) && c != ' ') {
            valid = false;
            break;
        }
    }
    return valid;
}

// Helper: Returns the current year (used to validate yearPublished input).
int currentYear() {
    time_t now = time(nullptr);
    tm local{};
    localtime_s(&local, &now);
    return local.tm_year + 1900;
}

// Helper: Checks whether a string contains only alphabets and spaces.
// Returns false if empty.
bool isAlphabetOnly(const string& s) {
    if (s.empty())
        return false;

    for (char c : s) {
        if (!isalpha(static_cast<unsigned char>(c)) && c != ' ')
            return false; // allow space but not special characters
    }

    return true;
}

// Helper: Escapes a string for CSV output.
// - Doubles any existing quotes ( " -> "" )
// - Wraps the final value in quotes
// This prevents commas/quotes in names from breaking the CSV format.
string csvEscape(const string& s) {
    string out = s;
    size_t pos = 0;
    while ((pos = out.find('"', pos)) != string::npos) {
        out.insert(pos, "\"");   // double the quote
        pos += 2;
    }
    return "\"" + out + "\"";    // wrap in quotes
}

/*********************************************************************************
 * Function  : Admin::addGame
 * Purpose   : Allows an admin to add a new game into the system.
 *
 * Effects   :
 * - Adds Game object into the games list (in-memory)
 * - Adds index entry into gameTable (fast lookup)
 * - Appends a new row into games.csv (persistent storage)
 *
 * Notes     :
 * - Inputs are validated for numeric ranges (players, playtime, year).
 * - Uses csvEscape() to safely write the game name into CSV.
 *********************************************************************************/
void Admin::addGame(List<Game>& games, HashTable<string, List<Game>::NodePtr>& gameTable) {
    // Allow admin to fill in fields
    cout << "----Add a game----\n";

    // Game name
    string name;
    cout << "Name of Game: ";

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
            cout << "Invalid input. Please enter a whole number that's >= " << minPlayer << " (minimum player)\n";
            cout << "Maximum player(s): ";
        }
        else {
            break;
        }
    }

    // min play time
    int minPlayTime;
    cout << "Minimum playtime (mins): ";

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
    cout << "Maximum playtime (mins): ";
    
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
    int current = currentYear();
    cout << "Year published: ";
    while (true) {
        getline(cin, input);
        stringstream ss(input);
        if (!(ss >> yearPublished) || !ss.eof()) {
            cout << "Invalid input! Please enter  a whole number!\n";
        }
        else if(yearPublished > current || yearPublished <= 0) {
            cout << "Invalid year!\n";
        }
        else {
            break;
        }

        cout << "Year published: ";
    }
    Game lastGame = games.get(games.getLength() - 1);
    int newGameId = lastGame.getGameId() + 1;
    string isActive = "TRUE";

    Game g(newGameId + 1, name, minPlayer, maxPlayer, minPlayTime, maxPlayTime, yearPublished, isActive);
    
    // Insert into in-memory list and update hash index for fast future lookups.
    List<Game>::NodePtr gamePtr = games.add(g);
    gameTable.add(to_string(newGameId), gamePtr);

    // fstream - include read (ios::in), write(ios::out), append (ios::app) modes
    // ofstream - write only (will overwrite the file if it already exists!)
    // ifstream - read only
    auto path = getDataCSVPath("games.csv");

    ofstream file(path, ios::app);
    if (!file.is_open()) {
        cout << "Failed to open file for append: " << path << endl;
        return;
    }

    file
        << newGameId << ","
        << csvEscape(name) << ","
        << minPlayer << ","
        << maxPlayer << ","
        << minPlayTime << ","
        << maxPlayTime << ","
        << yearPublished << ","
        << "TRUE" << "\n";

    file.close();


    cout << "Game added successfully! (" << name << ")\n";
}

/*********************************************************************************
 * Function  : Admin::removeGame
 * Purpose   : Soft-deletes a game by setting isActive = "FALSE".
 *
 * Effects   :
 * - Updates the in-memory Game node (isActive flag)
 * - Rewrites games.csv to reflect the updated active status
 *
 * Notes     :
 * - Soft-delete keeps the record for history/audit while hiding it from active views.
 *********************************************************************************/
void Admin::removeGame(List<Game>& games, HashTable<string, List<Game>::NodePtr>& gameTable) {
    cout << "---- Remove a game ----\n";

    string gameId;
    cout << "Enter GameID: ";

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    getline(cin, gameId);
    
    if (!gameTable.containsKey(gameId)) {
        cout << "Invalid ID!\n";
        return;
    }

    List<Game>::NodePtr gamePtr = gameTable.get(gameId);
    if (gamePtr != nullptr && gamePtr->item.getIsActive() != "TRUE") {
        for (List<Game>::NodePtr node = games.getNode(0); node != nullptr; node = node->next) {
            if (to_string(node->item.getGameId()) == gameId && node->item.getIsActive() == "TRUE") {
                gamePtr = node;
                break;
            }
        }
    }

    if (gamePtr != nullptr && gamePtr->item.getIsActive() == "TRUE") {
        cout << "Deleting " << gamePtr->item.getName() << "...\n";
        
        gamePtr->item.setIsActive("FALSE");

        // Rewrite the entire CSV file so the updated isActive status is persisted.
        auto path = getDataCSVPath("games.csv");
        ofstream file(path, ios::trunc);
        if (!file.is_open()) {
            cout << "Failed to open file for rewrite: " << path << endl;
            return;
        }

        //write header 
        file << "gameId,name,minPlayer,maxPlayer,minPlayTime,maxPlaytime,yearPublished,isActive\n";

        for (auto node = games.getNode(0); node != nullptr; node = node->next) {
            const Game& g = node->item;
            file << g.getGameId() << ","
                << csvEscape(g.getName()) << ","
                << g.getMinPlayer() << ","
                << g.getMaxPlayer() << ","
                << g.getMinPlayTime() << ","
                << g.getMaxPlayTime() << ","
                << g.getYearPublished() << ","
                << g.getIsActive() << "\n";
        }
        file.close();

        cout << "Successfully deleted " << gamePtr->item.getName() << " (ID " << gameId << ")\n";
    }
    else {
        cout << "Invalid game ID! Please enter a valid game ID.\n";
    }
}

/*********************************************************************************
 * Function  : Admin::addMember
 * Purpose   : Adds a new member into the system (User + Member records).
 *
 * Effects   :
 * - Inserts a new User (role = "member") into users list and userTable index
 * - Inserts a new Member into members list and memberTable index
 * - Appends a new record into users.csv for persistence
 *
 * Notes     :
 * - Member name is validated to contain only alphabets and spaces.
 * - Uses csvEscape() to safely store the member name in CSV.
 *********************************************************************************/
void Admin::addMember(List<Member>& members, HashTable<string, List<Member>::NodePtr>& memberTable, 
    List<User>& users, HashTable<string, List<User>::NodePtr>& userTable) {
    cout << "---- Add a new member ----\n" << endl;

    string name;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    while (true) {
        cout << "Name of member: ";
        getline(cin, name);
        // Validate member name: letters/spaces only, not empty.
        bool validName = checkInputCharacter(name);

        if (name.empty()) {
            cout << "Name cannot be empty!\n";
        }
        else if (!validName) {
            cout << "Name must contain letters only.\n";
        }
        else {
            break;
        }
    }

    int id = users.getLength();
    User newUser(id + 1, name, "member");
    List<User>::NodePtr userPtr = users.add(newUser);
    userTable.add(name, userPtr);

    Member newMember(id+1, name);
    List<Member>::NodePtr memberPtr = members.add(newMember);
    memberTable.add(name, memberPtr);

    auto path = getDataCSVPath("users.csv");
    ofstream file(path, ios::app);

    if (!file.is_open()) {
        cout << "Failed to open users.csv for append: " << path << endl;
        return;
    }

    file << id + 1 << ","
        << csvEscape(name) << ","
        << "member\n";

    file.close();

    cout << "Adding new member...\n";
    cout << name << " added successfully!\n";
}
