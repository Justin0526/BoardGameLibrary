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

		int getId() const { return id; }
		int getGameId() const { return gameId; }
		const std::string& getGameName() const { return gameName; }
		int getUserId() const { return userId; }
		const std::string& getUsername() const { return username; }
		int getRating() const { return rating; }
		const std::string& getReview() const { return review; }
		const std::string& getCreatedAt() const { return createdAt; }
};
