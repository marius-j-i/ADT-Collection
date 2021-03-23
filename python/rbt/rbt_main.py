from rbt import RedBlackTree


def cmpfunc(a, b, **_):
	""" Comparison function of numbers. """

	return a - b

class TestRBT:
	""" Test class for insertion of RedBlackTree. """

	def __init__(self, rbt):
		""" Assign RBT. """

		self.rbt = rbt

	def insert(self, inserts = 0):
		""" Insert numbers into RBT. """

		for i in range( inserts ):
			if not self.rbt.insert(i, i):
				print("Item {} not inserted. ".format(i) )
		print("All items inserted. \n")

	def search(self, inserts):
		""" Search from numbers in RBT. """

		for i in range(inserts):
			
			item = self.rbt.search(i)

			if item is not None:
				print("Key; {}, yielded item; {}".format(i, item))
			
			else:
				print("Fix your program!")
		print("\n")

	def iterate(self):
		""" Test iteration of RBT. """

		for item in self.rbt:
			print("{}".format(item))

	def print_rbt(self):
		""" Test print dunder-method. """

		print("\nPrinting RBT: ")
		print(self.rbt)

	def indexing(self, index):
		""" Test indexing method for RBT. """

		print("Item at index {}; \'{}\'".format( index, self.rbt[index]) )
		print("Item at last index; \'{}\' \n".format( self.rbt[index]) )


if __name__ == "__main__":

	inserts = int(input("Type numbers of elements to insert: "))
	
	test = TestRBT( RedBlackTree(cmpfunc) )

	test.insert(inserts)

	test.search(inserts)

	test.iterate()

	test.print_rbt()

	while True:
		test.indexing( int( input("Index-Position: ") ) )
		if input("Continue? (1/0): ") == 0:
			break

	test.insert(inserts)

	test.search(inserts)

	test.iterate()

	test.print_rbt()
	
	exit()