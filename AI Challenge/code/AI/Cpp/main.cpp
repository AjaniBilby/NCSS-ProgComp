
#include <iostream>
#include "network.hpp"
int main(){
	std::vector<unsigned int> topology;
	topology.resize(3);
	topology[0] = 3;
	topology[1] = 2;
	topology[2] = 1;

	Network net(topology);
	net.print();
	net = net.reproduce(&net);

	net.print();
};