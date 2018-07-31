class PlayTree:

	def __init__(self, card, hand, parent = None, depth = 0):
		self.parent   = parent
		self.card     = card
		self.depth    = depth
		self.children = []

		# There are no card patters that exceed this length
		if self.depth >= 5:
			return

		# Can no longer forward propergate
		if len(hand) < 1:
			return

		nxt = []
		c = None
		for i, card in enumerate(hand):
			nxt = hand[0:i] + hand[i+1:]
			c = PlayTree(hand[i], nxt, self, depth+1)
			self.children.append( c )

		return

	def __str__(self):
		out = str(self.card)+':[ '
		out += ', '.join([str(child) for child in self.children])
		out += ' ]'
		return out


test = PlayTree('', ['3D', '4D'])

print(test)


