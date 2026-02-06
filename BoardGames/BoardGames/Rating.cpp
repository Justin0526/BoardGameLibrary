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

