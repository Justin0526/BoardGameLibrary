#pragma once
#include <string>
#include <vector>
using namespace std;

// Forward declarations
class Game;
template<typename T> class List;

struct BorrowHistoryRecord {
    string action;      // "BORROW" or "RETURN"
    string gameId;      // game name or ID from CSV
    Game* gameDetails;  // pointer to actual game object for details
};

void writeBorrowRecord(
    int recordId,
    const string& memberId,
    const string& gameId,
    const string& action
);

void initializeBorrowRecordsCSV();
int getNextBorrowRecordId();
void loadMemberBorrowHistory(int memberId, vector<string>& borrowHistory);
void loadMemberBorrowHistoryDetailed(int memberId, List<Game>& games, vector<BorrowHistoryRecord>& borrowHistory);
void restoreGameBorrowStates(List<Game>& games);