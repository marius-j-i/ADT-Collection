# !usr/bin/env python3
from hashmap import HashMap


def cmpint(a, b):
	""" Comparison function for hashmap. """

	return a - b


class Test:
	""" Test class for HashMap. """

	def __init__(self, map = HashMap):
		""" Assign map. """

		self.map = map

	def test(self, inserts = 0):
		""" Perform the tests. """

		self.put(inserts)
		self.haskey(inserts)
		self.get(inserts)
		self.len()

	def put(self, inserts = 0):
		""" Attmept to put number of items in  """

		for i in range(inserts):
			if self.map.put(i, i) is not None:
				print("Value; \'{}\' inserted. ".format(i) )
			else:
				print("Value; \'{}\', did not insert. ".format(i) )
		print("\n\n")

	def haskey(self, inserts = 0):
		""" Test hashkey. """

		for i in range(inserts):
			if self.map.haskey(i):
				print("Key; \'{}\', is in map. ".format(i) )
			else:
				print("Key; \'{}\', is not in map. ")
		print("\n\n")

	def get(self, inserts = 0):
		""" Test get. """

		for i in range(inserts):
			if self.map.get(i) is not None:
				print("Key; \'{}\', retrieved. ".format(i) )
			else:
				print("Key; \'{}\', not retrieved. ".format(i) )
		print("\n\n")

	def len(self):
		""" Test length of map. """

		print(len(self.map))


if __name__ == "__main__":
	test = Test( HashMap(cmpint, hash) )
	test.test( input("Type number of item to map: ") )
	exit()