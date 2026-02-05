#include "Rating.h"

Rating::Rating() {};

Rating::Rating(int id, int gameId, string gameName, int userId, string username, int rating, string review, string createdAt)
    : id(id),
      gameId(gameId),
      gameName(gameName),
      userId(userId),
      username(username),
      rating(rating),
      review(review),
      createdAt(createdAt)
{}

int Rating::getRating() {
    return this->rating;
}

int Rating::getUserId() {
    return this->userId;
}

int Rating::getGameId() {
    return this->gameId;
}

ostream& operator<<(ostream& os, const Rating& r) {
    os << r.getId() << ", "
        << r.getGameId() << ", "
        << r.getGameName() << ", "
        << r.getUserId() << ", "
        << r.getUsername() << ", "
        << r.getRating() << endl;
    return os;
}