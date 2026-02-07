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
#include "Game.h"

// Default constructor
Game::Game() : gameId(1), minPlayer(0), maxPlayer(0), minPlaytime(0), maxPlaytime(0), yearPublished(0),  copy(1), isActive("TRUE"), borrowed(false) {};

// Parameterised constructor
Game::Game(int gameId, string name, int minPlayer, int maxPlayer, int minPlaytime, int maxPlaytime, int yearPublished, int copy, string isActive)
	: gameId(gameId),
	name(name),
	minPlayer(minPlayer),
	maxPlayer(maxPlayer),
	minPlaytime(minPlaytime),
	maxPlaytime(maxPlaytime),
	yearPublished(yearPublished),
	copy (copy),
	isActive(isActive),
	borrowed(false) {
}

// ---- Basic accessors (metadata) ----
// All getters are const to prevent unintended modification.
string Game::getName() const {
	return this->name;
}

int Game::getMinPlayer() const {
	return this->minPlayer;
}

int Game::getMaxPlayer() const {
	return this->maxPlayer;
}

int Game::getMaxPlayTime() const {
	return this->maxPlaytime;
}

int Game::getMinPlayTime() const {
	return this->minPlaytime;
}

int Game::getYearPublished() const {
	return this->yearPublished;
}

int Game::getGameId() const {
	return this->gameId;
}

int Game::getGameCopy() const {
	return this->copy;
}

// Updates the copy count and returns the updated value.
int Game::setGameCopy(int num) {
	return this->copy = num;
}

string Game::getIsActive() const {
	return this->isActive;
}

// Updates active status ("TRUE"/"FALSE") and returns the updated value.
// Used for soft-delete / filtering.
string Game::setIsActive(string active) {
	return this->isActive = active;
}

// Comparator functions for sorting Game objects using List<Game>::sort().
// Asc  = ascending order
// Desc = descending order
bool byYearAsc(const Game& a, const Game& b) {
	return a.getYearPublished() < b.getYearPublished();
}

bool byYearDesc(const Game& a, const Game& b) {
	return a.getYearPublished() > b.getYearPublished();
}

bool byNameAsc(const Game& a, const Game& b) {
	return a.getName() < b.getName();
}

bool byNameDesc(const Game& a, const Game& b) {
	return a.getName() > b.getName();
}

// ---- Borrow state ----
// Tracks whether this game is currently borrowed during runtime.
bool Game::isBorrowed() const {
	return this->borrowed;
}

void Game::setBorrowed(bool b) {
	this->borrowed = b;
}

// Stream output operator.
// Prints a single Game record in a compact CSV-like format for List::print().
ostream& operator<<(ostream& os, const Game& g) {
	os << "[" << g.getGameId() << "] "
		<< g.getName() << ", " <<
		g.getMinPlayer() << ", " <<
		g.getMaxPlayer() << ", " <<
		g.getMinPlayTime() << ", " <<
		g.getMaxPlayTime() << ", " <<
		g.getYearPublished() << ", " << 
		g.getGameCopy() << endl;
	return os;
}