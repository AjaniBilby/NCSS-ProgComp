from AI.card import Card, CardGroupID

def sort_cards(cards):
	def sortFunc(val):
		return [Card(val).unqiueID]
	
	return sorted(cards, key=sortFunc)


if __name__ == '__main__':
	print(sort_cards(['AS', '5H']))
	print(sort_cards(['JC', '3H', '6H', 'KH', '6S', '4H', '2C', '2S', '2D', '2H', '4S']))