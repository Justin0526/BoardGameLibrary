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

ostream& operator<<(ostream& os, const Rating& r) {
    os << r.getId() << ", "
        << r.getGameId() << ", "
        << r.getGameName() << ", "
        << r.getUserId() << ", "
        << r.getUsername() << ", "
        << r.getRating() << endl;
    return os;
}
