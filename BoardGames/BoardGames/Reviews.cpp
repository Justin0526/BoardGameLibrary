#include "Reviews.h"
#include <iostream>
#include <fstream>
#include <limits>
#include <ctime>
using namespace std;

// Helper to get current date as string
static std::string getCurrentDateStr() {
    time_t t = time(nullptr);
    tm tm;
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
    return buf;
}

// Append a new review to ratings.csv
static void appendReviewToCSV(const Rating& r) {
    ofstream file("ratings.csv", ios::app);
    if (!file.is_open()) {
        cout << "ERROR: Cannot open ratings.csv for writing.\n";
        return;
    }
    file << r.getId() << "," << r.getGameId() << ",\"" << r.getGameName() << "\"," << r.getUserId() << ",\"" << r.getUsername() << "\"," << r.getRating() << ",\"" << r.getReview() << "\"," << r.getCreatedAt() << endl;
    file.close();
}

void writeReview(
    List<Rating>& ratings,
    HashTable<std::string, List<Rating>::NodePtr>& gameRatings,
    List<Member>& members,
    List<Game>& games,
    int memberId
) {
    cout << "Enter game ID to review: ";
    string gameIdStr;
    getline(cin, gameIdStr);
    int gameId = -1;
    try { gameId = stoi(gameIdStr); }
    catch (...) { cout << "Invalid game ID.\n"; return; }

    // Find game
    Game* gamePtr = nullptr;
    for (int i = 0; i < games.getLength(); ++i) {
        auto node = games.getNode(i);
        if (node && games.getItem(node).getGameId() == gameId) {
            gamePtr = &games.getItem(node);
            break;
        }
    }
    if (!gamePtr) { cout << "Game not found.\n"; return; }

    // Find member
    Member* memberPtr = nullptr;
    for (int i = 0; i < members.getLength(); ++i) {
        auto node = members.getNode(i);
        if (node && members.getItem(node).getUserId() == memberId) {
            memberPtr = &members.getItem(node);
            break;
        }
    }
    if (!memberPtr) { cout << "Member not found.\n"; return; }

    cout << "Enter your rating (1-5): ";
    string ratingStr;
    getline(cin, ratingStr);
    int rating = 0;
    try { rating = stoi(ratingStr); }
    catch (...) { cout << "Invalid rating.\n"; return; }
    if (rating < 1 || rating > 5) { cout << "Rating must be 1-5.\n"; return; }

    cout << "Enter your review: ";
    string reviewText;
    getline(cin, reviewText);

    // Generate new review ID
    int maxId = 0;
    for (int i = 0; i < ratings.getLength(); ++i) {
        auto node = ratings.getNode(i);
        if (node) maxId = std::max(maxId, ratings.getItem(node).getId());
    }
    int newId = maxId + 1;

    Rating r(
        newId,
        gameId,
        gamePtr->getName(),
        memberId,
        memberPtr->getName(),
        rating,
        reviewText,
        getCurrentDateStr()
    );
    List<Rating>::NodePtr ratingPtr = ratings.add(r);
    gameRatings.add(to_string(gameId), ratingPtr);

    appendReviewToCSV(r);

    cout << "Review submitted!\n";
}

void displayReviewsForGame(
    List<Rating>& ratings,
    HashTable<std::string, List<Rating>::NodePtr>& gameRatings,
    List<Game>& games
) {
    cout << "Enter game ID to view reviews: ";
    string gameIdStr;
    getline(cin, gameIdStr);

    // Find game
    Game* gamePtr = nullptr;
    for (int i = 0; i < games.getLength(); ++i) {
        auto node = games.getNode(i);
        if (node && games.getItem(node).getGameId() == stoi(gameIdStr)) {
            gamePtr = &games.getItem(node);
            break;
        }
    }
    if (!gamePtr) { cout << "Game not found.\n"; return; }

    cout << "\n--- Reviews for " << gamePtr->getName() << " ---\n";
    bool found = false;
    for (int i = 0; i < ratings.getLength(); ++i) {
        auto node = ratings.getNode(i);
        if (node && ratings.getItem(node).getGameId() == gamePtr->getGameId()) {
            const Rating& r = ratings.getItem(node);
            cout << "Member: " << r.getUsername() << " | Rating: " << r.getRating() << "/5\n";
            cout << "Review: " << r.getReview() << "\n";
            cout << "Date: " << r.getCreatedAt() << "\n";
            cout << "-----------------------------\n";
            found = true;
        }
    }
    if (!found) cout << "No reviews for this game yet.\n";
}