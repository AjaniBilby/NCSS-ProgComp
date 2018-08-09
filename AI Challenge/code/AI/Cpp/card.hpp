#pragma once

#include <iostream>
#include <string>
#include <vector>

const char cardsSuit[5]  = "DCHS";
const char cardsRank[14] = "34567890JQKA2";
#define cardCount 52

typedef char CardText[2];

class Card{
	int  suitID;
	int  rankID;

	public:
		bool invalid;

		Card();
		Card(CardText);
		Card(int rankID, int suitID);

		std::string toString();

		bool lessThan    (Card *other);
		bool greaterThan (Card *other);
		bool equalTo     (Card *other);
};



class CardSet{
	std::vector<Card> remain;

	public:
		// Create a card set the uses a whole deck or is empty
		CardSet(bool autofill=true);
		// Create a duplicate card set
		CardSet(CardSet *other);

		// Get a specific card from the set
		Card& operator[](int x);
		// Get the number of cards able to be stored within the set
		size_t capacity();
		// Get the number of cards within the set
		size_t size();

		// Returns the percentile this card lies within (of strength)
		float strength(Card card);

		// Add a card to the set
		void append(Card card);

		// Does this set contain the given card?
		bool contains(Card card);

		// Find the lowest card > floor
		Card lowest(Card floor);

		// Get sub-set of cards greater than input
		CardSet subsetGreater(Card card);

		// Removes all cards from the set
		void flush();


		// Remove card(s) form this set
		void remove(Card card);
		void remove(CardSet cards);


		void shuffle();
};

#include "./card.cpp"