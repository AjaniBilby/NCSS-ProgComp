from card import Card
from bot import Bot


bot = Bot()

def play(hand, is_start_of_round, play_to_beat, round_history, player_no, hand_sizes, scores, round_no):
	# Read in hand
	bot.ResetHand([ Card(val) for val in hand ])

	for rnd in round_history:
		for item in rnd:
			# Ignore player's passing
			if len(item) < 2 or len(item[1]) == 0:
				continue

			bot.Observe(
				[Card(val) for val in item[1]], # Cards that were played
				item[0]                         # Player number
			)

	if is_start_of_round:
		if len(play_to_beat) != 0 and play_to_beat[0] != "":
			raise ValueError("There cannot be a play to beat when a round is starting")

	# Allow the bot to make a play
	if len(play_to_beat) < 0 or play_to_beat[0] == "":
		if not is_start_of_round:
			raise ValueError("No play to beat while not at the start of the game\nSome one's system broke")

		cards = bot.Play(
			True,
			[],
			hand_sizes
		)
	else:
		cards = bot.Play(
			is_start_of_round,
			[Card(val) for val in play_to_beat],
			hand_sizes
		)
	return [str(card) for card in cards]




















if __name__ == '__main__':
	# Write your own test cases for your `play` function here.
	# These can be run with the Run button and will not affect the tournament or marking.
	
	# Here's an example test case and testing code to kick you off.
	TESTS = [  # [ expected return value, inputs ]
		[
			['3D'],
			[
				['3D', '4D', '4H', '7D', '8D', '8H', '0D', '0C', 'JH', 'QC', 'QS', 'KH', 'AS'],
				True,
				[''],
				[
					[(0, ['5H'])]
				],
				0,
				[13, 13, 13, 13],
				[0, 0, 0, 0],
				0
			]
		],
		# Previous player passed
		[
			['4H'],
			[
				['3D', '4D', '4H', '7D', '8D', '8H', '0D', '0C', 'JH', 'QC', 'QS', 'KH', 'AS'],
				False,
				['4D'],
				[
					[(0, ['4C'])]
				],
				0,
				[13, 13, 13, 13],
				[0, 0, 0, 0],
				0
			]
		],
		# Previous failure
		[
			['5D'],
			[
				['4C', '5D', '5C', '7C', '7H', '7S', '8C', '9D', '9C', '0D', '0C', '0H', '2D'],
				False,
				['4H'],
				[
					[
						[ 1, ['3D'] ],
						[ 2, ['4H'] ],
						[ 3, []     ]
					]
				],
				0,
				[13, 12, 12, 13],
				[0, 0, 0, 0],
				0
			]
		]
	]
	
	# This runs the above test cases.
	for i, test in enumerate(TESTS):
		expected_return_value, inputs = test
		actual_return_value = play(*inputs)
		if actual_return_value == expected_return_value:
			print('PASSED {}/{}.'.format(i + 1, len(TESTS)))
		else:
			print('FAILED {}/{}.'.format(i + 1, len(TESTS)))
			print('    inputs:', repr(inputs))
			print('  expected:', repr(expected_return_value))
			print('    actual:', repr(actual_return_value))