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
		int getMinPlaytime() const;
		int getYearPublished() const;

};

ostream& operator<<(ostream& os, const Game& g);