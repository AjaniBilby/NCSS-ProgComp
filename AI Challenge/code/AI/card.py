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
