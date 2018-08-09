#include "./network-OLD.hpp"


Network::Network(){
	this->weights.resize(0);
	this->topology.resize(0);
	this->linearWeights.resize(0);
};
Network::Network(Network *other){
	this->weights       = other->weights;
	this->topology      = other->topology;
	this->linearWeights = other->linearWeights;
};
Network::Network(std::vector<unsigned int> topology, std::vector<float> weight){
	unsigned long xLen  = topology.size();
	unsigned long y1Len = 0;
	unsigned long y2Len = 0;

	this->topology = topology;
	this->linearWeights = weight;
	this->weights.resize(xLen);

	long underfill = 0;
	long weightLen = weight.size();

	unsigned long i=0;
	for (unsigned long x=1; x<xLen; x++){
		y1Len = topology[x];
		y2Len = topology[x-1];

		this->weights[x].resize(y1Len);
		for (unsigned long y1=0; y1<y1Len; y1++){

			this->weights[x][y1].resize(y2Len);
			for (unsigned long y2=0; y2<y2Len; y2++){
				if (i < weightLen){
					this->weights[x][y1][y2] = weight[i];
					i++;
					continue;
				}

				this->weights[x][y1][y2]  = rand();
				this->weights[x][y1][y2] /= RAND_MAX;
				underfill++;
			}
		}
	}

	if (underfill > 0){
		std::cerr << "Warn: not enough linear weights. Needs "<< underfill <<" more"<<std::endl;
	}
};
Network::Network(std::vector<unsigned int> topology, ANNWeights weight){
	this->topology = topology;
	this->weights = weight;

	// Resize the linear space
	unsigned long length = 0;
	unsigned long xLen  = topology.size();
	for (unsigned long i=1; i<topology.size(); i++){
		length += topology[i-1] * topology[i];
	}
	this->linearWeights.resize(length);


	// Read into linear space
	unsigned long y1Len = 0;
	unsigned long y2Len = 0;
	unsigned long i = 0;
	for (unsigned long x=0; x<xLen; x++){
		y1Len = this->weights[x].size();
		for (unsigned long y1=0; y1<y1Len; y1++){
			y2Len = this->weights[x][y1].size();
			for (unsigned long y2=0; y2<y2Len; y2++){
				this->linearWeights[i] = this->weights[x][y1][y2];
				i++;
			}
		}
	}
};

std::vector<float> Network::forward(std::vector<float> input){
	std::vector< std::vector<float> > neurons;

	if (input.size() != this->topology[0]){
		std::cerr << "Error: Invalid input data. ("<<this->topology[0]<<" != "<<input.size()<<")";
	}

	std::cout<<"Generating Neurons"<<std::endl;
	unsigned long xLen = this->topology.size();
	neurons.resize(xLen);
	for (unsigned long x=0; x<xLen; x++){
		neurons[x].resize(this->topology[x], 0);

		if (x == 0){
			unsigned long yLen = this->topology[x];
			for (unsigned long y=0; y<yLen; y++){
				neurons[x][y] = input[y];
			}
		}
	}
	std::cout<<"parsing forward"<<std::endl;


	unsigned long y1Len = 0;
	unsigned long y2Len = 0;
	for (unsigned long x=1; x<xLen; x++){
		y1Len = this->topology[x];
		y2Len = this->topology[x-1];

		for (unsigned long y1=0; y1<y1Len; y1++){
			for (unsigned long y2=0; y2<y2Len; y2++){
				std::cout << x << " " << y1 << ' ' << y2 << std::endl;

				neurons[x][y1] = neurons[x-1][y2] * this->weights[x-1][y1][y2];
			}
		}
	}


	unsigned long i=this->topology.size()-1;
	return neurons[i];
};


Network Network::reproduce(Network *other){	

	// Determine the number of columns
	int choice = rand() % 2;
	int columnCount = 0;
	if (choice == 0){
		columnCount = this->topology.size();
	}else{
		columnCount = other->topology.size();
	}
	// Mutate the column count
	columnCount += rand() % 3 - 1;
	if (columnCount < 3) {
		columnCount = 2;
	}


	// Determine the depth of each layer
	std::vector<int> opts;
	std::vector<unsigned int> columns;
	columns.resize(columnCount, 0);
	int colsSelf  = this->topology.size();
	int colsOther = other->topology.size();
	int temp = 0;
	for (unsigned int x=0; x<columnCount; x++){
		opts.resize(0);

		if (x < colsSelf){
			opts.push_back(this->topology[x]);
		}
		if (x < colsOther){
			opts.push_back(other->topology[x]);
		}

		if (opts.size() > 0){
			temp = opts[ rand() % opts.size() ];
		}else{
			temp = 1;
		}
		// Vary
		temp += rand() % 3 - 1;
		if (temp < 1){
			columns[x] = 1;
		}else{
			columns[x] = temp;
		}
	}


	// Determine the weights of each layer
	ANNWeights weights;
	weights.resize(columnCount);
	float delta = 0;
	for (unsigned int x=1; x<columnCount; x++){
		weights[x].resize(columns[x]);
		for (unsigned int y1=0; y1<columns[x]; y1++){
			weights[x][y1].resize(columns[x-1]);
			for (unsigned int y2=0; y2<columns[x-1]; y2++){
				opts.resize(0);

				if (x < colsSelf){
					if (y1 < this->weights[x].size()){
						if (y2 < this->weights[x][y1].size()){
							opts.push_back(this->weights[x][y1][y2]);
						}
					}
				}
				if (x < colsOther){
					if (y1 < other->weights[x].size()){
						if (y2 < other->weights[x][y1].size()){
							opts.push_back(other->weights[x][y1][y2]);
						}
					}
				}

				if (opts.size() > 0){
					weights[x][y1][y2] = opts[ rand() % opts.size() ];
				}else{
					weights[x][y1][y2] = 0;
				}
				
				// Vary
				delta =  rand();
				delta /= RAND_MAX;
				weights[x][y1][y2] += delta;
			}
		}
	}




	Network result(columns, weights);
	return result;
};



int Network::weightCount(){
	return this->linearWeights.size();
};
int Network::length(){
	return this->topology.size();
};