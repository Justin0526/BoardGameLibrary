#pragma once
#include <iostream>
#include <string>
#include <ctime>
using namespace std;

class Rating
{
	private:
		int id;
		int gameId;
		string gameName;
		int userId;
		string username;
		int rating;
		string review;
		string createdAt;

	public:
		Rating();
		Rating(int id, int gameId, string gameName, int userId, string username, int rating, string review, string createdAt);

		int getRating();
		int getUserId();
		int getGameId();
};


