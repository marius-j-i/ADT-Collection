# !usr/bin/env python
### Author: Marius Ingebrigtsen ###

from sys import path
path.insert(1, "../hashmap")
from hashmap import HashMap
path.insert(1, "../set")
from set import Set


def strcmp(a, b):
	""" String compare. """

	if a < b:
		return -1
	elif a == b:
		return 0
	else:
		return 1


class Node:
	""" Node class for abstract syntax tree. """

	def __init__(self, word = "", nodetype = ""):
		""" Create new node. """

		self.left		= self.right = None
		self.word		= word
		self.nodetype	= nodetype


class Index:
	""" Class for indexing of files and retrieval of information. """

	def __init__(self, expected_corpus = 2**12):
		""" Index Structure. """

		if expected_corpus < 2**12:
			expected_corpus = 2**12

		self.invindx	= HashMap(strcmp, hash, expected_corpus)
		self.file_count	= HashMap(strcmp, hash, expected_corpus)
		self.corpus		= 0

	# Index Add-Path: #
	def _inverse_index(self, path = "", word = ""):
		""" Map file-path to set with all words in file. """

		path_set = self.invindx.get(path)

		if path_set is None:
			path_set = Set()
			self.invindx.put(word, path_set)
		path_set.add(word)

		self.corpus += 1

	def _file_count(self, path = "", word = "", file_size = 0):
		""" Create file-counter for path if not counted before. 
			Increment counter for word, or create count for word if not counted before. """

		file_count = self.file_count.get(path)

		if file_count is None:
			file_count = HashMap(strcmp, hash)
			self.file_count.put(path, file_count)
			frequency = (1 / file_size)
		
		else:
			frequency = file_count.get(word)
			if frequency is None:
				frequency = (1 / file_size)
			else:
				frequency += (1 / file_size)

		file_count.put(word, count)

	def addpath(self, path = "", words = ["", ...]):
		""" Index file. """

		for word in words:
			self._inverse_index(path, word)

			self._file_count(path, word, len(words))

	def print(self, words = ["", ...]):
		""" Print index with information correlating with words in argument list. """

		for word in words:
			path_set = self.invindx.get(word)

			if path_set is None:
				continue

			for path in path_set:
				print(word + " -> " + path + " ")

	# Index Query: #
	#	Query BNF Language: 

	# query	::=	andterm
	# 		|	andterm "ANDNOT" query

	# andterm	::=	orterm
	# 		|	orterm "AND" andterm

	# orterm	::=	term
	# 		|	term "OR" orterm

	# term	::=	"(" query ")"
	# 		|	<word>

	def _parse_query(self):
		""" Construct Abstract Syntax Tree. """

		andterm = self._parse_andterm()

	def _parse_andterm(self):
		"""  """

		orterm = self._parse_orterm()

	def _parse_orterm(self):
		"""  """

		term = self._parse_term()

	def _parse_term(self):
		"""  """

		word = next(self.queries)
		if word == "(":
			query = self._parse_query()
			if next(self.queries) != ")":
				raise Exception("Missing end-parenthesis. ")

		else:
			return Node(word, "TERM")

		return 

	def query(self, words = ["", ...]):
		""" Return list of tuples with path to result of query and score for path on query. """
		
		self.queries = iter(words)
		try:
			self._parse_query()
		except StopIteration:
			pass

	def __len__(self):
		""" Return size of index. """

		return self.corpus


if __name__ == "__main__":
	pass