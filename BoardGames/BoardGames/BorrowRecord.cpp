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

void writeBorrowRecord(
    int recordId,
    const std::string& memberId,
    const std::string& gameId,
    const std::string& action,
    const std::string& borrowDate,
    const std::string& returnDate
) {
    namespace fs = std::filesystem;
    fs::path sourcePath = fs::path(__FILE__);
    fs::path projectRoot = sourcePath.parent_path();
    fs::path projectPath = projectRoot / "data/borrow_records.csv";

    std::ofstream file(projectPath, std::ios::app);
    if (!file.is_open()) {
        std::cout << "ERROR: Cannot open file: " << projectPath.string() << std::endl;
        return;
    }
    std::cout << "[BORROW CSV PATH] " << projectPath.string() << std::endl;

    file << recordId << "," << memberId << "," << gameId << "," << action << "," << borrowDate << "," << returnDate << std::endl;

    if (!file) {
        std::cout << "ERROR: Failed to write to file: " << projectPath.string() << std::endl;
    }

    file.close();
}

void initializeBorrowRecordsCSV() {
    namespace fs = std::filesystem;
    fs::path sourcePath = fs::path(__FILE__);
    fs::path projectRoot = sourcePath.parent_path();
    fs::path projectPath = projectRoot / "data/borrow_records.csv";

    std::ifstream checkFile(projectPath);
    bool fileExists = checkFile.good();
    std::string firstLine;
    if (fileExists) {
        std::getline(checkFile, firstLine);
    }
    checkFile.close();

    if (!fileExists || firstLine != "recordId,memberId,gameId,action,borrowDate,returnDate") {
        std::ofstream file(projectPath, std::ios::trunc);
        if (file.is_open()) {
            file << "recordId,memberId,gameId,action,borrowDate,returnDate" << std::endl;
            file.close();
            std::cout << "Initialized borrow_records.csv with header." << std::endl;
        }
    }
}

int getNextBorrowRecordId() {
    namespace fs = std::filesystem;
    fs::path sourcePath = fs::path(__FILE__);
    fs::path projectRoot = sourcePath.parent_path();
    fs::path projectPath = projectRoot / "data/borrow_records.csv";

    std::ifstream file(projectPath);
    if (!file.is_open()) {
        return 1; // Start with 1 if file doesn't exist
    }

    int maxId = 0;
    std::string line;

    // Skip header
    if (std::getline(file, line)) {
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            // Parse first column (recordId)
            size_t commaPos = line.find(',');
            if (commaPos != std::string::npos) {
                try {
                    int id = std::stoi(line.substr(0, commaPos));
                    if (id > maxId) maxId = id;
                } catch (...) {
                    // Skip invalid lines
                }
            }
        }
    }

    file.close();
    return maxId + 1;
}

void loadMemberBorrowHistory(int memberId, std::vector<std::string>& borrowHistory) {
    namespace fs = std::filesystem;
    fs::path sourcePath = fs::path(__FILE__);
    fs::path projectRoot = sourcePath.parent_path();
    fs::path projectPath = projectRoot / "data/borrow_records.csv";

    borrowHistory.clear();

    std::ifstream file(projectPath);
    if (!file.is_open()) {
        return; // No history if file doesn't exist
    }

    std::string line;

    // Skip header
    if (std::getline(file, line)) {
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            // Parse: recordId,memberId,gameId,action,borrowDate,returnDate
            std::istringstream ss(line);
            std::string recordId, memberIdStr, gameId, action, borrowDate, returnDate;

            if (std::getline(ss, recordId, ',') &&
                std::getline(ss, memberIdStr, ',') &&
                std::getline(ss, gameId, ',') &&
                std::getline(ss, action, ',') &&
                std::getline(ss, borrowDate, ',') &&
                std::getline(ss, returnDate)) {

                try {
                    if (std::stoi(memberIdStr) == memberId) {
                        borrowHistory.push_back(action + ": " + gameId);
                    }
                } catch (...) {
                    // Skip invalid lines
                }
            }
        }
    }

    file.close();
}

