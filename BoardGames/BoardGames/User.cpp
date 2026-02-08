/*********************************************************************************
 * Group         : T01
 * Team Member   : Khaleel Anis (S10270243)
 *
 * File Purpose:
 * - Implements the User base class.
 * - Provides shared logic for borrowing, returning, browsing,
 *   and recommending board games.
 *
 * Key Design Notes:
 * - Borrow/return enforcement is user-centric to prevent
 *   cross-user interference.
 * - UI pagination and sorting logic are kept here to
 *   avoid bloating controller code.
 * - Recommendation logic is intentionally data-driven,
 *   using HashTable indexes for performance.
 *
 * Constraints / Assumptions:
 * - Game availability is controlled via a single borrowed flag.
 * - All data structures (List, HashTable) are custom ADTs.
 * - CSV persistence is handled externally.
 *********************************************************************************/
#include "User.h"
#include "Member.h"
#include "Game.h"
#include "LinkedList.h"
#include "HashTable.h"
#include <iostream>
#include <iomanip>

User::User() {
    userId = 0;
    name = "";
    password = "";
    role = "";
}

User::User(int id, string name, string password, string role) {
    this->userId = id;
    this->name = name;
    this->password = password;
    this->role = role;
}

User::User(int id, string name, string role) {
    this->userId = id;
    this->name = name;
    this->role = role;
}

bool User::login(string inputName, string inputPassword) const {
    return (name == inputName && password == inputPassword);
}

int User::getUserId() const {
    return userId;
}

string User::getName() const {
    return name;
}

string User::getRole() const {
    return role;
}

// Helper: Truncate a long string to fit a fixed column width.
// If truncated, appends "..." to keep table alignment clean.
static string truncateText(const string& s, int width) {
    if ((int)s.size() <= width)
        return s;

    if (width <= 3)
        return s.substr(0, width);

    return s.substr(0, width - 3) + "...";
}

// Helper: Counts how many games are currently active (isActive == "TRUE").
// Used to compute total pages for pagination.
static int countActiveGames(List<Game>& games) {
    int count = 0;
    for (int i = 0; i < games.getLength(); i++) {
        auto node = games.getNode(i);
        if (!node)
            continue;
        Game& g = node->item;
        if (g.getIsActive() == "TRUE")
            count++;
    }
    return count;
}

// Helper: Prints one page of active games in a formatted table view.
// Uses iomanip manipulators:
// - left  : left-align columns
// - setw  : set fixed column width for alignment
static void printActiveGamesPage(List<Game>& games, int page, int pageSize) {
    const int totalActive = countActiveGames(games);
    const int totalPages = (totalActive + pageSize - 1) / pageSize;

    if (totalActive == 0) {
        cout << "No active games.\n";
        return;
    }
    if (page < 0) page = 0;
    if (page >= totalPages) page = totalPages - 1;

    const int start = page * pageSize;
    const int end = start + pageSize; // exclusive

    cout << "\nActive Games (Page " << (page + 1) << "/" << totalPages << ")\n";
    cout << "-------------------------------------------------------------------------------\n";
    cout << left
        << setw(6) << "ID"
        << setw(34) << "Name"
        << setw(10) << "Players"
        << setw(12) << "PlayTime"
        << setw(8) << "Year"
        << "\n";
    cout << "-------------------------------------------------------------------------------\n";

    int activeIndex = 0;
    for (int i = 0; i < games.getLength(); i++) {
        auto node = games.getNode(i);
        if (!node) continue;
        Game& g = node->item;

        if (g.getIsActive() != "TRUE") continue;

        if (activeIndex >= start && activeIndex < end) {
            string players = to_string(g.getMinPlayer()) + "-" + to_string(g.getMaxPlayer());
            string time = to_string(g.getMinPlayTime()) + "-" + to_string(g.getMaxPlayTime());

            cout << left
                << setw(6) << g.getGameId()
                << setw(34) << truncateText(g.getName(), 33)
                << setw(10) << players
                << setw(12) << time
                << setw(8) << g.getYearPublished()
                << "\n";
        }

        activeIndex++;
        if (activeIndex >= end) break; // stop early once page filled
    }

    cout << "-------------------------------------------------------------------------------\n";
    cout << "[n] Next  [p] Prev  [q] Quit\n";
}

