#include "Admin.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>

using namespace std;

Admin::Admin() : User() {}

Admin::Admin(int id, string name)
    : User(id, name, "admin") {
}

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

int currentYear() {
    time_t now = time(nullptr);
    tm local{};
    localtime_s(&local, &now);
    return local.tm_year + 1900;
}

bool isAlphabetOnly(const string& s) {
    if (s.empty())
        return false;

    for (char c : s) {
        if (!isalpha(static_cast<unsigned char>(c)) && c != ' ')
            return false; // allow space but not special characters
    }

    return true;
}

string csvEscape(const string& s) {
    string out = s;
    size_t pos = 0;
    while ((pos = out.find('"', pos)) != string::npos) {
        out.insert(pos, "\"");   // double the quote
        pos += 2;
    }
    return "\"" + out + "\"";    // wrap in quotes
}

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

    List<Game>::NodePtr gamePtr = gameTable.get(name);

    int copies = 1;
    if (gamePtr != nullptr && gamePtr->item.getIsActive() == "TRUE") {

        string option;
        cout << "You already have " << gamePtr->item.getGameCopy() << " copies\n";
        cout << "Do you want to add one more? (Y/N): ";

        // Validation loop for copies option (accepts lowercase/uppercase)
        while (true) {
            getline(cin, option);

            // Trim to first non-space character (basic trim)
            size_t first = option.find_first_not_of(" \t\r\n");
            if (first == string::npos) {
                cout << "Invalid option! Please enter Y or N: ";
                continue;
            }

            char c = option[first];
            c = static_cast<char>(toupper(static_cast<unsigned char>(c)));

            if (c == 'Y') {
                copies = gamePtr->item.getGameCopy() + 1;
                gamePtr->item.setGameCopy(copies);

                // overwrite
                ofstream file("games.csv", ios::trunc);

                //write header 
                file << "gameId,name,minPlayer,maxPlayer,minPlayTime,maxPlaytime,yearPublished,copy,isActive\n";
                for (List<Game>::NodePtr node = games.getNode(0); node != nullptr; node = node->next) {
                    const Game& g = node->item;
                    file << g.getGameId() << ","
                        << csvEscape(g.getName()) << ","
                        << g.getMinPlayer() << ","
                        << g.getMaxPlayer() << ","
                        << g.getMinPlayTime() << ","
                        << g.getMaxPlayTime() << ","
                        << g.getYearPublished() << ","
                        << g.getGameCopy() << ","
                        << g.getIsActive()
                        << "\n";
                }
                cout << "Copies updated successfully! Now you have " << copies << " copies.\n";
                file.close();
                break;
            }
            else if (c == 'N') {
                cout << "Not adding game. Exiting...\n";
                break;
            }
            else {
                cout << "Invalid option! Please enter Y or N: ";
            }
        }

    }
    else {
        Game lastGame = games.get(games.getLength() - 1);
        int lastGameId = lastGame.getGameId();
        string isActive = "TRUE";

        Game g(lastGameId + 1, name, minPlayer, maxPlayer, minPlayTime, maxPlayTime, yearPublished, copies, isActive);
        List<Game>::NodePtr gamePtr = games.add(g);
        gameTable.add(name, gamePtr);

        // fstream - include read (ios::in), write(ios::out), append (ios::app) modes
        // ofstream - write only (will overwrite the file if it already exists!)
        // ifstream - read only
        fstream file;
        file.open("games.csv", ios::app);
        file 
            << lastGameId + 1 << ","
            << csvEscape(name) << ','
            << minPlayer << ","
            << maxPlayer << ","
            << minPlayTime << ","
            << maxPlayTime << ","
            << yearPublished << ","
            << copies << ","
            << "TRUE";

        file.close();

        cout << "Game added successfully! (" << name << ")\n";
    }
}

void Admin::removeGame(List<Game>& games, HashTable<string, List<Game>::NodePtr>& gameTable) {
    cout << "---- Remove a game ----\n";

    string name;
    cout << "Name of game: ";

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    getline(cin, name);
    List<Game>::NodePtr gamePtr = gameTable.get(name);
    if (gamePtr != nullptr && gamePtr->item.getIsActive() != "TRUE") {
        for (List<Game>::NodePtr node = games.getNode(0); node != nullptr; node = node->next) {
            if (node->item.getName() == name && node->item.getIsActive() == "TRUE") {
                gamePtr = node;
                break;
            }
        }
    }

    if (gamePtr != nullptr && gamePtr->item.getIsActive() == "TRUE") {
        cout << "Deleting " << gamePtr->item.getName() << "...\n";
        
        gamePtr->item.setIsActive("FALSE");
        // overwrite
        ofstream file("games.csv", ios::trunc);

        //write header 
        file << "gameId,name,minPlayer,maxPlayTime,minPlayTime,maxPlaytime,yearPublished,copy,isActive\n";

        for (List<Game>::NodePtr node = games.getNode(0); node != nullptr; node = node->next) {
            const Game& g = node->item;
            if (gamePtr->item.getGameId() == node->item.getGameId()) {
                file << g.getGameId() << ","
                    << csvEscape(g.getName()) << ","
                    << g.getMinPlayer() << ","
                    << g.getMaxPlayer() << ","
                    << g.getMinPlayTime() << ","
                    << g.getMaxPlayTime() << ","
                    << g.getYearPublished() << ","
                    << g.getGameCopy() << ","
                    << "FALSE"
                    << "\n";
            }
            else {
                file << g.getGameId() << ","
                    << csvEscape(g.getName()) << ","
                    << g.getMinPlayer() << ","
                    << g.getMaxPlayer() << ","
                    << g.getMinPlayTime() << ","
                    << g.getMaxPlayTime() << ","
                    << g.getYearPublished() << ","
                    << g.getGameCopy() << ","
                    << g.getIsActive()
                    << "\n";
            }
        }
        file.close();

        cout << "Successfully deleted " << name << endl;
    }
    else {
        cout << "Invalid game name! Please enter a valid game name\n";
    }
}

void Admin::addMember(List<Member>& members, HashTable<string, List<Member>::NodePtr>& memberTable, 
    List<User>& users, HashTable<string, List<User>::NodePtr>& userTable) {
    cout << "---- Add a new member ----\n" << endl;

    string name;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    while (true) {
        cout << "Name of member: ";
        getline(cin, name);
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

    fstream file;
    file.open("users.csv", ios::app);
    file << id + 1 << ","
        << csvEscape(name) << ','
        << "member";

    file.close();

    cout << "Adding new member...\n";
    cout << name << "added successfully!\n";
}

void Admin::displayGameBorrowReturnSummary() {
    cout << "Displaying overall borrow/return summary" << endl;
}
