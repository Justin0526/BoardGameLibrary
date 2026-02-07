/*********************************************************************************
 * Group         : T01
 * Team Member   : Justin Tang Jia Ze (S10269496B)
 *
 * File Purpose:
 * - Defines the Game class, which represents a single board game record in the
 *   system, including player limits, playtime, publication year, and availability.
 *
 * Key Design Notes:
 * - Each Game object represents one physical copy of a game.
 * - Borrow status is tracked using a boolean flag instead of removing the object.
 * - The isActive field is used for soft-deletion and filtering (e.g., active listings).
 *
 * Constraints / Assumptions:
 * - gameId uniquely identifies a game across the system.
 * - copy represents the number of physical copies available for borrowing.
 *********************************************************************************/
#pragma once
#include<string>
#include<iostream>
using namespace std;

// Game:
// Core entity representing a board game and its borrow-related state.
class Game {
	private:
		string name;
		int minPlayer;
		int maxPlayer;
		int maxPlaytime;
		int minPlaytime;
		int yearPublished;
		int gameId;
		string isActive;
		bool borrowed;          // Indicates whether this game copy is currently borrowed

	public:
		Game();
		Game(int gameId, string name, int minPlayer, int maxPlayer, int minPlaytime, int maxPlaytime, int yearPublished,  string isActive);

		// ---- Basic accessors (game metadata) ----
		string getName() const;
		int getMinPlayer() const;
		int getMaxPlayer() const;
		int getMaxPlayTime() const;
		int getMinPlayTime() const;
		int getYearPublished() const;
		int getGameId() const;
		string getIsActive() const;
		string setIsActive(string active);

		// ---- Borrow state accessors ----
		bool isBorrowed() const;
		void setBorrowed(bool b);
};

// Comparator functions for sorting Game objects using List<Game>::sort().
// Asc  = ascending order
// Desc = descending order
bool byYearAsc(const Game& a, const Game& b);
bool byYearDesc(const Game& a, const Game& b);
bool byNameAsc(const Game& a, const Game& b);
bool byNameDesc(const Game& a, const Game& b);

// Stream output operator.
// Enables Game objects to be printed via List<Game>::print() and cout.
ostream& operator<<(ostream& os, const Game& g);