/*********************************************************************************
 * Group         : T01
 * Team Member   : Khaleel Anis (S10270243)
 *
 * File Purpose:
 * - Declares review-related functions that allow members to write reviews
 *   and view existing reviews for board games.
 *
 * Key Design Notes:
 * - Reviews are stored as Rating objects to reuse the existing rating model.
 * - Reviews are indexed by gameId using a HashTable for efficient lookup.
 * - Functions are implemented procedurally to simplify menu integration.
 *
 * Constraints / Assumptions:
 * - Only members can submit reviews.
 * - Validation of user input (IDs, rating range) is handled within functions.
 * - Persistent storage is maintained via ratings.csv.
 *********************************************************************************/
#pragma once
#include "LinkedList.h"
#include "HashTable.h"
#include "Rating.h"
#include "Member.h"
#include "Game.h"
#include <string>

 // Write a review for a game by a member.
 // Prompts for rating and review text, updates in-memory structures,
 // and appends the new review to ratings.csv.
void writeReview(
    List<Rating>& ratings,
    HashTable<std::string, List<List<Rating>::NodePtr>*>& gameRatings,
    List<Member>& members,
    List<Game>& games,
    int memberId
);

// Display all reviews associated with a specific game.
// Reviews are retrieved via the gameRatings index for efficiency.
void displayReviewsForGame(
    List<Rating>& ratings,
    HashTable<std::string, List<List<Rating>::NodePtr>*>& gameRatings,
    List<Game>& games
);