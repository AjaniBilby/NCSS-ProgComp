#include "./simulate.hpp"

GameResult playRound(Bot* players[4]){
	CardSet deck(true);
	deck.shuffle();


	// Empty player's hands
	for (int i=0; i<4; i++){
		players[i]->reset();
	}

	// Distribute the cards
	int length = deck.size();
	for (int i=0; i<length; i+=4){
		for (int j=0; j<4; j++){
			players[j]->append(deck[i+j]);
		}
	}



	// Round info
	int            i = 0;              // Current player
	int    decompose = 3;              // How many people able to pass in a row
	bool start_round = true;           // Is this the start of the round
	bool     playing = true;          // Is the game finished
	int handSizes[4] = {13,13,13,13};  // Number of cards in each player's hand
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

	int passes = 0;

	// Simulate game turn by turn
	while (playing){
		// Detect pass loops
		if (passes > 8){
			std::cerr << "Warn: Pass loop detected, terminating round" << std::endl;
			break;
		}


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

		play = players[i]->play(
			start_round,
			beat,
			handSizes
		);
		start_round = false;

		// Handle players passing
		if (play.size() == 0){
			decompose -= 1;
			passes++;
			continue;
		}
		passes = 0;

		// Record the number of cards for each player
		handSizes[i] -= play.size();

		// Check that the play is valid
		if (beat.size() > 1){
			if (play[0].lessThan(&beat[0])){
				std::cerr << "Invalid play: " << play[0].toString() << " < " << beat[0].toString();
			}
		}
		beat = play;

		// This player has played their last card
		if (handSizes[i] < 1){
			playing = false;
			continue;
		}

		// Let all other player's know what happened
		for (int i=0; i<4; i++){
			players[i]->observe(play);
		}
	};



	// Calculate scores
	GameResult res;
	res.winnerIndex = 0;
	res.scores[0] = 0;
	res.scores[1] = 0;
	res.scores[2] = 0;
	res.scores[3] = 0;
	int cards = 0;
	int total = 0;
	for (int i=0; i<4; i++){
		cards = handSizes[i];

		if (cards == 0){
			res.scores[i] = 0;
		}else if (cards < 10){
			res.scores[i] = -1 * cards;
		}else if (cards <= 12) {
			res.scores[i] = -2 * cards;
		}else{
			res.scores[i] = -3 * cards;
		}
		total -= res.scores[i];

		if (res.scores[i] > res.scores[res.winnerIndex]){
			res.winnerIndex = i;
		}
	}
	// Sum of total scores - their score = sum of other players scores
	res.scores[res.winnerIndex] = total + res.scores[res.winnerIndex];


	return res;
};

GameResult playGame(Bot* players[4]){
	GameResult res;
	GameResult rnd;

	res.winnerIndex = 0;
	res.scores[0]   = 0;
	res.scores[1]   = 0;
	res.scores[2]   = 0;
	res.scores[3]   = 0;


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
	for (int i=1; i<4; i++){
		if (res.scores[i] > res.scores[bestIndex]){
			bestIndex = i;
		}
	}
	res.winnerIndex = bestIndex;


	return res;
};





