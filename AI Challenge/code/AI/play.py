class PlayTree:
	card     = None
	children = []

	def __init__(self, card, hand):
		nxt = []

		for i, card in enumerate(hand):
			nxt = hand[0:i] + hand[i+1:];
			self.children += new PlayTree(card, nxt)

		pass


