#include "Game.h"

Game::Game() : id(0), minPlayer(0), maxPlayer(0), maxPlaytime(0), minPlaytime(0), yearPublished(0), borrowed(false) {};

Game::Game(string name, int minPlayer, int maxPlayer, int maxPlaytime, int minPlaytime, int yearPublished)
	: name(name),
	id(0),
	minPlayer(minPlayer),
	maxPlayer(maxPlayer),
	maxPlaytime(maxPlaytime),
	minPlaytime(minPlaytime),
	yearPublished(yearPublished),
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
	os << "[" << g.getId() << "] " << g.getName() << ", " << 
		g.getMinPlayer() << ", " <<
		g.getMaxPlayer() << ", " <<
		g.getMaxPlayTime() << ", " <<
		g.getMinPlayTime() << ", " <<
		g.getYearPublished() << endl;
	return os;
}