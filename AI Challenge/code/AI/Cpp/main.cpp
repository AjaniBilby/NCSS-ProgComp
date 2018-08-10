
#include <iostream>



#include "./simulate.hpp"
#include "./bot.hpp"
void TestGame(){
	Bot *players[] = {nullptr, nullptr, nullptr, nullptr};
	players[0] = new Bot(true);
	players[1] = new Bot(false);
	players[2] = new Bot(false);
	players[3] = new Bot(false);


	GameResult res = playGame(players);
	std::cout << "scores";
	for (int i=0; i<4; i++){
		std::cout << " " << res.scores[i];
	}
	std::cout << std::endl;

}



// #include "bot.hpp"
// #include "card.hpp"
// void TestBot(){
// 	CardSet deck = CardSet(true);
// 	Bot brain = Bot(false);
// 	brain.network = brain.network.reproduce(&brain.network);
	

// 	std::cout << brain.network.toString() << "\n\n\n";

// 	brain.reset();
// 	for (int i=2; i<13; i++){
// 		brain.append(deck[i]);
// 	}
// 	std::cout << brain.handSize() << std::endl;


// 	Matrix pipe = Matrix(7, 1, 1);
// 	pipe = brain.network.forward(pipe);


// 	CardSet pick = CardSet(false);
// 	pick.append(Card( CardText{'4', 'D'} ));

// 	int hands[4] = {13,13,13,12};
// 	pick = brain.play(false, pick, hands);
// 	std::cout << pick.toString() << std::endl;
// };







// #include "network.hpp"
// void ANNTest(){
// 	std::vector<unsigned int> topology;
// 	topology.resize(3);
// 	topology[0] = 3;
// 	topology[1] = 2;
// 	topology[2] = 1;

// 	Network parent(topology);
// 	std::cout << parent.toString() << std::endl;
// 	Network child = parent.reproduce(&parent);
// 	std::cout << child.toString() << std::endl;

// 	Matrix data = Matrix(3, 1, 1);

// 	data = child.forward(data);
// 	std::cout << data.toString();
// };









int main(){

	NetworkTopology topology;
	topology.resize(7);
	topology[0] = 7;
	topology[1] = 6;
	topology[2] = 5;
	topology[3] = 4;
	topology[4] = 3;
	topology[5] = 2;
	topology[6] = 1;
	Network net(topology);

	net = train(net);
	std::cout << "\n\n" << net.toString();

	return 0;
}