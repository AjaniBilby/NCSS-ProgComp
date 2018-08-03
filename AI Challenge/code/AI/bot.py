from network import ANN
from card import Card, CardSet

class Bot:
	def __init__(self):
		self.hand = CardSet(autoFill=False)
		self.unseen = CardSet(autoFill=True)
		self.network = ANN(
			[4, 5, 2, 1],
			[11.472439753208366, 7.62371042738111, -0.025490302495908335, -5.231799338898514, -2.6109619064039267, 8.64580129906338, 1.1539865254894566, 4.390404738186968, -7.667009373651904, 6.01504489864956, -7.033433075994807, 3.549281842264964, 3.480245103281684, 0.20419263494333584, 2.1458982624681386, -3.934724994819617, -1.5356321265067228, -0.9878789701492283, 1.5775201008532658, -0.14011092398569946, 2.077488340430753, -0.40189121894212976, 3.332797685540013, 3.9932699487934036, -1.2638766823224898, -6.2059348058128565, -6.492314142185058, 1.8760962787712923, -0.5376992561099287, 0.7081016200425365, -0.16230722858165247, -0.546262551930419]
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
