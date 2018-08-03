import random

class ANN:
	def __init__(self, columns:list, weights:list, autoFill=False):
		self.columns = columns
		
		# Interpret 1D weights into 3D
		self.linear = weights
		self.weights = []
		i = 0
		for x in range(1, len(columns)):
			self.weights.append( [] )
			for y1 in range(0, columns[x]):
				self.weights[x-1].append( [] )

				for y2 in range(0, columns[x-1]):
					self.weights[x-1][y1].append( weights[i] )
					i += 1

	def forward(self, data):
		if len(data) < self.columns[0]:
			raise ValueError("Invalid input data, not enough elements")

		# Fill the neurons with starting data
		neurons = []
		for x, val in enumerate(self.columns):
			neurons.append([])
			for y in range(0, val):
				if x==0:
					neurons[x].append(data[y])
				else:
					neurons[x].append(0)

		for x in range(1, len(neurons)):
			for y1 in range(0, len(neurons[x])):
				for y2 in range(0, len(neurons[x-1])):
					neurons[x][y1] = neurons[x-1][y2] * self.weights[x-1][y1][y2]
		
		# Return the last column of neurons
		i = len(neurons)-1
		return neurons[i]


	def reproduce(self, other):
		# NOTE: Variations has a small bias towards increase in neuron count

		# Determine the number of columns
		columnCount = random.choice([
			len(self.columns),
			len(other.columns)
		])
		# Mutate the result
		columnCount = round( columnCount + (random.random()*2.25-1) )
		# Ensure input and output columns exist
		if columnCount < 2:
			columnCount = 2


		# Determine the number of neurons within a column
		columns = []
		for i in range(0, columnCount):
			opts = []
			if i < len(self.columns):
				opts.append(self.columns[i])
			if i < len(other.columns):
				opts.append(other.columns[i])
			if len(opts) == 0:
				opts.append(1)

			columns.append( random.choice(opts) )
			# Mutate
			columns[i] = round( columns[i] + (random.random()*2.25-1) )
			# Ensure at least one neuron exists
			if columns[i] < 2:
				columns[i] = 2

		# Insure that input and output sizes stay the same
		columns[0] = self.columns[0]
		columns[-1] = self.columns[-1]

		
		# Determine the weights
		i = 0
		linear  = []
		weights = []
		for x in range(1, columnCount):
			weights.append([])
			for y1 in range(0, columns[x]):
				weights[x-1].append([])
				for y2 in range(0, columns[x-1]):
					opts = []

					if x < len(self.weights):
						if y1 < len(self.weights[x-1]):
							if y2 < len(self.weights[x-1][y1]):
								opts.append(self.weights[x-1][y1][y2])
					if x < len(other.weights):
						if y1 < len(other.weights[x-1]):
							if y2 < len(other.weights[x-1][y1]):
								opts.append(other.weights[x-1][y1][y2])
					if len(opts) < 1:
						opts.append(0)

					weights[x-1][y1].append( random.choice(opts) )
					weights[x-1][y1][y2] += (random.random()*2 -1)
					linear.append(weights[x-1][y1][y2])

		out = ANN([], [])
		out.columns = columns
		out.weights = weights
		out.linear  = linear
		return out

	def clone(self):
		out = ANN([], [])
		out.weights = self.weights
		out.columns = self.columns

		return out



# print( ANNForward(
# 	[3, 3, 2, 1],
# 	[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],

# 	[3, 2, 1]
# ) )