Network train(Network reference){
	// Create bots
	Bot *player[] = {nullptr, nullptr, nullptr, nullptr};
	player[0] = new Bot(true);
	player[1] = new Bot(false);
	player[2] = new Bot(false);
	player[3] = new Bot(false);

	
	// Create breeders
	Network bestNetwork = reference;
	Network prevNetwork = reference;
	int bestIndex       = 0;


	// Training / Log info
	int       games = 0;
	int        wins = 0;
	int      cycles = 5000;
	int logInterval = 50; // 500 long points
	int generations = 0;
	int    tally[4] = {0,0,0,0};
	int   scores[4] = {0,0,0,0};
	float   winRate = 1;
	float  sWinRate = 0;
	int   highScore = 0;
	int    lowScore = 0;
	int  totalScore = 0;
	float deviation = 0;
	float remaining = 0;
	std::string str;


	// CSV file
	std::ofstream csv("log.csv");

	// Duration
	clock_t begin_time = clock();
	float duration;


	// Temp data
	GameResult out;
	int cBest = 0;
	int tick  = 0;


	for (int rerun=0; rerun<cycles; rerun++){

		// Mutate bots
		for (int i=0; i<4; i++){
			if (i == bestIndex || player[i]->simplex == true){
				continue;
			}

			player[i]->network = bestNetwork.reproduce(&prevNetwork);
		}



		// Best of 5 to remove fluke
		scores[0] = 0;
		scores[1] = 0;
		scores[2] = 0;
		scores[3] = 0;
		for (int rnd=0; rnd<5; rnd++){
			out = playGame(player);
			scores[out.winnerIndex]++;
			games += 1;

			for (int i=0; i<4; i++){
				tally[i] += out.scores[i];
			}
			// tally[out.winnerIndex]++;

			if (player[out.winnerIndex]->simplex == false){
				wins++;
			}
		}


		// Determine which player one the most games
		cBest = -1;
		for (int i=0; i<4; i++){
			// Ignore not ANNs
			if (player[i]->simplex == true){
				continue;
			}

			if (cBest == -1){
				cBest = i;
				continue;
			}

			if (scores[i] > scores[cBest]){
				cBest = i;
			}
		}


		// If this is a new champion, give them breeding rights
		if (cBest != bestIndex){
			prevNetwork = bestNetwork;
			bestNetwork = player[cBest]->network;
			bestIndex = cBest;

			generations++;
			std::cout << "   New Generation" << std::endl;
		}


		if (tick >= logInterval){
			tick = 0;

			winRate = float(wins) / games;
			sWinRate = (winRate + sWinRate) / 2;

			duration = float(clock() - begin_time) / CLOCKS_PER_SEC;
			remaining = (duration / rerun) * (cycles - rerun);

			lowScore   = tally[0];
			highScore  = tally[0];
			totalScore = 0;
			for (int i=0; i<4; i++){
				if (lowScore > tally[i]){
					lowScore = tally[i];
				}
				if (highScore < tally[i]){
					highScore = tally[i];
				}

				totalScore += abs(tally[i]);
			}
			deviation = float(abs(highScore - lowScore)) / totalScore;


			str  = "\n\nStatus;\n";
			str += "  - Iteration       : " + std::to_string(rerun) + " of " + std::to_string(cycles) + "\n";
			str += "  - Generation      : " + std::to_string(generations) + "\n";
			str += "  - Duration        : " + std::to_string(duration) + " sec\n";
			str += "  - Estimate Time   : " + std::to_string(remaining) + " mins\n";
			str += "Games;\n";
			str += "  - Avg. Win        : " + std::to_string(sWinRate*100).substr(0,4) + "%\n";
			str += "  - Games Played    : " + std::to_string(games)                           +  "\n";
			str += "  - Wins VS Simplex : " + std::to_string(wins)                            +  "\n";
			str += "  - Win Rate        : " + std::to_string(winRate*100).substr(0,4)         + "%\n";
			
			str += "  - Tally           :";
			for (int i=0; i<4; i++){
				str += " " + std::to_string( tally[i] );
			}
			str += "\n";

			str += "  - Deviation       : " + std::to_string( deviation*100 ).substr(0,4)     + "%\n";

			str += "ANN;\n";
			str += "  - Topology        :";
			int len = bestNetwork.topology.size();
			for (int i=0; i<len; i++){
				str += " "+std::to_string(bestNetwork.topology[i]);
			}
			str += "\n";

			std::cout << str;



			csv << generations << ",";
			csv << winRate << ",";
			csv << deviation << "\n";
			csv.flush();



			tally[0] = 0;
			tally[1] = 0;
			tally[2] = 0;
			tally[3] = 0;
			wins = 0;
			games = 0;

			bestNetwork.save("network.dat");
		}
		tick++;
	}

	std::cout << "\aTranning complete;"<<std::endl;
	std::cout << "  Generations: " << generations << std::endl;
	bestNetwork.save("network.dat");
	csv.close();

	return bestNetwork;
};