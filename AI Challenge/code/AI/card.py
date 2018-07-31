# Short hand use
#  For API
cardsSuit = 'DCHS'
cardsRank = '34567890JQKA2'
cardCount =  len(cardsSuit)*len(cardsRank)

class Card:
	def __init__(self, string):
		self.suitID   = 0
		self.rankID   = 0
		self.worth = 0

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

		self.worth = (self.rankID)*len(cardsSuit) + (self.suitID)
		pass

	def __str__(self):
		return cardsRank[self.rankID].upper() + cardsSuit[self.suitID].upper()

	def toDebugStr(self):
		return str(self.worth)+":"+str(self)

	def match(self, other):
		return self.suitID == other.suitID and self.rankID == other.suitID


def CardSortMethod(val):
	return [val.rankID, val.suitID]






cardDeck = []
# Generate all possible combinations
for suit in cardsSuit:
	for rank in cardsRank:
		cardDeck.append( Card(rank+suit) )

class CardSet:
	def __init__(self, autoFill=False):
		self.remain = []

		if autoFill:
			for card in cardDeck:
				self.remain.append(card)

		return

	def remove(self, cards:[Card]):
		for card in cards:
			if card in self.remain:
				i = self.remain.index(card)
				self.remain = self.remain[0:i] + self.remain[i+1:]

		return

	def sort(self):
		self.remain = sorted(self.remain, key=CardSortMethod)

	def strength(self, card):
		self.sort()

		i = 0
		while (self.remain[i].suitID < card.suitID or self.remain[i].rankID < card.rankID):
			i += 1

			# Prevent index overflow
			# This is the best, and winning card
			if i >= len(self.remain):
				break

		return i / len(self.remain)

	def highest(self):
		highest = None

		for val in self.remain:
			# Found a better option
			if highest == None or val.worth > highest.value:

				highest = val

		return highest

	def lowest(self, minimum=None):
		lowest = None

		for val in self.remain:
			# Found a better option
			if lowest == None or val.worth < lowest.worth:
				if minimum != None and not(val.worth > minimum.worth):
					continue

				lowest = val

		return lowest