/*********************************************************************************
 * Function  : User::printActiveGames
 * Purpose   : Displays all active games (soft-delete aware) using pagination.
 *
 * Notes     :
 * - Filters out inactive games (isActive != "TRUE").
 * - Supports navigation: [n] next page, [p] previous page, [q] quit.
 * - Uses helper functions: countActiveGames() and printActiveGamesPage().
 *********************************************************************************/
void User::printActiveGames(List<Game>& games) {
    const int pageSize = 20;
    int page = 0;

    const int totalActive = countActiveGames(games);
    const int totalPages = (totalActive + pageSize - 1) / pageSize;

    if (totalActive == 0) {
        cout << "No active games.\n";
        return;
    }

    while (true) {
        if (page < 0) page = 0;
        if (page >= totalPages) page = totalPages - 1;

        printActiveGamesPage(games, page, pageSize);

        cout << "Choice: ";
        string choice;
        getline(cin, choice);

        if (choice == "n" || choice == "N") page++;
        else if (choice == "p" || choice == "P") page--;
        else if (choice == "q" || choice == "Q") break;
        else cout << "Invalid option.\n";
    }
}

bool User::borrowGame(
    List<Game>& games,
    HashTable<string, List<Game>::NodePtr>& /*gameTable*/,
    const string& gameName
) {
    // Find game by NAME
    List<Game>::NodePtr found = nullptr;

    for (int i = 0; i < games.getLength(); ++i) {
        auto node = games.getNode(i);
        if (node && games.getItem(node).getName() == gameName) {
            found = node;
            break;
        }
    }

    if (!found) {
        cout << "Game \"" << gameName << "\" not found.\n";
        return false;
    }

    Game& g = games.getItem(found);
    int gid = g.getGameId();

    // Check availability using borrowed list ONLY
    for (int i = 0; i < borrowed.getLength(); ++i) {
        if (borrowed.get(i) == gid) {
            cout << "No available copy for \"" << gameName << "\".\n";
            return false;
        }
    }

    // Borrow
    g.setBorrowed(true);        // optional UI state
    borrowed.add(gid);          // authoritative state
    history.add(gid);           // audit trail

    cout << role << " " << name
        << " borrowed game [" << gid << "] "
        << g.getName() << endl;

    return true;
}

bool User::borrowGameById(
    List<Game>& games,
    HashTable<string, List<Game>::NodePtr>& gameTable,
    int gameId
) {
    string key = to_string(gameId);

    if (!gameTable.containsKey(key)) {
        cout << "Game ID " << gameId << " not found.\n";
        return false;
    }

    auto node = gameTable.get(key);
    Game& g = games.getItem(node);

    // Check availability
    for (int i = 0; i < borrowed.getLength(); ++i) {
        if (borrowed.get(i) == gameId) {
            cout << "No available copy for game ID " << gameId << ".\n";
            return false;
        }
    }

    // Borrow
    g.setBorrowed(true);
    borrowed.add(gameId);
    history.add(gameId);

    cout << role << " " << name
        << " borrowed game [" << gameId << "] "
        << g.getName() << endl;

    return true;
}

bool User::returnGame(
    List<Game>& games,
    HashTable<std::string, List<Game>::NodePtr>& gameTable,
    int gameId
) {
    auto node = gameTable.get(std::to_string(gameId));
    if (!node) {
        cout << "Game ID " << gameId << " not found.\n";
        return false;
    }

    if (!games.getItem(node).isBorrowed()) {
        cout << "Game is not currently borrowed.\n";
        return false;
    }

    // Must belong to THIS user
    bool ownedByUser = false;
    List<int>::NodePtr target = nullptr;

    for (int i = 0; i < borrowed.getLength(); ++i) {
        auto p = borrowed.getNode(i);
        if (p && borrowed.getItem(p) == gameId) {
            ownedByUser = true;
            target = p;
            break;
        }
    }

    if (!ownedByUser) {
        cout << "You cannot return a game you did not borrow.\n";
        return false;
    }

    // Proceed with return
    Game& g = games.getItem(node);
    g.setBorrowed(false);
    borrowed.remove(target);
    history.add(gameId);

    cout << role << " " << name
        << " returned game [" << gameId << "] "
        << g.getName() << endl;

    return true;
}


