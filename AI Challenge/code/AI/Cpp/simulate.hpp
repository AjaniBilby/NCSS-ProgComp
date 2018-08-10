#pragma once

#include "./bot.hpp"
#include "./card.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <chrono>


struct GameResult{
	int winnerIndex;
	int scores[4];
};


#include "./simulate.cpp"