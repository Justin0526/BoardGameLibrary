#include "Game.h"

Game::Game() : gameId(1), minPlayer(0), maxPlayer(0), minPlaytime(0), maxPlaytime(0), yearPublished(0),  copy(1), isActive("TRUE"), borrowed(false) {};

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

int Game::getId() const {
	return this->id;
}

void Game::setId(int idVal) {
	this->id = idVal;
}

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

int Game::setGameCopy(int num) {
	return this->copy = num;
}

string Game::getIsActive() const {
	return this->isActive;
}

string Game::setIsActive(string active) {
	return this->isActive = active;
}

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

bool Game::isBorrowed() const {
	return this->borrowed;
}

void Game::setBorrowed(bool b) {
	this->borrowed = b;
}

ostream& operator<<(ostream& os, const Game& g) {
	os << "[" << g.getGameId() << "] "
		<< g.getName() << ", " <<
		g.getMinPlayer() << ", " <<
		g.getMaxPlayer() << ", " <<
		g.getMaxPlayTime() << ", " <<
		g.getMinPlayTime() << ", " <<
		g.getYearPublished() << ", " << 
		g.getGameCopy() << endl;
	return os;
}