void User::displayBorrowedAndHistory() const {
    cout << role << ": " << getName() << " (ID:" << getUserId() << ")\n";
    cout << "Currently borrowed (" << borrowed.getLength() << "):\n";
    if (borrowed.getLength() == 0) {
        cout << "  None\n";
    } else {
        for (int i = 0; i < borrowed.getLength(); ++i) {
            cout << "  Game ID: " << borrowed.get(i) << "\n";
        }
    }

    cout << "Borrow history (" << history.getLength() << "):\n";
    if (history.getLength() == 0) {
        cout << "  None\n";
    } else {
        for (int i = 0; i < history.getLength(); ++i) {
            cout << "  Game ID: " << history.get(i) << "\n";
        }
    }
}

/*********************************************************************************
 * Function  : User::displayGamesPlayableByNPlayers
 * Purpose   : Filters games that can support a user-specified player count (N)
 *             and optionally sorts them before displaying only active results.
 *
 * Notes     :
 * - Filtering uses List<Game>::filter(...) with a lambda predicate.
 * - Sorting (optional) uses List<Game>::sort(cmp) with comparator functions.
 *********************************************************************************/
void User::displayGamesPlayableByNPlayers(List<Game>& games) {
    cout << "---- Display a list of games that can be played by a given number of players ----\n";
    // Ask user for number of players
    string input;
    int n;
    cout << "Number of player(s): ";

    while (true) {
        getline(cin, input);
        stringstream ss(input);

        if (!(ss >> n) || n <= 0 || !ss.eof()) {
            cout << "Invalid input. Please enter a whole number 1 or more.\n";
            cout << "Number of player(s): ";
        }
        else {
            break;
        }
    }

    // Ask user for sorting choice
    int option = -1;
    bool (*cmp)(const Game&, const Game&) = nullptr;

    while (true) {
        cout << "\n-- Sort By --" << endl;
        cout << "0. Default\n";
        cout << "1. Name in Ascending order\n";
        cout << "2. Name in Descending order\n";
        cout << "3. Published year in Ascending order\n";
        cout << "4. Published year in Descending order\n";

        cout << "Enter your option: ";
        cin >> option;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << endl;

        if (option == 0) {
            cout << "Loading in default view...\n";
            break;
        }

        else if (option == 1)
        {
            cout << "Loading NAME in ascending order...\n";
            cmp = byNameAsc;
            break;
        }

        else if (option == 2) {
            cout << "Loading NAME in descending order...\n";
            cmp = byNameDesc;
            break;
        }

        else if (option == 3) {
            cout << "Loading PUBLISHED YEAR in ascending order...\n";
            cmp = byYearAsc;
            break;
        }
            
        else if (option == 4) {
            cout << "loading PUBLISHED YEAR in descendig order...\n";
            cmp = byYearDesc;
            break;
        }
           
        else
            cout << "Invalid option!";

    }
  
    // Lambda predicate for filtering:
    // [&] captures local variables by reference (here: n).
    // The predicate returns true if the game supports N players:
    // minPlayer <= n <= maxPlayer.
    List<Game> filtered = games.filter([&](const Game& g) {
        return n >= g.getMinPlayer() && n <= g.getMaxPlayer();
    });
   
    // sort filtered list using merge sort
    if (cmp != nullptr) {
        filtered.sort(cmp);
    }
    // print
    printActiveGames(filtered);
}

