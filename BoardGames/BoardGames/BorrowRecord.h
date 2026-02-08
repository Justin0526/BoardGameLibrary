/*********************************************************************************
 * Group         : T01
 * Team Member   : Khaleel Anis (S10270243)
 *
 * File Purpose:
 * - Declares data structures and helper functions for tracking
 *   game borrow and return records.
 * - Serves as the persistence and reporting backbone for
 *   borrowing-related features.
 *
 * Key Design Notes:
 * - Borrow records are stored externally in borrow_records.csv.
 * - GameBorrowStat aggregates statistics derived from CSV records.
 * - BorrowHistoryRecord supports enriched, per-member history views
 *   with optional Game object linkage.
 *
 * Constraints / Assumptions:
 * - CSV file is the single source of truth for borrow/return history.
 * - Game IDs are stable and unique.
 * - Borrow and return actions are logged sequentially.
 *********************************************************************************/
#pragma once
#include <string>
#include <vector>
#include "HashTable.h" 
using namespace std;

// Forward declarations
class Game;
template<typename T> class List;

// Represents a single borrow/return event for history display.
struct BorrowHistoryRecord {
    string action;      // "BORROW" or "RETURN"
    string gameId;      // game name or ID from CSV
    Game* gameDetails;  // pointer to actual game object for details
    string borrowDate;
    string returnDate;
};

// Aggregated statistics for a single game.
struct GameBorrowStat {
    int borrowCount = 0;
    int returnCount = 0;
    string lastBorrowDate = "";
    string lastAction = "";
};

// ---- CSV persistence helpers ----
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

// ---- Borrow history loading ----
void loadMemberBorrowHistory(int memberId, vector<string>& borrowHistory);
void loadMemberBorrowHistoryDetailed(int memberId, List<Game>& games, vector<BorrowHistoryRecord>& borrowHistory);

// ---- State restoration & statistics ----
void restoreGameBorrowStates(List<Game>& games);
bool buildBorrowStatsFromCSV(HashTable<string, GameBorrowStat>& stats, int& totalBorrows, int& totalReturns);

// ---- Reporting / summaries ----
void displayOverallBorrowSummary();
void displayGameBorrowSummary(string gameId, List<Game>& games);
void displayAllGameBorrowSummary(List<Game>& games);
// ---- Utility ----
std::string getCurrentDate();