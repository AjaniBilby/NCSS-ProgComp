#include "bot.hpp"



Bot::Bot(bool simple){
	std::cout << "Bot boot, + arg" << std::endl;

	this->simplex = simple;
	this->   hand = CardSet(false);
	this-> unseen = CardSet(false);

	if (simple == true){
		this->network = Network();
	}else{
		std::vector<unsigned int> topology;
		std::vector<float> weights;

		topology.resize(7);
		topology[0] = 7;
		topology[1] = 6;
		topology[2] = 5;
		topology[3] = 4;
		topology[4] = 3;
		topology[5] = 2;
		topology[6] = 1;

		this->network = Network( topology, weights );
	}
};
Bot::Bot(){
	std::cout << "Missing arg"<<std::endl;

	this->simplex = false;
	this->   hand = CardSet(false);
	this-> unseen = CardSet(false);

	std::vector<unsigned int> topology;
	std::vector<float> weights;

	topology.resize(7);
	topology[0] = 7;
	topology[1] = 6;
	topology[2] = 5;
	topology[3] = 4;
	topology[4] = 3;
	topology[5] = 2;
	topology[6] = 1;

	this->network = Network( topology, weights );
};


// Game Handles
void Bot::reset(){
	this->hand.flush();
	this->unseen = CardSet(true);
};
void Bot::observe(CardSet cards){
	this->unseen.remove(cards);
};
void Bot::append(Card card){
	this->hand.append(card);
}
bool Bot::has(Card card){
	return this->hand.contains(card);
};
bool Bot::hasCards(){
	return this->hand.size() > 0;
}
CardSet Bot::play(bool is_start_of_round, CardSet play_to_beat, int handsizes[4]){
	CardSet out = CardSet(false);
	Card card;

	// Play 3D to start the round if possible
	if (is_start_of_round){
		card = Card(CardText{'3', 'D'});
		if (this->hand.contains( card ) == true){

			// Remove the played card from hand
			out.append( card );
			this->hand.remove(card);
			return out;
		}
	}


	if (this->simplex){
		card = this->hand.lowest(play_to_beat[0]);
		if (card.invalid == true){
			return out;
		}

		// Remove the played card from hand
		out.append(card);
		this->hand.remove(card);
		return out;
	}


	// Remove the played cards
	out = this->select(play_to_beat, handsizes);
	this->hand.remove(out);
	return out;
};
int Bot::handSize(){
	return this->hand.size();
}



CardSet Bot::select(CardSet beat, int handsizes[4]){
	CardSet       opts = this->hand.subsetGreater(beat[0]);
	int         length = opts.size();
	float beatStrength = this->unseen.strength(beat[0]);

	CardSet out(false);


	// Special cases to save on compute
	if (length < 1){        // No more cards
		return out;
	}else if (length < 2){  // Only one card
		out.append(opts[0]);
		return out;
	}


	// Get the smallest hand size
	float shs = 14;
	for (int i=0; i<4; i++){
		if (handsizes[i] < shs){
			shs = handsizes[i];
		}
	}
	shs /= 13;


	// Info self
	float handPercent    = this->hand.size();
	handPercent /= 13;
	// Best play available
	int            bestI = 0;
	float bestConfidence = 0;
	// Item's stats
	float     confidence = 0;
	int         strength = 0;
	std::vector<float>  pipe;
	pipe.resize(7);

	for (unsigned int i=0; i<length; i++){
		strength = this->unseen.strength(opts[i]);

		pipe[0] = strength,              // Strength of this play
		pipe[1] = beatStrength,          // Strength of oponent's play
		pipe[2] = (strength/beatStrength), // Relative strengths

		pipe[3] = handPercent,           // Percent of my hand remain
		pipe[4] = shs,                   // Percent of openent hand remain
		pipe[5] = (handPercent/shs),       // Relative sizes of hands

		pipe[6] = (1/5),                   // Cards within play (relative to max play size)


		confidence = this->network.forward(pipe)[0];
		if (confidence > bestConfidence){
			bestConfidence = confidence;
			bestI          = i;
		}
	}

	
	// Allow the neural network to pass if necessary
	if (bestConfidence > 1){
		out.append(opts[bestI]);
		return out;
	}

	return out;
};