#! usr/env/bin python3.6


class Index():
	""" Index class for mapping file-content to file-paths. """

	def __init__(self):
		""" Initialize. """
		self.path	= {}
		self.counter= {}
		self.corpus	= 0

	def map_inverse(self, path, word):
		if word not in self

	def addpath(self, path):
		"""  """
		if path in self.path:
			return

		with open(path) as f:
			words = [ word.strip('\n') for word in f.read().split(' ', -1) ]

		for word in words:
			self.map_inverse(path, word)

	def query(self, query):
		pass