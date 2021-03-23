from sys import argv
from os import listdir
from index import Index


class Test:
	"""  """

	def __init__(self):
		"""  """

		self.index = Index()

	def perform(self, files = ["path", "path", ...]):
		""" Executes tests on index. """

		try:
			self.addpath(files)
		except OSError as err:
			print(err)

		self.print_index(files)
		self.query()

	def addpath(self, files = ["path", "path", ...]):

		for path in files:
			with open(path) as file:
				words = []
				for line in file:
					for word in line.split(" ", -1):
						words.append(word)

				self.index.addpath(path, words)

	def print_index(self, files = ["", ...]):
		""" Open and append words in first file. Test print with words list. """

		words = []

		with open(files[0]) as file:
			for line in file:
				for word in line.split(" ", -1):
					words.append(word)

		self.index.print(words)

	def query(self):
		""" Performs query test on index. """

		pass


if __name__ == "__main__":
	
	if len(argv) < 2:
		print("Usage: {} <directory> \n".format(argv[0]))
		exit()

	test = Test()

	files = listdir(argv[1])

	for i in range( len(files) ):
		files[i] = argv[1] + files[i]

	test.perform(files)

	exit()