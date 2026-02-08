/*********************************************************************************
 * Group         : T01
 * Team Member   : Khaleel Anis (S10270243)
 *
 * File Purpose:
 * - Declares the Rating model, which represents a single user rating and
 *   optional review for a board game.
 *
 * Key Design Notes:
 * - Stores both relational identifiers (gameId, userId) and denormalised data
 *   (gameName, username) to simplify display and reporting.
 * - Designed as a lightweight data object with read-only accessors.
 * - No business logic is embedded in this class.
 *
 * Constraints / Assumptions:
 * - Each Rating instance corresponds to exactly one game and one user.
 * - Validation (rating range, review length, etc.) is handled externally.
 *********************************************************************************/
#pragma once
#include <iostream>
#include <string>
#include <ctime>
using namespace std;

// Rating:
// Represents a single user rating + optional review for a specific game.
class Rating
{
	private:
		// ---- Primary identifiers ----
		int id;             // Unique rating ID
		int gameId;         // ID of the rated game
		int userId;         // ID of the user who submitted the rating

		// ---- Denormalised display data ----
		string gameName;    // Cached game name for display
		string username;    // Cached username for display

		// ---- Rating content ----
		int rating;         // Numerical rating score
		string review;      // Optional text review
		string createdAt;   // Timestamp of rating creation

	public:
		Rating();
		Rating(int id, int gameId, string gameName, int userId, string username, int rating, string review, string createdAt);

		// ---- Accessors ----
	    // All getters are const to preserve immutability of rating records.
		int getId() const { return id; }
		int getGameId() const { return gameId; }
		const std::string& getGameName() const { return gameName; }
		int getUserId() const { return userId; }
		const std::string& getUsername() const { return username; }
		int getRating() const { return rating; }
		const std::string& getReview() const { return review; }
		const std::string& getCreatedAt() const { return createdAt; }
};

// Stream output operator.
// Enables Rating objects to be printed via cout or List<Rating>::print().
ostream& operator<<(ostream& os, const Rating& r);