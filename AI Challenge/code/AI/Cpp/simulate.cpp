#include "./simulate.hpp"

GameResult playRound(Bot *players[4]){
	CardSet deck(true);
	deck.shuffle();

	// Empty player's hands
	for (int i=0; i<4; i++){
		players[i]->reset();
	}

	// Distribute the cards
	int length = deck.size();
	for (int i=0; i<length; i+=4){
		for (int j=0; j<length; j++){
			players[j]->append(deck[i+j]);
		}
	}



	// Round info
	int            i = 0;              // Current player
	int    decompose = 3;              // How many people able to pass in a row
	bool start_round = true;           // Is this the start of the round
	bool     playing = false;          // Is the game finished
	int    handSizes[4];               // Number of cards in each player's hand
	CardSet beat(false);               // The current best play
	CardSet play(false);               // The current play

	// Find which player has
	Card startCard(CardText{'3', 'D'});
	for (int j=0; j<4; j++){
		if (players[j]->has(startCard) == true){
			i = j-1;
			break;
		}
	}

	// Simulate game turn by turn
	while (playing){
		// Loop around the group
		i++;
		if (i > 3){
			i = 0;
		}

		// If all players passed
		// Reset the card to beat
		if (decompose == 0){
			beat      = CardSet(false);
			decompose = 3;
		}

		// Get play hand sizes
		for (int j=0; j<4; j++){
			handSizes[j] = players[j]->handSize();
		}

		play = players[i]->play(
			start_round,
			beat,
			handSizes
		);

		// Handle players passing
		if (play.size() == 0){
			decompose -= 1;
			continue;
		}

		// Check that the play is valid
		if (play[0].lessThan(&beat[0])){
			std::cerr << "Invalid play: " << play[0].toString() << " < " << beat[0].toString();
		}

		// This player has played their last card
		if (players[i]->hasCards() == false){
			playing = false;
			continue;
		}

		// Let all other player's know what happened
		for (int i=0; i<4; i++){
			players[i]->observe(play);
		}
	};



	// Calculate scores
	int score[4];
	int cards = 0;
	int winnerIndex = 0;
	int total = 0;
	for (int i=0; i<4; i++){
		cards = players[i]->handSize();

		if (cards == 0){
			score[i] = 0;
			winnerIndex  = i;
		}else if (cards < 10){
			score[i] = -1 * cards;
		}else if (cards <= 12) {
			score[i] = -2 * cards;
		}else{
			score[i] = -3 * cards;
		}
		total -= score[i];
	}
	score[winnerIndex] = total;


	// Parse results
	GameResult res;
	res.winnerIndex = winnerIndex;
	res.scores[0] = score[0];
	res.scores[1] = score[1];
	res.scores[2] = score[2];
	res.scores[3] = score[3];
	return res;
};

GameResult playGame(Bot *players[4]){
	GameResult res;
	GameResult rnd;


	// Play 10 rounds stopping if any score reaches 100
	bool finished = false;
	for (int i=0; i<10; i++){
		if (finished){
			break;
		}

		rnd = playRound(players);
		for (int j=0; j<4; j++){
			res.scores[j] += rnd.scores[j];

			if (res.scores[j] >= 100){
				finished = true;
			}
		}
	}


	// Determine the winner
	int bestIndex = 0;
	for (int i=0; i<4; i++){
		if (res.scores[i] > res.scores[bestIndex]){
			bestIndex = i;
		}
	}
	res.winnerIndex = bestIndex;


	return res;
};



void print(
	int rerun,
	int cycles,
	int gameCount,
	int winCount,
	int generation,
	int networkLinear,
	int networkColumns,

	float winRate,
	float smoothedWinRate,

	int tally[4]
){
	std::string out;
	winRate         *= 100;
	smoothedWinRate *= 100;


	float deviation = 0;
	int lowest  = tally[0];
	int highest = tally[0];
	int total   = 0;
	for (int i=0; i<4; i++){
		if (lowest > tally[i]){
			lowest = tally[i];
		}
		if (highest < tally[i]){
			highest = tally[i];
		}

		total += tally[i];
	}
	deviation = (highest - lowest);
	deviation /= total * 100;


	out  = " Status;\n";
	out += "  - Iterations      : " + std::to_string(rerun) + " of " + std::to_string(cycles) + '\n';
	out += "  - Generation      : " + std::to_string(generation) + '\n';
	out += " Game Meta;\n";
	out += "  - Avg. Win        : " + std::to_string(winRate).substr(0, 5)   + "%\n";
	out += "  - Games           : " + std::to_string(gameCount)              +  '\n';
	out += "  - Wins VS Simplex : " + std::to_string(winCount)               +  '\n';
	out += "  - Win Rate        : " + std::to_string(winCount).substr(0, 5)  + "%\n";
	out += "  - Deviation       : " + std::to_string(deviation).substr(0, 5) + "%\n";

	std::cout << out << std::endl;
}



void train(){
	std::cout<< "Training..." << std::endl;

	Bot playerA = Bot();
	std::cout << "al";
	Bot playerB = Bot();
	std::cout << "al";
	Bot playerC = Bot();
	std::cout << "al";
	Bot playerD = Bot();
	std::cout << "al";

	std::cout<< "init"<< std::endl;

	Bot* players[] = {nullptr, nullptr, nullptr, nullptr};
	std::cout << "bl";
	std::cout << players[0];
	players[0] = &playerA;
	std::cout << "bl";
	players[1] = &playerB;
	std::cout << "bl";
	players[2] = new Bot();
	std::cout << "bl";
	players[3] = new Bot();
	std::cout << "bl";

	std::cout<< "Initilized Bots..." << std::endl;

	Network bestNetwork( players[0]->network );
	Network prevNetwork( players[0]->network );

	std::cout<< "Initilized Networks..." << std::endl;

	int   bestBot = 0;
	int bestScores[4];

	int      cycles = 1000;
	int logInterval = cycles/100;
	int  generation = 0;
	int       games = 0;
	int        wins = 0;
	float   winRate = 0;
	float  sWinRate = 0;


	GameResult rnd;
	int temp = 0;
	for (int rerun=0; rerun<cycles; rerun++){
		std::cout << rerun << std::endl;

		// Spawn networks
		for (int i=0; i<4; i++){
			if (i == bestBot){
				continue;
			}
			players[i]->network = bestNetwork.reproduce(&prevNetwork);
		}

		// Reset round data
		rnd.winnerIndex = 0;
		rnd.scores[0] = 0;
		rnd.scores[1] = 0;
		rnd.scores[2] = 0;
		rnd.scores[3] = 0;

		// Best of 10 games
		//  Count the wins
		for (int i=0; i<10; i++){
			temp = playGame(players).winnerIndex;
			rnd.scores[ temp ] += 1;
			games++;

			if (players[temp]->simplex == false){
				wins++;
			}
		}


		// Transfer breeding rights
		if (bestBot != rnd.winnerIndex){
			bestBot = rnd.winnerIndex;
			generation++;

			prevNetwork = bestNetwork;
			bestNetwork = players[bestBot]->network;

			bestScores[0] = rnd.scores[0];
			bestScores[1] = rnd.scores[1];
			bestScores[2] = rnd.scores[2];
			bestScores[3] = rnd.scores[3];
		}

		if (rerun % logInterval == 0){
			winRate  = wins;
			winRate /= games;
			sWinRate = (sWinRate + winRate)/2;

			print(
				rerun,
				cycles,
				games,
				wins, 
				generation,
				bestNetwork.weightCount(),
				bestNetwork.length(),
				winRate,
				sWinRate,
				bestScores
			);
		}
	}
}