void loadMemberBorrowHistoryDetailed(int memberId, List<Game>& games, std::vector<BorrowHistoryRecord>& borrowHistory) {
    namespace fs = std::filesystem;
    fs::path sourcePath = fs::path(__FILE__);
    fs::path projectRoot = sourcePath.parent_path();
    fs::path projectPath = projectRoot / "data/borrow_records.csv";

    borrowHistory.clear();

    std::ifstream file(projectPath);
    if (!file.is_open()) {
        return; // No history if file doesn't exist
    }

    std::string line;

    // Skip header
    if (std::getline(file, line)) {
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            // Parse: recordId,memberId,gameId,action,borrowDate,returnDate
            std::istringstream ss(line);
            std::string recordId, memberIdStr, gameId, action, borrowDate, returnDate;

            if (std::getline(ss, recordId, ',') &&
                std::getline(ss, memberIdStr, ',') &&
                std::getline(ss, gameId, ',') &&
                std::getline(ss, action, ',') &&
                std::getline(ss, borrowDate, ',') &&
                std::getline(ss, returnDate)) {

                try {
                    if (std::stoi(memberIdStr) == memberId) {
                        BorrowHistoryRecord record;
                        record.action = action;
                        record.gameId = gameId;
                        record.gameDetails = nullptr;
                        record.borrowDate = borrowDate;
                        record.returnDate = returnDate;

                        // Find the corresponding game object
                        int n = games.getLength();
                        for (int i = 0; i < n; ++i) {
                            auto node = games.getNode(i);
                            if (node == nullptr) continue;
                            Game& g = games.getItem(node);

                            // Check if gameId matches by name or ID
                            if (g.getName() == gameId || std::to_string(g.getGameId()) == gameId) {
                                record.gameDetails = &g;
                                break;
                            }
                        }

                        borrowHistory.push_back(record);
                    }
                } catch (...) {
                    // Skip invalid lines
                }
            }
        }
    }

    file.close();
}

void restoreGameBorrowStates(List<Game>& games) {
    namespace fs = std::filesystem;
    fs::path sourcePath = fs::path(__FILE__);
    fs::path projectRoot = sourcePath.parent_path();
    fs::path projectPath = projectRoot / "data/borrow_records.csv";

    std::ifstream file(projectPath);
    if (!file.is_open()) {
        return; // No records to restore
    }

    // Track currently borrowed games by BOTH name and ID
    std::map<std::string, bool> borrowedByName;
    std::map<int, bool> borrowedById;

    std::string line;
    if (std::getline(file, line)) { // skip header
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::istringstream ss(line);
            std::string recordId, memberIdStr, gameIdStr, action, borrowDate, returnDate;
            if (std::getline(ss, recordId, ',') &&
                std::getline(ss, memberIdStr, ',') &&
                std::getline(ss, gameIdStr, ',') &&
                std::getline(ss, action, ',') &&
                std::getline(ss, borrowDate, ',') &&
                std::getline(ss, returnDate)) {

                // Update borrow state maps
                if (action == "BORROW") {
                    borrowedByName[gameIdStr] = true;
                    try {
                        int gid = std::stoi(gameIdStr);
                        borrowedById[gid] = true;
                    } catch (...) {}
                } else if (action == "RETURN") {
                    borrowedByName[gameIdStr] = false;
                    try {
                        int gid = std::stoi(gameIdStr);
                        borrowedById[gid] = false;
                    } catch (...) {}
                }
            }
        }
    }
    file.close();

    // Update game states
    int n = games.getLength();
    for (int i = 0; i < n; ++i) {
        auto node = games.getNode(i);
        if (node == nullptr) continue;
        Game& g = games.getItem(node);

        std::string gameName = g.getName();
        int gameId = g.getGameId();

        bool finalState = false; // Default to available
        bool stateFound = false;

        // Check by name first
        if (borrowedByName.find(gameName) != borrowedByName.end()) {
            finalState = borrowedByName[gameName];
            stateFound = true;
        }

        // Check by ID (this can override name-based state if there's a more recent ID-based record)
        if (borrowedById.find(gameId) != borrowedById.end()) {
            finalState = borrowedById[gameId];
            stateFound = true;
        }

        if (stateFound) {
            g.setBorrowed(finalState);
            std::cout << "Restored " << gameName << " (ID:" << gameId << ") borrowed state: "
                      << (finalState ? "BORROWED" : "AVAILABLE") << std::endl;
        }
    }
}


