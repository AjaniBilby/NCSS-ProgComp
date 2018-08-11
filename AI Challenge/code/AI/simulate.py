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
		if players[j].hand.contains(startCard) == True:
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
	for i, player in enumerate(players):
		cards.append(len(player.hand.remain))

	# Calculate scores
	scores = [0, 0, 0, 0]
	for i in range(0, 4):
		# Determine the winner to be the sum of the rest later
		if cards[i] == 0:
			continue

		if cards[i] < 10:
			scores[i] = -1 * cards[i]
		elif cards[i] <= 12:
			scores[i] = -2 * cards[i]
		else:
			scores[i] = -3 * cards[i]

	
	# Calculate winner's score
	for i in range(0, 4):
		if cards[i] == 0:
			# Sum other player's scores as a postive number
			for j in range(0, 4):
				if j == i:
					continue

				scores[i] -= scores[j]
			break


	return scores


def game(players):
	scores = [0, 0, 0, 0]
	finish = False

	# Play 10 rounds
	for _ in range(0, 10):
		if finish:
			break

		out = playRound(players)

		# Sum all scores
		for j, val in enumerate(out):
			scores[j] += val

		# If a player has passed 100 (there may be multiple)
		#  Stop
		for val in scores:
			if val >= 100:
				finish = True
				break

	return scores



"""
Three ANN bots verse one simplex
Breed from the best and previous best bot
"""
def learnByExample():
	players = [
		Bot(),
		Bot(),
		Bot(),
		Bot()
	]
	players[0].simplex = False
	# Keep the best bot in play for the next round
	players[0].best = False
	players[1].best = False
	players[2].best = False
	players[3].best = False
	bestScore   = 0
	bestNetwork = players[0].network
	prevNetwork = players[0].network

	file = open("log.csv", "w+")
	file.write('Elapsed,Generation,Score 1,Score 2,Score 3,Score 4,Win Rate,Weights,Columns,Largest Column,Neurons\n')

	# Estimation info
	start = time.time()

	smoothedWinRate = 0
	winRate = 0
	wins        = 0
	games       = 0
	generation  = 0
	cycles      = 5000
	logInterval = (cycles/100) # 500 = number of data points at the end of training
	score       = [-100,-100,-100,-100]
	tally       = [0,0,0,0]

	def updateLog(rerun, gameCount, winCount):
		if rerun < 2:
			print(' Est: Unknown')
		else:
			# duration 1 log inter. | log intervals left
			duration = now-start
			duration = duration/(rerun/cycles) - duration
			print(' Est:', str(duration/60)[0:5], 'mins')

		highest = tally[1]
		lowest = tally[1]
		total = 0
		for i in range(0, 4):
			if players[i].simplex == True:
				continue

			if highest < tally[i]:
				highest = tally[i]
			elif lowest > tally[i]:
				lowest = tally[i]
			total += tally[i]
		deviation = (highest - lowest) / total * 100

		print(' Status;')
		print('  - Iteration       :', rerun, 'of', cycles)
		print('  - Generation      :', generation)
		print(' Game Meta;')
		print('  - Avg. Win        :', str(smoothedWinRate*100)[0:5]+'%')
		print('  - Games Played    :', gameCount)
		print('  - Wins VS Simplex :', winCount)
		print('  - Win Rate        :', str(winRate*100)[0:4]+'%')
		print('  - Win Tally       :', ', '.join([str(val) for val in tally]))
		print('  - Deviation       :', str(deviation)[0:4]+'%')
		print(' Best;')
		print('  - Weights         : [', ', '.join([str(val)[0:5] for val in bestNetwork.linear[0:10]]) + ', ... ]', len(bestNetwork.linear) )
		print('  - Structure       : [', ', '.join([str(val)[0:5] for val in bestNetwork.columns]), ']', len(bestNetwork.columns) )
		print(' Previous;')
		print('  - Weights         : [', ', '.join([str(val)[0:5] for val in prevNetwork.linear[0:10]]) + ', ... ]', len(prevNetwork.linear) )
		print('  - Structure       : [', ', '.join([str(val)[0:5] for val in prevNetwork.columns]), ']', len(prevNetwork.columns)  )
		print(' Progress;')

		file.write(str(now-start)+',')                               # Elapsed
		file.write(str(generation)+',')                              # Generation
		file.write(','.join([str(val) for val in tally])+',')   # Scores[4]
		file.write(str(winRate)+',')                                 # Win Rate
		file.write(str(len(bestNetwork.linear))+',')                 # Number of weights
		file.write(str(len(bestNetwork.columns))+',')                # Number of columns
		file.write(str(max(bestNetwork.columns))+',')                # Largest column
		neurons = 0                                                  # Number of neurons
		for val in bestNetwork.columns:
			neurons += val
		file.write(str(neurons)+'\n')



	for rerun in range(0, cycles):


		# Create mutations of best BOT
		for i in range(0, 4):
			if players[i].simplex == False and players[i].best == False:
				players[i].network = bestNetwork.reproduce(prevNetwork)



		# Make the games best out of 5 to remove chances of fluke
		score = [0, 0, 0, 0]
		for _ in range(0, 5):
			out = game(players)
			games += 1

			winnerScore = out[0]
			winnerIndex = 0
			for k in range(1, 4):
				if out[k] > winnerScore:
					winnerScore = out[k]
					winnerIndex = k
			score[winnerIndex] += 1
			tally[winnerIndex] += 1



		# Determine the best (non-simplex) bot
		greatestScore = 0
		greatestIndex = -1
		for i in range(0, 4):
			if players[i].simplex == True:
				continue

			if score[i] > greatestScore:
				greatestScore = score[i]
				greatestIndex = i

		# Data for logging
		if greatestScore == bestScore:
			print('    - New Varient')
		elif greatestIndex > bestScore:
			print('  - New Best:', score[greatestIndex])
			bestScore = score[greatestIndex]

		# Is this a new best?
		# If so, make it a new breeder
		if players[greatestIndex].best != True:
			generation += 1

			# Mark this player as the best
			# And remove that status from all other players
			for j in range(0, 4):
				if j == i:
					continue
				players[j].best = False
			players[greatestIndex].best = True
			
			prevNetwork = bestNetwork
			bestNetwork = players[greatestIndex].network




		# Add up all wins not including the simplex bot
		for i in range(0, 4):
			if players[i].simplex == True:
				continue

			wins += score[i]




		
		# Remove some data points to help acess
		if rerun % logInterval == 0:

			# Print and write out results
			print('\nStatus:', str(rerun/cycles*100)[0:4] + '%')
			winRate = wins/games
			smoothedWinRate = (smoothedWinRate*3 + winRate) / 4
			now = time.time()
			updateLog(rerun, games, wins)
			wins = 0
			games = 0
			for i in range(0,4):
				tally[i] = 0

	print('Finished\a')
	file.close()

	print('\nEnd Product')
	print(' Best;')
	print('  - Weights:   [', ', '.join([str(val)[0:10] for val in bestNetwork.linear]), ']')
	print('  - Structure: [', ', '.join([str(val)[0:10] for val in bestNetwork.columns]), ']')

	file = open("result.dat", "w+")
	file.write('Result;\n')
	file.write(' - Weights:   [' + ', '.join([str(val) for val in bestNetwork.linear]) + ']\n')
	file.write(' - Structure: [' + ', '.join([str(val) for val in bestNetwork.columns]) + ']\n')
	file.close()





if __name__ == '__main__':
	learnByExample()