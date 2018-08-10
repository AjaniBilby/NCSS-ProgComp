#pragma once

#include <iostream>

#include "./network.hpp"
#include "./card.hpp"

class Bot{
	private:
		CardSet select(CardSet beat, int handsizes[4]);
	public:
		Network network;
		bool simplex;

		CardSet   hand;
		CardSet unseen;

		Bot();
		Bot(bool simple = false);

		void reset();
		void observe(CardSet play);
		void append(Card card);

		CardSet play(bool is_start_of_round, CardSet play_to_beat, int handsizes[4]);

		bool has(Card card);
		int  handSize();
		bool hasCards();
};



#include "./bot.cpp"