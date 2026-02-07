#include "BorrowRecord.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>
#include <sstream>
#include <map>
#include <chrono>
#include <iomanip>
#include "Game.h"
#include "LinkedList.h"
#include "HashTable.h"

using namespace std;

// Always resolve the SAME borrow_records.csv
static std::filesystem::path getBorrowCSVPath() {
    return std::filesystem::path(__FILE__).parent_path() / "data" / "borrow_records.csv";
}

std::string getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
    return buf;
}

void initializeBorrowRecordsCSV() {
    auto path = getBorrowCSVPath();

    if (!std::filesystem::exists(path)) {
        std::ofstream file(path);
        file << "recordId,memberId,gameId,action,borrowDate,returnDate\n";
        file.close();
    }
}

void writeBorrowRecord(
    int recordId,
    const std::string& memberId,
    const std::string& gameId,
    const std::string& action,
    const std::string& borrowDate,
    const std::string& returnDate
) {
    auto path = getBorrowCSVPath();

    std::ofstream file(path, std::ios::app);
    if (!file.is_open()) {
        std::cout << "ERROR: Cannot open file: " << path << std::endl;
        return;
    }

    file << recordId << ","
        << memberId << ","
        << gameId << ","
        << action << ","
        << borrowDate << ","
        << returnDate << "\n";

    file.close();
}

int getNextBorrowRecordId() {
    auto path = getBorrowCSVPath();

    std::ifstream file(path);
    if (!file.is_open()) return 1;

    int maxId = 0;
    std::string line;

    std::getline(file, line); // header
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        size_t comma = line.find(',');
        if (comma == std::string::npos) continue;

        try {
            int id = std::stoi(line.substr(0, comma));
            maxId = std::max(maxId, id);
        }
        catch (...) {}
    }

    return maxId + 1;
}

void loadMemberBorrowHistoryDetailed(
    int memberId,
    List<Game>& games,
    std::vector<BorrowHistoryRecord>& borrowHistory
) {
    auto path = getBorrowCSVPath();
    borrowHistory.clear();

    std::ifstream file(path);
    if (!file.is_open()) return;

    std::string line;
    std::getline(file, line); // header

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        BorrowHistoryRecord rec;
        std::string recordId, memberIdStr;

        std::getline(ss, recordId, ',');
        std::getline(ss, memberIdStr, ',');
        std::getline(ss, rec.gameId, ',');
        std::getline(ss, rec.action, ',');
        std::getline(ss, rec.borrowDate, ',');
        std::getline(ss, rec.returnDate);

        try {
            if (std::stoi(memberIdStr) != memberId) continue;
        }
        catch (...) { continue; }

        rec.gameDetails = nullptr;

        int n = games.getLength();
        for (int i = 0; i < n; ++i) {
            auto node = games.getNode(i);
            if (!node) continue;
            Game& g = games.getItem(node);

            if (std::to_string(g.getGameId()) == rec.gameId ||
                g.getName() == rec.gameId) {
                rec.gameDetails = &g;
                break;
            }
        }

        borrowHistory.push_back(rec);
    }
}

void restoreGameBorrowStates(List<Game>& games) {
    auto path = getBorrowCSVPath();
    std::ifstream file(path);
    if (!file.is_open()) return;

    std::map<int, bool> lastState;

    std::string line;
    std::getline(file, line); // header

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string recordId, memberId, gameId, action, borrowDate, returnDate;

        std::getline(ss, recordId, ',');
        std::getline(ss, memberId, ',');
        std::getline(ss, gameId, ',');
        std::getline(ss, action, ',');
        std::getline(ss, borrowDate, ',');
        std::getline(ss, returnDate);

        try {
            int gid = std::stoi(gameId);
            lastState[gid] = (action == "BORROW");
        }
        catch (...) {}
    }

    int n = games.getLength();
    for (int i = 0; i < n; ++i) {
        auto node = games.getNode(i);
        if (!node) continue;

        Game& g = games.getItem(node);
        int id = g.getGameId();

        if (lastState.count(id)) {
            g.setBorrowed(lastState[id]);
        }
    }
}

// Justin
// Parses a single CSV row into columns while supporting:
// - quoted fields ("...") that may contain commas
// - escaped quotes ("") inside quoted fields
// Returns a vector of column strings in the same order as the CSV.
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

// Justin
/*********************************************************************************
 * Function  : buildBorrowStatsFromCSV
 * Purpose   : Reads borrow_records.csv and aggregates per-game statistics into a
 *             HashTable keyed by gameId.
 *
 * Params    :
 * - stats        : Output table (gameId -> GameBorrowStat) to be filled/updated
 * - totalBorrows : Output count of BORROW actions across all records
 * - totalReturns : Output count of RETURN actions across all records
 *
 * Returns   :
 * - bool : true if file was read successfully, false if file could not be opened
 *
 * Notes     :
 * - Uses separate chaining HashTable for fast updates by gameId.
 * - lastBorrowDate tracks the most recent borrowDate seen for each game.
 * - CSV file path resolution is handled by getBorrowCSVPath() (Khaleel).
 *
 * Time Complexity:
 * - O(R) average, where R is number of records (each line processed once).
 *********************************************************************************/
