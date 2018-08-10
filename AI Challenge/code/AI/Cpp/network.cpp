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

	int len = structure.size()-1;
	this->weights.resize( len );

	for (int i=0; i<len; i++){
		this->weights[i].resize(structure[i+1], structure[i], 0);
	}
};
Network::Network(std::vector<Matrix> form){
	this->weights = form;

	int len = this->weights.size()+1;
	this->topology.resize(len);
	
	this->topology[0] = form[0].rows;
	for (int i=1; i<len; i++){
		this->topology[i] = form[i-1].columns;
	}
};


// Forward propergate the matrix through the network
Matrix Network::forward(Matrix input){
	if (input.columns != this->topology[0] || input.rows != 1){
		std::cerr << "Invalid input matrix" << std::endl;

		return Matrix(this->topology[ this->topology.size()-1 ], 1, 0);
	}

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
	int      choice = random() % 2;
	int    colsSelf = this->topology.size();
	int   colsOther = other->topology.size();
	int columnCount = 0;
	if (choice == 0){
		columnCount = colsSelf;
	}else{
		columnCount = colsOther;
	}
	columnCount += int(random() % 4) - 1;
	// Ensure that input and output columns exist
	if (columnCount < 2){
		columnCount = 2;
	}
	structure.resize(columnCount);
	form.resize(columnCount-1);




	// Determine the depth for each
	std::vector<int> opts;
	int temp = 0;
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
			temp = opts[ random() % opts.size() ];
		}else{
			temp = 1;
		}

		// Vary
		temp += int(random() % 4) - 1;
		if (temp < 1){
			structure[x] = 1;
		}else{
			structure[x] = temp;
		}
	}
	// Ensure that the input and output sizes do not change
	structure[0]             = this->topology[0];
	structure[columnCount-1] = this->topology[colsSelf-1];




	// Determine the weights
	double delta = 0;
	for (unsigned int x=1; x<columnCount; x++){
		form[x-1].resize(structure[x], structure[x-1], 0);

		for (unsigned int y1=0; y1<structure[x-1]; y1++){
			for (unsigned int y2=0; y2<structure[x]; y2++){
				opts.resize(0);

				// Get options
				if (x < colsSelf){
					if (y1 < this->topology[x-1]){
						if (y2 < this->topology[x]){
							opts.push_back(this->weights[x-1][y1][y2]);
						}
					}
				}
				if (x < colsOther){
					if (y1 < other->topology[x-1]){
						if (y2 < other->topology[x]){
							opts.push_back(other->weights[x-1][y1][y2]);
						}
					}
				}


				// Select one
				if (opts.size() > 0){
					form[x-1][y1][y2] = opts[ random() % opts.size()];
				}

				// Vary
				delta  = random();
				delta /= RANDOM_MAX;
				form[x-1][y1][y2] += delta;
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

std::string Network::toString(){
	int xLen = this->weights.size();
	int y1Len = 0;
	int y2Len = 0;
	
	int len = this->topology.size();
	std::string str = "Network ("+std::to_string(this->topology[0]);
	for (int i=1; i<len; i++){
		str += ", " + std::to_string(this->topology[i]);
	}
	str += "){\n";

	std::string temp;
	int tempLen = 0;
	for (int x=0; x<xLen; x++){
		temp = this->weights[x].toString();
		tempLen = temp.size();

		// Indent the result
		str += "  ";
		for (int i=0; i<tempLen; i++){
			if (temp[i] == '\n'){
				str += "\n  ";
			}else{
				str += temp[i];
			}
		}
		str += '\n';
	}
	str += "}";

	return str;
};