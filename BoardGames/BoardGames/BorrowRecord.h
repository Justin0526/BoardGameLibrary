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
    string borrowDate;
    string returnDate;
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
std::string getCurrentDate();