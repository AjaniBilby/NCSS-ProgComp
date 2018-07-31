from card import Card, cardDeck
from bot import Bot
import random

def trick(weights):
	# Duplicated the list and shuffle
	deck = [val for val in cardDeck]
	random.shuffle( deck )

	players = []
	for i in range(0, 4):
		players.append( Bot() )
		players[i].ResetHand(deck[i*13 : (i+1)*13])
		players[i].out = False
		players[i].weight = weights[i]


	start_round = True
	playOccured = True
	finished    = False
	beat = []
	while playOccured and not finished:
		playOccured = False

		for i, player in enumerate(players):
			if player.out == True:
				continue

			# Get the bot's play
			play = player.Play(
				start_round,
				beat,
				[len(player.hand.remain) for player in players]
			)

			# Did the bot pass?
			if len(play) == 0:
				player.out = True
				continue
			playOccured = True
			beat = play

			if len(player.hand.remain) < 1:
				finished = True
				break

			# Allow other bots to know what happened
			for other in players:
				other.Observe(beat, 0)

		start_round = False

	scores = []
	for player in players:
		scores.append(len(player.hand.remain))

	return scores


def learn():
	learningRate = 0.05
	bestCards  = 0
	bestWeight = [1, 1, 1, 1]
	weights = [
		[1, 1, 1, 1],
		[1, 1, 1, 1],
		[1, 1, 1, 1],
		[1, 1, 1, 1]
	]

	res = []

	file = open("log.csv", "w+")
	file.write('Weight 1,Weight 2,Weight 3,Weight 4\n')

	for rerun in range(0, 10000000):
		if rerun % 10000 == 0:
			print(rerun)

		for i in range(0, 4):
			for j in range(0, 4):
				# Vary from best weight by +/- 0.5
				weights[i][j] = bestWeight[j] + (random.random()-0.5)*learningRate

		res = [0, 0, 0, 0]
		for i in range(0, 10):
			out = trick(weights)
			for i in range(0, 4):
				res[i] += out[i]

		# Select the best bot for mutation
		bestCards  = res[0]
		bestWeight = weights[0]
		for i in range(1, 3):
			if res[i] < bestCards:
				bestCards = res[i]
				bestWeight = weights[i]
		
		# Remove some data points to help acess
		if rerun % 1000 == 0:
			file.write(','.join([str(val) for val in bestWeight]) +'\n')

	file.close()
	print(','.join([str(val) for val in bestWeight]) +'\n')

learn()