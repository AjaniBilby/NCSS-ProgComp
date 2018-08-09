#pragma once

#include "./bot.hpp"

#include <iostream>
#include <string>
#include <vector>


struct GameResult{
	int winnerIndex;
	int scores[4];
};


void print(
	int rerun,
	int cycles,
	int gameCount,
	int winCount,
	int generation,
	int networkLinear,
	int networkColumns,

	float winRate,
	float smoothedWinRate,

	int tally[4]
);


#include "./simulate.cpp"