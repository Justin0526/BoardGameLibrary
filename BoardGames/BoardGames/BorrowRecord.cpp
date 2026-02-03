#include "BorrowRecord.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>
#include <sstream>
#include <map>
#include "Game.h"           
#include "LinkedList.h"     

void writeBorrowRecord(
    int recordId,
    const std::string& memberId,
    const std::string& gameId,
    const std::string& action
) {
    namespace fs = std::filesystem;
    
    // Write only to the project source directory
    fs::path sourcePath = fs::path(__FILE__);
    fs::path projectRoot = sourcePath.parent_path();
    fs::path projectPath = projectRoot / "borrow_records.csv";

    std::ofstream file(projectPath, std::ios::app);
    if (!file.is_open()) {
        std::cout << "ERROR: Cannot open file: " << projectPath.string() << std::endl;
        return;
    }

    file << recordId << "," << memberId << "," << gameId << "," << action << std::endl;
    
    if (!file) {
        std::cout << "ERROR: Failed to write to file: " << projectPath.string() << std::endl;
    }

    file.close();
}

// Add this new function to ensure CSV has proper header
void initializeBorrowRecordsCSV() {
    namespace fs = std::filesystem;
    fs::path sourcePath = fs::path(__FILE__);
    fs::path projectRoot = sourcePath.parent_path();
    fs::path projectPath = projectRoot / "borrow_records.csv";

    // Check if file exists and has content
    std::ifstream checkFile(projectPath);
    bool fileExists = checkFile.good();
    std::string firstLine;
    if (fileExists) {
        std::getline(checkFile, firstLine);
    }
    checkFile.close();

    // If file doesn't exist or doesn't have proper header, create it
    if (!fileExists || firstLine != "recordId,memberId,gameId,action") {
        std::ofstream file(projectPath, std::ios::trunc); // Overwrite mode
        if (file.is_open()) {
            file << "recordId,memberId,gameId,action" << std::endl;
            file.close();
            std::cout << "Initialized borrow_records.csv with header." << std::endl;
        }
    }
}

int getNextBorrowRecordId() {
    namespace fs = std::filesystem;
    fs::path sourcePath = fs::path(__FILE__);
    fs::path projectRoot = sourcePath.parent_path();
    fs::path projectPath = projectRoot / "borrow_records.csv";

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
    fs::path projectPath = projectRoot / "borrow_records.csv";

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
            
            // Parse: recordId,memberId,gameId,action
            std::istringstream ss(line);
            std::string recordId, memberIdStr, gameId, action;
            
            if (std::getline(ss, recordId, ',') &&
                std::getline(ss, memberIdStr, ',') &&
                std::getline(ss, gameId, ',') &&
                std::getline(ss, action)) {
                
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
    fs::path projectPath = projectRoot / "borrow_records.csv";

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
            
            // Parse: recordId,memberId,gameId,action
            std::istringstream ss(line);
            std::string recordId, memberIdStr, gameId, action;
            
            if (std::getline(ss, recordId, ',') &&
                std::getline(ss, memberIdStr, ',') &&
                std::getline(ss, gameId, ',') &&
                std::getline(ss, action)) {
                
                try {
                    if (std::stoi(memberIdStr) == memberId) {
                        BorrowHistoryRecord record;
                        record.action = action;
                        record.gameId = gameId;
                        record.gameDetails = nullptr;
                        
                        // Find the corresponding game object
                        int n = games.getLength();
                        for (int i = 0; i < n; ++i) {
                            auto node = games.getNode(i);
                            if (node == nullptr) continue;
                            Game& g = games.getItem(node);
                            
                            // Check if gameId matches by name or ID
                            if (g.getName() == gameId || std::to_string(g.getId()) == gameId) {
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
    fs::path projectPath = projectRoot / "borrow_records.csv";

    std::ifstream file(projectPath);
    if (!file.is_open()) {
        return; // No records to restore
    }

    // Track currently borrowed games by BOTH name and ID
    std::map<std::string, bool> borrowedByName;
    std::map<int, bool> borrowedById;

    std::string line;
    // Skip header
    if (std::getline(file, line)) {
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            // Parse: recordId,memberId,gameId,action
            std::istringstream ss(line);
            std::string recordId, memberIdStr, gameId, action;

            if (std::getline(ss, recordId, ',') &&
                std::getline(ss, memberIdStr, ',') &&
                std::getline(ss, gameId, ',') &&
                std::getline(ss, action)) {

                // Try to determine if gameId is a number (ID) or text (name)
                bool isNumeric = true;
                try {
                    int id = std::stoi(gameId);
                    // It's a numeric ID
                    if (action == "BORROW") {
                        borrowedById[id] = true;
                    } else if (action == "RETURN") {
                        borrowedById[id] = false;
                    }
                } catch (...) {
                    // It's a game name
                    isNumeric = false;
                }
                
                if (!isNumeric) {
                    // It's a game name
                    if (action == "BORROW") {
                        borrowedByName[gameId] = true;
                    } else if (action == "RETURN") {
                        borrowedByName[gameId] = false;
                    }
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
        int gameId = g.getId();
        
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