/*********************************************************************************
 * Function  : User::recommendFromGame
 * Purpose   : Generates recommendation candidates based on collaborative filtering.
 *
 * High-Level Logic:
 * 1) Identify "fans" who rated the target game >= ratingCutoff.
 * 2) For each fan, scan their other high-rated games (>= ratingCutoff).
 * 3) Aggregate candidate scores (sum of ratings) and support (number of fans).
 * 4) Output candidates as GameCandidate objects into outCandidates.
 *
 * Params    :
 * - gameId         : Target game ID to recommend from
 * - ratings        : Master list of all Rating objects
 * - outCandidates  : Output list to be cleared and filled with candidates
 * - gameRatings    : Index (gameId -> list of rating node pointers for that game)
 * - memberRatings  : Index (userId -> list of rating node pointers by that user)
 * - games          : Game list for resolving game names
 * - gameTable      : Index (gameId -> Game node pointer) for fast lookup
 *
 * Notes     :
 * - Uses HashTable to aggregate candidateScore and candidateSupport by candId.
 * - outCandidates is cleared at the start to avoid shallow-copy issues.
 *********************************************************************************/
void User::recommendFromGame(
    int gameId,
    List<Rating>& ratings, 
    List<GameCandidate>& outCandidates,
    HashTable<string, List<List<Rating>::NodePtr>*>& gameRatings, 
    HashTable<string, List<List<Rating>::NodePtr>*>& memberRatings,
    List<Game>& games,
    HashTable<string, List<Game>::NodePtr>& gameTable) {
    // Clear outcandidates
    outCandidates.clear();
    int fanCount = 0;
    int totalFanRatingsScanned = 0;
    int totalCandidateAdds = 0;
    int totalCandidateScoreKeys = 0;

    // Game scoring table
    HashTable<string, int> candidateScore;
    HashTable<string, int> candidateSupport;

    // Parameters
    int ratingCutoff = 4;
    int minSupport = 2;
    int topN = 10;

    // Get all ratings from game
    string targetKey = to_string(gameId);
    if (!gameRatings.containsKey(targetKey)) {
        cout << "There is currently no ratings for this game\n";
        return;
    }
    List<List<Rating>::NodePtr>* allRatings = gameRatings.get(targetKey);

    // Find "fans" of the game A
    for (auto node = allRatings->getNode(0); node != nullptr; node = node->next) {
        List<Rating>::NodePtr ratingNode = node->item;
        Rating& r = ratings.getItem(ratingNode);
        if (r.getRating() >= ratingCutoff) {
            fanCount++;

            string userKey = to_string(r.getUserId());
            if (!memberRatings.containsKey(userKey)) continue;
            List<List<Rating>::NodePtr>* memberList = memberRatings.get(userKey);
            
            if (memberList == nullptr) {
                cout << "We can’t find this user in memberRatings index\n";
                continue;
            }

            for (auto mNode = memberList->getNode(0); mNode != nullptr; mNode = mNode->next){
                totalFanRatingsScanned++;
                List<Rating>::NodePtr mRatingNode = mNode->item;
                Rating& mr = ratings.getItem(mRatingNode);

                if (mr.getRating() >= ratingCutoff && mr.getGameId() != gameId) {
                    totalCandidateAdds++;
                    string candId = to_string(mr.getGameId());
                    int score;

                    if (!candidateScore.containsKey(candId)) {
                        score = mr.getRating();
                    }
                    else {
                        score = candidateScore.get(candId) + mr.getRating();
                    }
                    candidateScore.addOrUpdate(candId, score);

                    int numOfSupport;
                    if (!candidateSupport.containsKey(candId)) {
                        numOfSupport = 1;
                    }
                    else {
                        numOfSupport = candidateSupport.get(candId) + 1;
                    }
                    candidateSupport.addOrUpdate(candId, numOfSupport);
                }
            }
        }
    }
    //cout << "\n[DEBUG] fanCount=" << fanCount
    //    << " totalFanRatingsScanned=" << totalFanRatingsScanned
    //    << " totalCandidateAdds=" << totalCandidateAdds << "\n";

    // Convert aggregated hash-table results into a List<GameCandidate> for sorting/display.
    // Lambda captures external tables/lists (candidateSupport, gameTable, games, outCandidates).
    candidateScore.forEach([&](const string& candId, int score) {
        totalCandidateScoreKeys++;
        if (!candidateSupport.containsKey(candId)) return;
        int support = candidateSupport.get(candId);

        // lookup name
        string name = "(unknown)";
        if (gameTable.containsKey(candId)) {
            Game& g = games.getItem(gameTable.get(candId));
            name = g.getName();
        }

        outCandidates.add(GameCandidate(candId, name, score, support));
        });

    //cout << "[DEBUG] candidateScoreKeys=" << totalCandidateScoreKeys
    //    << " outCandidatesLen=" << outCandidates.getLength() << "\n";

}

