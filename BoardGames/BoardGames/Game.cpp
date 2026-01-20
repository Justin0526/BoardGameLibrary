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

void Game::setName(string name) {
	this->name = name;
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

int Game::getMinPlaytime() const {
	return this->minPlaytime;
}

int Game::getYearPublished() const {
	return this->yearPublished;
}


ostream& operator<<(ostream& os, const Game& g) {
	os << g.getName() << ", " << 
		g.getMinPlayer() << ", " <<
		g.getMaxPlayer() << ", " <<
		g.getMaxPlayTime() << ", " <<
		g.getMinPlaytime() << ", " <<
		g.getYearPublished() << endl;
	return os;
}