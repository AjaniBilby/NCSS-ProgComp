class Sandwich:
	ingredients = []
	score = 0

	def __init__(self, ingredients):
		self.ingredients = ingredients

		for ingredient in self.ingredients:
			for pref in preferences:
				if pref[0] == ingredient:
					self.score += int(pref[1])
					break

	def __str__(self):
		return ','.join(self.ingredients)


def readCSV(file):
	out = []
	with open(file, 'r') as csvfile:
		i = 0
		while True:
			out.append( csvfile.readline() )
			if (out[i] == ""):
				# Ignore the last empty line
				out = out[:-1]
				break

			# Remove special characters
			# +Split into columns
			if (out[i][0] == '\r'):
				out[i] = out[i][1:]
			else:
				out[i] = out[i][0:]

			if out[i][-1] == '\n':
				out[i] = out[i][0:-1]

			out[i] = out[i].split(',')
			
			i += 1

	return out

preferences = readCSV('./preferences.txt')
menu = readCSV('./sandwiches.txt')

# Check if valid options
if len(menu) == 0:
	print('devo :(')
else:
	opt = [Sandwich(val) for val in menu]

	def SortSandwiches(val):
		out = [-val.score, -len(val.ingredients)] + val.ingredients
		return out
	opt = sorted(opt, key=SortSandwiches, reverse=False)

	print ('\n'.join([str(val) for val in opt]))
