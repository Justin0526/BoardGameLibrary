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

void displayOverallBorrowSummary() {
    HashTable<string, GameBorrowStat> stats;
    int totalBorrows;
    int totalReturns;

    if (!buildBorrowStatsFromCSV(stats, totalBorrows, totalReturns)) {
        cout << "ERROR: Cannot open borrow records.\n";
        return;
    }

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