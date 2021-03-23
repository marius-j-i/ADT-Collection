# !usr/bin/env python3.8
from sys import argv
from bst import BinarySearchTree


def strcmp(a, b):
	var = -1 if a < b else 1
	return var if a != b else 0


class Test:
	def __init__(self, value = 0):

		self.bst = BinarySearchTree(strcmp)
		self.root = round(value / 2)
		self.bst.add(self.root)

		print(f"\nRoot is; {self.root}. \n")

	def perform(self, inserts = 0):
		self.addition(inserts)
		self.remove(inserts)
		self.search(inserts)

	def addition(self, inserts = 0):
		for i in range(inserts):
			if i != self.root:
				if self.bst.add(i) is not True:
					raise ValueError

		print(f"\nAll {inserts}-inserts completed. \n")

	def remove(self, inserts = 0):
		print(f"\nTree-print before removal; { self.bst.__str__() }. \n")

		print(f"\nPopping item; \'{ self.bst.pop( self.root ) }\'. \n")

		print(f"\nTree-print after removal; { self.bst.__str__() }. \n")

	def search(self, inserts = 0):
		for i in range(inserts):
			if i != self.root:
				if self.bst.contains(i) is not True:
					exit(f"\n\'{i}\' not found in bst. \n")

		print("\nBst validated after removal. \n")


if __name__ == '__main__':
	
	if len(argv) < 2:
		print(f"Usage: {argv[0]} <value> \n")
		exit()

	test = Test(int(argv[1]))

	test.perform( int(argv[1]) )

	exit()