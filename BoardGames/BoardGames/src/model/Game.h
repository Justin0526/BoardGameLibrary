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
		string getName();
		int getMinPlayer();
		int getMaxPlayer();
		int getMaxPlayTime();
		int getMinPlaytime();
		int getYearPublished();
};