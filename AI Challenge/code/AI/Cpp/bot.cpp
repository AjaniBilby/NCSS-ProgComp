#include "bot.hpp"



Bot::Bot(bool simple){
	this->simplex = simple;
	this->   hand = CardSet(false);
	this-> unseen = CardSet(false);

	if (simple == true){
		this->network = Network();
	}else{
		NetworkTopology topology;

		topology.resize(7);
		topology[0] = 7;
		topology[1] = 6;
		topology[2] = 5;
		topology[3] = 4;
		topology[4] = 3;
		topology[5] = 2;
		topology[6] = 1;

		this->network = Network( topology );
	}
};


// Game Handles/Helpers
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
int Bot::handSize(){
	return this->hand.size();
};

// Bot's communication to the game
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


	// Don't use the ANN if simplex
	if (this->simplex){
		if (play_to_beat.size() > 0){
			card = this->hand.lowest(play_to_beat[0]);
		}else{
			card = this->hand.lowest(Card());
		}

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






CardSet Bot::select(CardSet beat, int handsizes[4]){
	bool hasBeat = beat.size()!=0;

	CardSet opts;
	if (hasBeat){
		opts = this->hand.subsetGreater(beat[0]);
	}else{
		opts = this->hand;
	}
	int         length = opts.size();

	CardSet out(false);


	// Get the smallest hand size
	float shs = handsizes[0];
	for (int i=1; i<4; i++){
		if (handsizes[i] < shs){
			shs = handsizes[i];
		}
	}
	shs /= 13;


	// // Info self
	float beatStrength = 0;
	if (hasBeat){
		beatStrength = this->unseen.strength(beat[0]);
	}
	float handPercent  = this->hand.size();
	      handPercent /= 13;
	// Best play available
	int            bestI = 0;
	float bestConfidence = 0;
	// Item's stats
	float     confidence = 0;
	float       strength = 0;
	Matrix pipe = Matrix(7, 1, 1);
	Matrix res;

	for (unsigned int i=0; i<length; i++){
		strength = this->unseen.strength(opts[i]);

		pipe[0][0] = strength;              // Strength of this play
		pipe[0][1] = beatStrength;          // Strength of oponent's play
		if (beatStrength > 0){
			pipe[0][2] = (strength/beatStrength); // Relative strengths
		}else{
			pipe[0][2] = 1;
		}

		pipe[0][3] = handPercent;           // Percent of my hand remain
		pipe[0][4] = shs;                   // Percent of openent hand remain
		pipe[0][5] = handPercent/shs;       // Relative sizes of hands

		pipe[0][6] = float(1) / 5;                   // Cards within play (relative to max play size)

		res = this->network.forward(pipe);
		confidence = ( res )[0][0];
		if (confidence > bestConfidence){
			bestConfidence = confidence;
			bestI          = i;
		}
	}
	
	// Allow the neural network to pass if necessary
	if (bestConfidence > 0){
		out.append(opts[bestI]);
		return out;
	}

	return out;
};