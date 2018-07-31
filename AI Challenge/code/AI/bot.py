from card import Card, CardSet

class Bot:
	def __init__(self):
		self.hand = CardSet(autoFill=False)
		self.unseen = CardSet(autoFill=True)
		self.weight = [3.8,-2.4,-2.6,3.2]
		self.neverPass = False

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

		# # IF only one card remains
		# #  Play the card in attempt to win the round
		# #  Thus starting the next round will cause you to win
		# if len(self.hand.remain) <= 1:
		# 	pick = self.hand.highest()

		# 	if beat[0].worth < pick.worth:
		# 		return [pick]
		# 	else:
		# 		return []

		# Player the lowest valid card
		pick = self.hand.lowest(beat[0])
		if pick == None:
			return []

		if self.neverPass:
			return [pick]

		# Save powerful cards
		#        Make the power's relative
		shs = min(hand_sizes)
		weight = ((beat[0].worth/pick.worth) / shs )       * self.weight[0]
		weight += (beat[0].worth/pick.worth)               * self.weight[1]
		weight += (shs/13)                                 * self.weight[2]
		weight += self.unseen.strength(pick)               * self.weight[3]
		if (weight > 1):
			return []

		return [pick]

	def Play(self, is_start_of_round, play_to_beat, hand_sizes):
		if is_start_of_round:
			result = Card('3D')
			if result in self.hand.remain:
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
