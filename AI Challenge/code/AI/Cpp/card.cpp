#include "card.hpp"



Card::Card(){
	this->invalid = true;
	this->rankID = 9999;
	this->suitID = 9999;
}
Card::Card(CardText str){
	this->rankID = -1;
	for (unsigned int i=0; i<13; i++){
		if (str[0] == cardsRank[i]){
			this->rankID = i;
			break;
		}
	}

	this->suitID = -1;
	for (unsigned int i=0; i<4; i++){
		if (str[1] == cardsSuit[i]){
			this->suitID = i;
			break;
		}
	}

	if (this->suitID != -1 && this->rankID != -1){
		this->invalid = false;
	}else{
		this->invalid = true;
	}
}
Card::Card(int rank, int suit){
	this->rankID = rank;
	this->suitID = suit;
	if (rank < 5 && suit < 14){
		this->invalid = true;
	}else{
		this->invalid = false;
	}
};

std::string Card::toString(){
	std::string out;
	out.resize(2);
	out[0] = cardsRank[this->rankID];
	out[1] = cardsSuit[this->suitID];

	return out;
};

bool Card::lessThan(Card *other){
	if (other->invalid == true){
		return false;
	}

	if (this->rankID < other->rankID){
		return true;
	}
	if (this->suitID < other->suitID){
		return true;
	}

	return false;
};
bool Card::greaterThan(Card *other){
	if (other->invalid == true){
		return true;
	}

	if (this->rankID > other->rankID){
		return true;
	}
	if (this->suitID > other->suitID){
		return true;
	}

	return false;
};
bool Card::equalTo(Card *other){
	if (other->invalid == this->invalid){
		return true;
	}

	if (this->rankID != other->rankID){
		return false;
	}
	if (this->suitID != other->suitID){
		return false;
	}

	return true;
};











// Initlizers
CardSet::CardSet(bool autoFill){
	if (autoFill){
		this->remain.resize(52);
		unsigned int i=0;
		for (int s=0; s<4; s++){
			for (int r=0; r<13; r++){
				this->remain[i] = Card(r, s);
				i++;
			}
		}
	}else{
		this->remain.resize(0);
	}
};
CardSet::CardSet(CardSet *other){
	unsigned int length = other->size();
	this->remain.resize(length);

	for (unsigned int i=0; i<length; i++){
		this->remain[i] = other->remain[i];
	}
}



// Nice Handles
Card& CardSet::operator[](int x){
	return this->remain[x];
};
size_t CardSet::capacity(){
	return this->remain.capacity();
};
size_t CardSet::size(){
	return this->remain.size();
};
void CardSet::append(Card card){
	this->remain.push_back(card);
};
void CardSet::flush(){
	this->remain.resize(0);
};



float CardSet::strength(Card card){
	unsigned int betterThan = 0;
	unsigned int length = this->size();

	for (unsigned int i=0; i<length; i++){
		if (this->remain[i].lessThan(&card)){
			betterThan++;
		}
	}

	return betterThan / length;
};

bool CardSet::contains(Card card){
	unsigned int length = this->remain.size();
	for (unsigned int i=0; i<length; i++){
		if (card.equalTo( &this->remain[i] ) == true){
			return true;
		}
	}

	return false;
};

Card CardSet::lowest(Card lowest){
	Card opt = Card();

	unsigned int length = this->remain.size();
	for (unsigned int i=0; i<length; i++){
		if (lowest.lessThan(&this->remain[i]) && opt.greaterThan(&this->remain[i])){
			opt = this->remain[i];
		}
	}

	return opt;
}

CardSet CardSet::subsetGreater(Card card){
	CardSet out = CardSet();

	unsigned int length = this->remain.size();
	for (unsigned int i=0; i<length; i++){
		if (this->remain[i].greaterThan(&card)){
			out.append(this->remain[i]);
		}
	}

	return out;
}



void CardSet::remove(Card card){
	int i=-1;
	int length = this->remain.size();

	// Find the index of the card
	for (int j=0; j<length; j++){
		if (card.equalTo(&this->remain[j])){
			i = j;
			break;
		}
	}

	// The card doesn't exist already
	if (i == -1){
		return;
	}

	// Splice the element form the list
	for (int j=i+1; j<length; j++){
		this->remain[j-1] = this->remain[j];
	}
	this->remain.resize(length -1);

	return;
}
void CardSet::remove(CardSet cards){
	// Remove each card in the set from this set
	int length = cards.size();
	for (int i=0; i<length; i++){
		this->remove(cards[i]);
	}
}



void CardSet::shuffle(){
	CardSet nxt(false);
	
	int i=0;
	int length = this->size();
	while (length > 0){
		i = rand() % length;

		// Transfer the card
		nxt.append( this->remain[i] );

		// Remove it from this set
		this->remove(this->remain[i]);
		length--;
	}

	// make this deck the shuffled one
	*this = nxt;
};