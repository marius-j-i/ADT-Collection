from set import Set
from sys import path, argv, exit
path.insert(1, "..")
from test import cmpint


class Test:

	def __init__(self):
		self.set = Set(cmpint)

	def perform(self, inserts = 0):
		self.inserts(inserts)
		self.contains(inserts)
		self.indexing(inserts)
		self.setoperations(inserts)
		self.printing(inserts)

	def inserts(self, inserts = 0):
		for i in range(inserts):
			if self.set.add(i) is not True:
				print("\nItem; \'{}\', not inserted. \n".format(i))
				exit()
		print("\nAll {}-additions completed. \n".format(inserts))

	def contains(self, inserts = 0):
		for i in range(inserts):
			if self.set.contains(i) is None:
				print("\nItem; \'{}\', not found in set. \n".format(i))
				exit()
		print("\nAll {}-searches completed. \n".format(inserts))

	def indexing(self, inserts = 0):
		try:
			for i in range(inserts):
				if self.set[i] != i:
					print("\noops\n")
					raise IndexError
		except IndexError as indxerr:
			exit("\nItem \'{}\', not indexed correctly. Error: {} \n".format(i, indxerr))

		print("\nAll {}-items indexed correctly. \n".format(inserts))

	def setoperations(self, inserts = 0):
		self.b = Set(cmpint)
		for i in range(0, inserts, 2):
			if self.b.add(i) is not True:
				exit("\nItem; \'{}\', not added. \n".format(i))

		try:

			print("\nSet Union: \n")
			try:
				c = self.set + self.b
				if not isinstance(c, Set):
					raise TypeError

				try:
					for i in range(inserts):
						if i not in c:
							raise ValueError

				except ValueError as valerr:
					exit("\nItem not in set operation. Error: {} \n".format(valerr))

			except TypeError as typerr:
				raise TypeError


			print("\nSet Intersection: \n")
			try:
				c = self.set * self.b
				if not isinstance(c, Set):
					raise TypeError

				try:
					for i in range(0, inserts, 2):
						if i not in c:
							raise ValueError

				except ValueError as valerr:
					exit("\nItem not in set operation. Error: {} \n".format(valerr))

			except TypeError as typerr:
				raise TypeError


			print("\nSet Difference: \n")
			try:
				c = self.set - self.b
				if not isinstance(c, Set):
					raise TypeError

				try:
					for i in range(0, inserts, 2):
						if i in c:
							raise ValueError

				except ValueError as valerr:
					exit("\nItem not in set operation. Error: {} \n".format(valerr))

			except TypeError as typerr:
				raise TypeError

		except TypeError as typerr:
			exit("\nReturn operation not a Set. Error: {} \n".format(typerr))

		print("\nAll set operations completed. \n")

	def printing(self, inserts = 0):
		print(self.set)
		self.b.add(self.set)
		print(self.b)


if __name__ == "__main__":
	
	if len(argv) < 2:
		print("Usage: {} <value> \n".format(argv[0]))
		exit()

	test = Test()

	test.perform( int(argv[1]) )

	exit()