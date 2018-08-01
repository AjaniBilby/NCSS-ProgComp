from card import Card, cardDeck
from bot import Bot
import random
import time

def playRound(players):
	# Generate a new shuffled deck
	deck = [val for val in cardDeck]
	random.shuffle( deck )

	# Divide the deck up into player's hands
	for i in range(0, 4):
		players[i].ResetHand( deck[i*13 : (i+1)*13] )

	# Setup round info
	i = 0                 # Start the round at the previous winner
	decompose   = 3       # Stop the round after all player's passed in a row
	start_round = True    # Is this the start of the game
	finished    = False   
	beat = []

	# Find the starting player
	startCard = Card('3D')
	for j in range(0, 4):
		if players[i].hand.contains(startCard):
			i = j
			break

	while not finished:
		# Loop around
		if i > 3:
			i = 0

		# All other players passed
		# Reset the card(s) to beat
		if decompose == 0:
			beat = []
			decompose = 3

		# Allow the current player to play
		play = players[i].Play(
			start_round,
			beat,
			[len(player.hand.remain) for player in players]
		)
		start_round = False

		# Did the bot pass?
		if len(play) == 0:
			decompose -= 1
			continue

		# They made a play, reset decomposure
		decompose = 3

		# Check if the move was valid
		if len(beat) > 0:
			if play[0].lessThan(beat[0]):
				raise ValueError("Invalid play: "+str(play[0])+':'+str(play[0].worth)+' is not greater than '+str(beat[0])+':'+str(beat[0].worth))
		beat = play

		# See if the current player just played their last card
		#  Thus, end game
		if len(players[i].hand.remain) < 1:
			finished = True
			break

		# Allow other bots to know what happened
		for player in players:
			player.Observe(beat, 0)
		
		# Allow the next player to make a turn
		i += 1




	#----------------------------
	#  Scoring
	#----------------------------

	# Determine the number of remaining cards
	# Determine the winner of the trick
	cards = []
	lowest = 14
	for i, player in enumerate(players):
		cards.append(len(player.hand.remain))

		if cards[i] < lowest:
			lowest = cards[i]

	# Calculate scores
	scores = [0, 0, 0, 0]
	for i in range(0, 4):
		# Determine the winner to be the sum of the rest later
		if cards[i] == lowest:
			scores[i] = 0
			continue

		if cards[i] < 10:
			scores[i] = -1 * cards[i]
		elif cards[i] <= 12:
			scores[i] = -2 * cards[i]
		else:
			scores[i] = -3 * cards[i]

	
	# Calculate winner's score
	for i in range(0, 4):
		if cards[i] == lowest:
			scores[i] = 0

			# Sum other player's scores as a postive number
			for j in range(0, 4):
				scores[i] -= scores[j]
			break


	return scores


def game(players):
	scores = [0, 0, 0, 0]
	finish = False

	# Play 10 rounds
	for i in range(0, 10):
		out = playRound(players)

		# Sum all scores
		for j, val in enumerate(out):
			if j >= len(scores):
				print('out', out)
			scores[j] += val

		# If a player has passed 100 (there may be multiple)
		#  Stop
		for val in scores:
			if val >= 100:
				finish = True

		if finish:
			break

	return scores




def learn():
	players = [
		Bot(),
		Bot(),
		Bot(),
		Bot()
	]
	players[0].simplex = True
	
	bestScores = [0, 0, 0, 0]
	bestScore = -100
	bestNetwork = players[0].network
	prevNetwork = players[0].network

	file = open("log.csv", "w+")
	file.write('Generation,')
	file.write('Score 1, Score 2,Score 3, Score 4, Neuron Count, Columns, Largest Column\n')

	# Estimation info
	last = time.time()

	generation  = 0
	cycles      = 1000000
	logInterval = (cycles/200) # 200 = number of data points at the end of training
	score       = [-100,-100,-100,-100]

	def updateLog(gameCount):
		if generation < 2:
			print(' Est: Unknown')
		else:
			duration = (now - last) / logInterval * (cycles-generation)
			print(' Est:', str(duration/60)[0:5]+'mins')

		print(' Status;')
		print('  - Iteration: ', gameCount)
		print('  - Generation:', generation)
		print(' Best;')
		print('  - Weights:   [', ', '.join([str(val)[0:5] for val in bestNetwork.linear[0:10]]) + ', ... ]', len(bestNetwork.linear) )
		print('  - Structure: [', ', '.join([str(val)[0:5] for val in bestNetwork.columns]), ']', len(bestNetwork.columns) )
		print('  - Scores:   ', ', '.join([str(val) for val in bestScores]))
		print(' Previous;')
		print('  - Weights:   [', ', '.join([str(val)[0:5] for val in prevNetwork.linear[0:10]]) + ', ... ]', len(prevNetwork.linear) )
		print('  - Structure: [', ', '.join([str(val)[0:5] for val in prevNetwork.columns]), ']', len(prevNetwork.columns)  )
		print(' Progress;')

		file.write(str(generation)+',')
		file.write(','.join([str(val) for val in bestScores])+',')
		file.write(str(len(bestNetwork.linear))+',')
		file.write(str(len(bestNetwork.columns))+',')
		file.write(str(max(bestNetwork.columns))+'\n')
		file.flush()



	for rerun in range(0, cycles):


		# Create mutations of best BOT
		for i in range(1, 4):
			players[i].network = bestNetwork.reproduce(prevNetwork)



		# Make the games best out of 3 to remove chances of fluke
		score = [0, 0, 0, 0]
		for j in range(0, 3):
			out = game(players)
			for k, val in enumerate(out):
				score[k] += val



		# Determine the best (non-simplex) bot
		greatest = bestScore
		for i in range(1, 4):
			if score[i] > greatest:
				greatest = score[i]
		# Store the weights of the best bot for the next round
		for i in range(1, 4):
			if score[i] == greatest:
				# Do not replace the current best with a worse solution
				if score[i] <= bestScore:
					break

				prevNetwork = bestNetwork
				bestNetwork = players[i].network
				bestScore   = score[i]
				bestScores  = score
				generation += 1
				print('  - New Best:', bestScore)
				break


		
		# Remove some data points to help acess
		if rerun % logInterval == 0:

			# Print and write out results
			print('\nStatus:', str(rerun/cycles*100)[0:3] + '%')
			now = time.time()
			updateLog(rerun)
			last = time.time()

	print('Finished\a')
	now = time.time()
	updateLog(cycles)
	last = now
	file.close()

	print('\nEnd Product');
	print(' Best;')
	print('  - Weights:   [', ', '.join([str(val)[0:10] for val in bestNetwork.weights]), ']')
	print('  - Structure: [', ', '.join([str(val)[0:10] for val in bestNetwork.columns]), ']')

	file = open("result.dat", "w+")
	file.write('Result;')
	file.write(' - Weights:   [' + ', '.join([str(val)[0:10] for val in bestNetwork.weights]) + ']')
	file.write(' - Structure: [' + ', '.join([str(val)[0:10] for val in bestNetwork.columns]) + ']')
	file.close()


if __name__ == '__main__':
	learn()