/*********************************************************************************
 * Function  : User::printRecommendedGames
 * Purpose   : Displays recommended game candidates and allows user-selected sorting.
 *
 * Notes     :
 * - Sorting uses List<GameCandidate>::sort(cmp) with comparator functions.
 * - Filters out inactive games using gameTable lookup + isActive check.
 * - Uses truncateText() to keep table columns aligned.
 *********************************************************************************/
void User::printRecommendedGames(
    List<GameCandidate>& candidates,
    List<Game>& games,
    HashTable<string, List<Game>::NodePtr>& gameTable
) {

    int option = -1;
    bool (*cmp)(const GameCandidate&, const GameCandidate&) = nullptr;

    while (true) {
        cout << "\n-- Sort By --" << endl;
        cout << "0. Default\n";
        cout << "1. Name in Ascending order\n";
        cout << "2. Name in Descending order\n";
        cout << "3. Score in Ascending order\n";
        cout << "4. Score in Descending order\n";
        cout << "5. Support in Ascending order\n";
        cout << "6. Support in Descending order\n";

        cout << "Enter your option: ";
        cin >> option;
        cout << endl;

        if (option == 0) {
            cout << "Loading in default view...\n";
            break;
        }

        else if (option == 1)
        {
            cout << "Loading NAME in ascending order...\n";
            cmp = byNameAsc;
            break;
        }

        else if (option == 2) {
            cout << "Loading NAME in descending order...\n";
            cmp = byNameDesc;
            break;
        }

        else if (option == 3) {
            cout << "Loading SCORE in ascending order...\n";
            cmp = byScoreAsc;
            break;
        }

        else if (option == 4) {
            cout << "loading SCORE in descending order...\n";
            cmp = byScoreDesc;
            break;
        }

        else if (option == 5) {
            cout << "loading SUPPORT in ascending order...\n";
            cmp = bySupportAsc;
            break;
        }

        else if (option == 6) {
            cout << "loading SUPPORT in descending order...\n";
            cmp = bySupportDesc;
            break;
        }

        else
            cout << "Invalid option!";

    }

    if (cmp != nullptr) {
        candidates.sort(cmp);
    }

    cout << "\n" << left
        << setw(8) << "GameID"
        << setw(36) << "Name"
        << setw(8) << "Score"
        << setw(10) << "Support"
        << setw(8) << "Avg/5"
        << "\n";
    cout << string(70, '-') << "\n";

    cout << fixed << setprecision(2);

    for (auto p = candidates.getNode(0); p != nullptr; p = p->next) {
        GameCandidate& c = candidates.getItem(p);
        const string& gid = c.getGameId();

        if (!gameTable.containsKey(gid))
            continue;

        Game& g = games.getItem(gameTable.get(gid));
        if (g.getIsActive() != "TRUE")
            continue;

        double avg = (double)c.getScore() / c.getSupport();

        ostringstream avgOut;
        avgOut << avg << "/5";

        cout << left
            << setw(8) << gid
            << setw(36) << truncateText(g.getName(), 35)
            << setw(8) << c.getScore()
            << setw(10) << c.getSupport()
            << setw(8) << avgOut.str()
            << "\n";
    }
}

ostream& operator<<(ostream& os, const User& u) {
    os << u.getName() << " [User ID: " << u.getUserId() << "]" << endl;
    return os;
}
