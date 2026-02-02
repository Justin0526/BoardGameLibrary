#pragma once
#include<string>
#include<iostream>
using namespace std;

class Game {
	private:
		string name;
		int id;                 // unique id assigned at load time (per copy)
		int minPlayer;
		int maxPlayer;
		int maxPlaytime;
		int minPlaytime;
		int yearPublished;
		bool borrowed;          // whether this copy is currently borrowed

	public:
		Game();
		Game(string name, int minPlayer, int maxPlayer, int maxPlaytime, int minPlaytime, int yearPublished);

		// id accessors
		int getId() const;
		void setId(int idVal) ;

		// basic accessors
		string getName() const;
		int getMinPlayer() const;
		int getMaxPlayer() const;
		int getMaxPlayTime() const;
		int getMinPlayTime() const;
		int getYearPublished() const;

		// borrow state
		bool isBorrowed() const;
		void setBorrowed(bool b);

};

bool byYearAsc(const Game& a, const Game& b);
bool byYearDesc(const Game& a, const Game& b);
bool byNameAsc(const Game& a, const Game& b);
bool byNameDesc(const Game& a, const Game& b);

ostream& operator<<(ostream& os, const Game& g);