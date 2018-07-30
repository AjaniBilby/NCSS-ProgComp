from AI.card import Card, CardGroupID

def is_higher(card1, card2):
	a = Card(card1)
	b = Card(card2)

	return a.unqiueID > b.unqiueID


if __name__ == '__main__':
	print(is_higher('8D', '9S'))
	print(is_higher('2S', '2D'))
	print(is_higher('3H', '2H'))
	print(is_higher('QS', 'JS'))
	print(is_higher('AD', '2S'))