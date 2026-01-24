#pragma once
#include<string>
#include<iostream>
using namespace std;

class Game {
	private:
		string name;
		int minPlayer;
		int maxPlayer;
		int maxPlaytime;
		int minPlaytime;
		int yearPublished;

	public:
		Game();
		Game(string name, int minPlayer, int maxPlayer, int maxPlaytime, int minPlaytime, int yearPublished);
		string getName() const;
		int getMinPlayer() const;
		int getMaxPlayer() const;
		int getMaxPlayTime() const;
		int getMinPlayTime() const;
		int getYearPublished() const;

};

bool byYearAsc(const Game& a, const Game& b);
bool byYearDesc(const Game& a, const Game& b);
bool byNameAsc(const Game& a, const Game& b);
bool byNameDesc(const Game& a, const Game& b);

ostream& operator<<(ostream& os, const Game& g);