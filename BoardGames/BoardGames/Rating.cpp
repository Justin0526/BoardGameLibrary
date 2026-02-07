/*********************************************************************************
 * Group         : T01
 * Team Member   : Khaleel Anis (S10270243)
 *
 * File Purpose:
 * - Implements the Rating data model and its associated stream output operator.
 *
 * Key Design Notes:
 * - Rating is treated as an immutable record after construction.
 * - Operator<< provides a consistent, concise representation for debugging
 *   and reporting.
 *
 * Constraints / Assumptions:
 * - Formatting of output is intended for console/debug use only.
 * - Full review text may be omitted depending on display context.
 *********************************************************************************/
#include "Rating.h"

 // Default constructor
Rating::Rating() {};

// Parameterised constructor
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

// Stream output operator.
// Prints key rating fields in a compact, readable format.
ostream& operator<<(ostream& os, const Rating& r) {
    os << r.getId() << ", "
        << r.getGameId() << ", "
        << r.getGameName() << ", "
        << r.getUserId() << ", "
        << r.getUsername() << ", "
        << r.getRating() << endl;
    return os;
}
