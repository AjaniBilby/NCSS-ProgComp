IS_VICINAL, IS_NON_VICINAL, IS_NEITHER = 1, 2, 3
letters = "abcdefghijklmnopqrstuvwxyz"

def letterDist(a, b):
	i = letters.index(a)
	j = letters.index(b)

	# Ensure j > i via swapping
	if j < i:
		j, i = i, j

	return min(
		( len(letters)-j ) + i,  # Length via looping around the array
		j-i                      # Length from point to point
	)

def categorise_word(word):
	word = word.lower()

	found = False  # Found a link last iteration
	had = False    # Had a link at some point
	for i, a in enumerate(word):
		found = False

		for j, b in enumerate(word):
			if i == j:
				continue

			dist = letterDist(a, b)
			if dist == 1:
				found = True
				had = True
				break
		
		if not found:
			break

	if not found:
		if had:
			return IS_NEITHER
		return IS_NON_VICINAL
	return IS_VICINAL

if __name__ == '__main__':
	print(categorise_word('The'))
	print(categorise_word('blacksmith'))
	print(categorise_word('fights'))
	print(categorise_word('some'))
	print(categorise_word('baker'))
	print(categorise_word('baa'))