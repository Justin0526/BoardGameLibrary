#pragma once
#include "LinkedList.h"
#include "HashTable.h"
#include "Rating.h"
#include "Member.h"
#include "Game.h"
#include <string>

// Write a review for a game (by a member)
void writeReview(
    List<Rating>& ratings,
    HashTable<std::string, List<Rating>::NodePtr>& gameRatings,
    List<Member>& members,
    List<Game>& games,
    int memberId
);

// Display all reviews for a game
void displayReviewsForGame(
    List<Rating>& ratings,
    HashTable<std::string, List<Rating>::NodePtr>& gameRatings,
    List<Game>& games
);