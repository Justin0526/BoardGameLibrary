#include "Game.h"

Game::Game() {};

Game::Game(string name, int minPlayer, int maxPlayer, int maxPlaytime, int minPlaytime, int yearPublished)
	: name(name),
	minPlayer(minPlayer),
	maxPlayer(maxPlayer),
	maxPlaytime(maxPlaytime),
	minPlaytime(minPlaytime),
	yearPublished(yearPublished) {
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

ostream& operator<<(ostream& os, const Game& g) {
	os << g.getName() << ", " << 
		g.getMinPlayer() << ", " <<
		g.getMaxPlayer() << ", " <<
		g.getMaxPlayTime() << ", " <<
		g.getMinPlayTime() << ", " <<
		g.getYearPublished() << endl;
	return os;
}