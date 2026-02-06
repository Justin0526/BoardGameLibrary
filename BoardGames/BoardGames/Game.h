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
		int gameId;
		int copy;
		string isActive;
		bool borrowed;          // whether this copy is currently borrowed

	public:
		Game();
		Game(int gameId, string name, int minPlayer, int maxPlayer, int minPlaytime, int maxPlaytime, int yearPublished, int copy, string isActive);

		// basic accessors
		string getName() const;
		int getMinPlayer() const;
		int getMaxPlayer() const;
		int getMaxPlayTime() const;
		int getMinPlayTime() const;
		int getYearPublished() const;
		int getGameId() const;
		int getGameCopy() const;
		int setGameCopy(int num);
		string getIsActive() const;
		string setIsActive(string active);

		// borrow state
		bool isBorrowed() const;
		void setBorrowed(bool b);

};

bool byYearAsc(const Game& a, const Game& b);
bool byYearDesc(const Game& a, const Game& b);
bool byNameAsc(const Game& a, const Game& b);
bool byNameDesc(const Game& a, const Game& b);

ostream& operator<<(ostream& os, const Game& g);