bool buildBorrowStatsFromCSV(HashTable<string, GameBorrowStat>& stats, int& totalBorrows, int& totalReturns) {
    totalBorrows = 0;
    totalReturns = 0;

    auto path = getBorrowCSVPath();

    ifstream file(path);
    if (!file.is_open()) return false;

    string line;
    // 1) Skip header line
    if (!getline(file, line))
        return false;

    while (getline(file, line)) {
        if (line.empty()) continue;

        vector<string> cols = parseCsvLine(line);

        string recordId = cols[0];
        string memberId = cols[1];
        string gameIdStr = cols[2];
        string action = cols[3];
        string borrowDate = cols[4];
        string returnDate = cols[5];

        GameBorrowStat s;
        if (stats.containsKey(gameIdStr)) {
            s = stats.get(gameIdStr);
        }

        if (action == "BORROW") {
            totalBorrows++;
            s.borrowCount++;
            s.lastAction = "BORROW";

            if (!borrowDate.empty() && borrowDate > s.lastBorrowDate) {
                s.lastBorrowDate = borrowDate;
            }
        }
        else if (action == "RETURN") {
            totalReturns++;
            s.returnCount++;
            s.lastAction = "RETURN";
        }

        stats.addOrUpdate(gameIdStr, s);
    }
    return true;
}

// Justin
/*********************************************************************************
 * Function  : displayOverallBorrowSummary
 * Purpose   : Displays an overall borrow/return summary across all games.
 *
 * Output    :
 * - Total borrow records
 * - Total return records
 * - Currently borrowed count (games whose lastAction is BORROW)
 *
 * Notes     :
 * - Uses HashTable::forEach with a lambda to count "currently borrowed".
 *********************************************************************************/
void displayOverallBorrowSummary() {
    HashTable<string, GameBorrowStat> stats;
    int totalBorrows;
    int totalReturns;

    if (!buildBorrowStatsFromCSV(stats, totalBorrows, totalReturns)) {
        cout << "ERROR: Cannot open borrow records.\n";
        return;
    }

    // Count games that are currently considered borrowed based on their latest action.
    int currentlyBorrowed = 0;
    stats.forEach([&](const string&, const GameBorrowStat& s) {
        if (s.lastAction == "BORROW")
            currentlyBorrowed++;
        });

    cout << "\n==== OVERALL SUMMARY ====\n";
    cout << "Total borrow records : " << totalBorrows << "\n";
    cout << "Total return records : " << totalReturns << "\n";
    cout << "Currently borrowed   : " << currentlyBorrowed << "\n";
}

/*********************************************************************************
 * Function  : displayGameBorrowSummary
 * Purpose   : Displays borrow/return statistics for a specific gameId, including
 *             game name and active status looked up from the Game list.
 *
 * Params    :
 * - gameId : The target gameId (string form, used as HashTable key)
 * - games  : Game list used to resolve name/status for display
 *
 * Notes     :
 * - Stats are computed by reading borrow_records.csv and aggregating by gameId.
 * - Game name/status is obtained by scanning games to match stoi(gameId).
 *********************************************************************************/
void displayGameBorrowSummary(string gameId, List<Game>& games) {
    HashTable<string, GameBorrowStat> stats;
    int totalBorrows;
    int totalReturns;

    if (!buildBorrowStatsFromCSV(stats, totalBorrows, totalReturns)) {
        cout << "ERROR: Cannot open borrow records.\n";
        return;
    }

    if (!stats.containsKey(gameId)) {
        cout << "No borrow/return records found for gameId " << gameId << endl;
        return;
    }

    GameBorrowStat s = stats.get(gameId);

    bool isActive = false;
    string gameName = "(unknown)";

    for (auto n = games.getNode(0); n != nullptr; n = n->next) {
        Game& g = games.getItem(n);
        if (g.getGameId() == stoi(gameId)) {
            gameName = g.getName();
            isActive = (g.getIsActive() == "TRUE");
            break;
        }
    }

    std::cout << "\n===== GAME SUMMARY =====\n";
    std::cout << "Game ID          : " << gameId << "\n";
    std::cout << "Name             : " << gameName << "\n";
    std::cout << "Borrows          : " << s.borrowCount << "\n";
    std::cout << "Returns          : " << s.returnCount << "\n";
    std::cout << "Last Borrow Date : " << (s.lastBorrowDate.empty() ? "-" : s.lastBorrowDate) << "\n";
    std::cout << "Status           : " << (isActive ? "Active" : "Not Active") << "\n";
}