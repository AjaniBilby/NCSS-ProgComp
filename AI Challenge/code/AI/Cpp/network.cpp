#include "./network.hpp"


Network::Network(){
	this->topology.resize(0);
	this->weights.resize(0);
};
Network::Network(Network *other){
	this->topology = other->topology;
	this->weights = other->weights;
};
Network::Network(NetworkTopology structure){
	this->topology = structure;
	this->weights.resize( structure.size()-1 );

	int len = this->weights.size();
	for (int i=0; i<len; i++){
		this->weights[i].resize(structure[i], structure[i+1], 0);
	}
};
Network::Network(std::vector<Matrix> form){
	this->weights = form;

	int len = this->weights.size();
	this->topology.resize(len+1);
	this->topology[0] = form[0].columns;

	for (int i=0; i<len; i++){
		this->topology[i] = form[i+1].rows;
	}
};


// Forward propergate the matrix through the network
Matrix Network::forward(Matrix input){
	Matrix out = input;

	int len = this->weights.size();
	for (int i=0; i<len; i++){
		out = out.dot(this->weights[i]);
	}

	return out;
};


// Create a child with this network and other
Network Network::reproduce(Network *other){
	NetworkTopology structure;
	std::vector<Matrix>  form;


	// Pick the number of columns from either parents
	// Vary the value slightly
	int      choice = rand() % 2;
	int    colsSelf = this->topology.size();
	int   colsOther = other->topology.size();
	int columnCount = 0;
	if (choice == 0){
		columnCount = colsSelf;
	}else{
		columnCount = colsOther;
	}
	columnCount += rand() % 3 - 1;
	if (columnCount < 2){
		columnCount = 2;
	}
	structure.resize(columnCount);
	form.resize(columnCount-1);



	// Determine the depth for each
	std::vector<int> opts;
	int temp = 0;
	std::cout << "Structure: ";
	for (unsigned int x=0; x<columnCount; x++){
		opts.resize(0);

		if (x < colsSelf){
			opts.push_back(this->topology[x]);
		}
		if (x< colsOther){
			opts.push_back(other->topology[x]);
		}

		// Pick an option from either parent when valid
		if (opts.size() > 0){
			temp = opts[ rand() % opts.size() ];
		}else{
			temp = 1;
		}

		// Vary
		temp += rand() % 3 - 1;
		if (temp < 1){
			structure[x] = 1;
		}else{
			structure[x] = temp;
		}
		std::cout << structure[x] << " ";
	}
	// Ensure that the input and output sizes do not change
	structure[0]           = this->topology[0];
	structure[columnCount] = this->topology[colsSelf];

	std::cout << std::endl << std::endl;

	// Determine the weights
	float delta = 0;
	for (unsigned int x=1; x<columnCount; x++){
		form[x-1].resize(structure[x-1], structure[x], 0);

		for (unsigned int y1=0; y1<structure[x-1]; y1++){
			for (unsigned int y2=0; y2<structure[x]; y2++){
				opts.resize(0);
				std::cout << x << ' ' << y1 << ' ' << y2 << " | ";
				std::cout << form[x-1].columns << ' '  << form[x-1].rows << std::endl;

				// Get options
				std::cout << 'Z';
				if (x < colsSelf){
					std::cout << '1';
					if (y1 < this->topology[x-1]){
						std::cout << '2';
						if (y2 < this->topology[x]){
							std::cout << '3';
							opts.push_back(this->weights[x-1][y1][y2]);
							std::cout << '4';
						}
					}
				}
				std::cout << 'A';
				if (x < colsOther){
					std::cout << '1';
					if (y1 < other->topology[x-1]){
						std::cout << '2';
						if (y2 < other->topology[x]){
							std::cout << '3';
							opts.push_back(other->weights[x-1][y1][y2]);
							std::cout << '4';
						}
					}
				}
				std::cout << 'B';

				// Select one
				if (opts.size() > 0){
					form[x-1][y1][y2] = opts[ rand() % opts.size()];
				}

				std::cout << 'C';

				// Vary
				delta  = rand();
				delta /= RAND_MAX;
				form[x-1][y1][y2] += delta;
				std::cout << 'D' << std::endl;
			}
		}
	}

	return Network(form);
};



// Helper functions for logging
int Network::length(){
	return this->topology.size();
};
int Network::neurons(){
	int tally = 0;

	int len = this->topology.size();
	for (int i=0; i<len; i++){
		tally += this->topology[i];
	}

	return tally;
};

void Network::print(){
	int xLen = this->weights.size();
	int y1Len = 0;
	int y2Len = 0;
	std::cout << "Network\n---------\n";
	for (int x=0; x<xLen; x++){
		std::cout << this->weights[x].toString();
	}
	std::cout << "---------\n";
};