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
	players[0].neverPass = True


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
	learningRate = 0.01
	bestWeight = [3.8,-2.4,-2.6,3.2]
	weights = [
		[1,1,1,1],
		[1,1,1,1],
		[1,1,1,1],
		[1,1,1,1],
	]

	file = open("log.csv", "w+")
	file.write('Weight 1,Weight 2,Weight 3,Weight 4,Score 1, Score 2,Score 3, Score 4\n')

	for rerun in range(0, 100000):
		#  Bot[0] never passes
		for i in range(1, 4):
			for j in range(0, 4):
				# Vary from best weight by +/-learningRate
				weights[i][j] = ((random.random()-0.5)*2) *learningRate + bestWeight[j]



		# Determine the best player by number of wins over multiple games
		wins = [0,0,0,0]
		for i in range(0, 5):
			out = trick(weights)

			# Determine the winner
			lowestScore = 100
			for i in range(0, 4):
				if out[i] < lowestScore:
					lowestScore = out[i]

			# Give the winner a single point
			for i in range(0, 4):
				if out[i] == lowestScore:
					wins[i] += 1
					break



		# Select the best bot for mutation
		#  Bot[0] never passes
		greatest = 0
		for i in range(1, 3):
			if wins[i] > greatest:
				greatest = wins[i]
		# only transfer the winners data as apose to the current best
		#   saves compute time
		for i in range(1, 3):
			if wins[i] == greatest:
				bestWeight = [val for val in weights[i]]
				break


		
		# Remove some data points to help acess
		if rerun % 500 == 0:

			# Print and write out results
			print(str(rerun/1000) + '%')
			print(' ', ','.join([str(val) for val in bestWeight]))

			file.write(','.join([str(val) for val in bestWeight]))
			file.write(','+ ','.join([str(val/5) for val in wins]) +'\n')

	file.close()
	print(','.join([str(val) for val in bestWeight]) +'\n')

learn()