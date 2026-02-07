#pragma once
#include <string>
#include <vector>
#include "HashTable.h" 
using namespace std;

// Forward declarations
class Game;
template<typename T> class List;

struct BorrowHistoryRecord {
    string action;      // "BORROW" or "RETURN"
    string gameId;      // game name or ID from CSV
    Game* gameDetails;  // pointer to actual game object for details
    string borrowDate;
    string returnDate;
};

// Structure for GameBorrowStat
struct GameBorrowStat {
    int borrowCount = 0;
    int returnCount = 0;
    string lastBorrowDate = "";
    string lastAction = "";
};

void writeBorrowRecord(
    int recordId,
    const string& memberId,
    const string& gameId,
    const string& action,
    const string& borrowDate,
    const string& returnDate
);

void initializeBorrowRecordsCSV();
int getNextBorrowRecordId();
void loadMemberBorrowHistory(int memberId, vector<string>& borrowHistory);
void loadMemberBorrowHistoryDetailed(int memberId, List<Game>& games, vector<BorrowHistoryRecord>& borrowHistory);
void restoreGameBorrowStates(List<Game>& games);
bool buildBorrowStatsFromCSV(HashTable<string, GameBorrowStat>& stats, int& totalBorrows, int& totalReturns);
void displayOverallBorrowSummary();
void displayGameBorrowSummary(string gameId, List<Game>& games);
void displayAllGameBorrowSummary(List<Game>& games);
std::string getCurrentDate();