#pragma once

#include "./matrix.hpp"

#include <streambuf>
#include <fstream>

#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>

typedef std::vector<unsigned int> NetworkTopology;

#include <random>
std::random_device random;
auto RANDOM_MAX = random.max();

class Network{
	public:
		std::vector<unsigned int> topology;
		std::vector<Matrix> weights;

		// Create an empty network
		Network();
		// Duplicate another network
		Network(Network *other);
		// Create a network with the specified topology
		Network(NetworkTopology);
		// Create a network using the weights to generate the topology
		Network(std::vector<Matrix>);
		// From flatterned state
		Network(std::vector<double>, NetworkTopology);

		// Propogate data through the network
		Matrix forward(Matrix input);
		// Make a new network using two as reference
		// + Mutate
		Network reproduce(Network *other);


		// Helper functions for logging
		int neurons();
		int length();

		std::string toString();

		// Convert network weights to 1D form
		std::vector<double> flatten();

		// Save the network to a file
		void save(std::string filepath);
		// Load the network from a file
		void load(std::string filepath);
};


#include "./network.cpp"


