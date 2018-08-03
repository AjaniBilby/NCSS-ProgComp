from network import ANN
from card import Card, CardSet

class Bot:
	def __init__(self):
		self.hand = CardSet(autoFill=False)
		self.unseen = CardSet(autoFill=True)
		self.network = ANN(
			[4,3,2,1],
			[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
		)
		self.simplex = False

	"""
	Ensure that the BOT only has cards it should have
	"""
	def ResetHand(self, hand):
		self.hand = CardSet()
		for card in hand:
			self.hand.remain.append(card)

	"""
	Allows the bot to count the cards in play
	"""
	def Observe(self, play, player_no):
		self.unseen.remove(play)

	def __Select(self, beat, hand_sizes):

		# No passing behaviour active when simplex is on
		#   Player the lowest valid card
		if self.simplex == True:
			pick = self.hand.lowest(beat[0])
			if pick == None:
				return []

			return [pick]




		# Out of all of the cards able to be played
		# Select the one with the greatest confidence
		opt = self.hand.subsetGreater(beat[0])
		pick = None
		pickConfidence = 0
		nxtConfidence  = 0

		shs = min(hand_sizes)
		for card in opt:
			nxtConfidence = self.network.forward([
				(beat[0].worth/card.worth),
				(shs/13),
				self.unseen.strength(card),
				len(self.hand.remain)
			])[0]

			# If this card has a better confindence for playing it
			# Select it
			if nxtConfidence > pickConfidence:
				nxtConfidence = pickConfidence
				pick = card

		
		if pick == None:
			return []

		return [pick]

	def Play(self, is_start_of_round, play_to_beat, hand_sizes):
		if is_start_of_round or len(play_to_beat) == 0:
			result = [Card('3D')]
			if self.hand.contains(result[0]):
				pass
			else:
				result = [self.hand.lowest()]
		else:
			result = self.__Select(play_to_beat, hand_sizes)

		if result == None:
			return []

		# Remove these cards from the bots hand
		self.hand.remove(result)

		return result
