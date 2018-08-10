#pragma once

#include "./matrix.hpp"

#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>

typedef std::vector<unsigned int> NetworkTopology;

#include <random>
std::random_device random;
auto RANDOM_MAX = random.max();

class Network{
	private:
		std::vector<unsigned int> topology;
		std::vector<Matrix> weights;

	public:
		// Create an empty network
		Network();
		// Duplicate another network
		Network(Network *other);
		// Create a network with the specified topology
		Network(NetworkTopology);
		// Create a network using the weights to generate the topology
		Network(std::vector<Matrix>);

		// Propogate data through the network
		Matrix forward(Matrix input);
		// Make a new network using two as reference
		// + Mutate
		Network reproduce(Network *other);


		// Helper functions for logging
		int neurons();
		int length();

		std::string toString();
};


#include "./network.cpp"


