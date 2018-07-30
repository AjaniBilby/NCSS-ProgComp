# Short hand use
#  For API
cardsSuit      = 'DCHS'
cardsRank    = '34567890JQKA2'
cardCount = len(cardsSuit)*len(cardsRank)

class Card:
	suitID   = 0
	rankID = 0
	unqiueID = 0

	def __init__(self, string):

		if len(string) < 2 or type(string) != str:
			raise ValueError("Invalid card \""+ string +"\"")

		# Read suitID
		suit = string[1].upper()
		if suit not in cardsSuit:
			raise ValueError("Invalid card \""+ string +"\"")
		self.suitID = cardsSuit.index(suit)

		# Read numberID
		rank = string[0].upper()
		if rank not in cardsRank:
			raise ValueError("Invalid card \""+ string +"\"")
		self.rankID = cardsRank.index(rank)


		self.unqiueID = (self.rankID+1)*len(cardsSuit) + (self.suitID+1)
		pass

	def __str__(self):
		return cardsRank[self.rankID].upper() + cardsSuit[self.suitID].upper()

	def toDebugStr(self):
		return str(self.unqiueID)+":"+str(self)


def CardGroupID(cardList):
	exponent = len(cardList)-1
	tally = 0

	for i, card in enumerate(cardList):
		tally += cardCount**(exponent-i) * card.unqiueID

	return tally



	

def sort_cards(cards):
	def sortFunc(val):
		return [Card(val).unqiueID]
	
	return sorted(cards, key=sortFunc)

def play(hand, is_start_of_round, play_to_beat, round_history, player_no, hand_sizes, scores, round_no):
	if is_start_of_round:
		if '3D' in hand:
			return ["3D"]

	cards = []
	for value in hand:
		cards.append( Card(value) )

	def sortFunc(val):
		return [val.unqiueID]
	cards = sorted(cards, key=sortFunc)

	if len(play_to_beat) < 1:
		beat = 0
	else:
		beat = CardGroupID([ Card(play_to_beat[0]) ])

	for card in cards:
		if card.unqiueID > beat:
			return [str(card)]
	

	return []






if __name__ == '__main__':
	# Write your own test cases for your `play` function here.
	# These can be run with the Run button and will not affect the tournament or marking.
	
	# Here's an example test case and testing code to kick you off.
	TESTS = [  # [ expected return value, inputs ]
		[['3D'], [['3D', '4D', '4H', '7D', '8D', '8H', '0D', '0C', 'JH', 'QC', 'QS', 'KH', 'AS'], True, [], [[]], 0, [13, 13, 13, 13], [0, 0, 0, 0], 0]],
		# Add more tests here.
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