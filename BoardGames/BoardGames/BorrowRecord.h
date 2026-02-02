#pragma once
#include <string>
#include <vector>
using namespace std;

// Forward declarations - don't include the full headers here to avoid circular dependencies
class Game;
template<typename T> class List;

void writeBorrowRecord(
    int recordId,
    const string& memberId,
    const string& gameId,
    const string& action
);

void initializeBorrowRecordsCSV();
int getNextBorrowRecordId();
void loadMemberBorrowHistory(int memberId, vector<string>& borrowHistory);
void restoreGameBorrowStates(List<Game>& games);