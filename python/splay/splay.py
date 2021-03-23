# !usr/bin/env python3.6
### Author: Marius Ingebrigtsen ###

from sys import path
path.insert(1, "../bst")
from bst import BinarySearchTree, Node


class Node(Node):
	""" Node class with assignable left- and right-nodes. """

	def __init__(self, key = None, item = None, left = Node, right = Node, nextnode = Node):
		""" Create node and assign values. """

		self.key	= key
		self.item	= item
		self.left	= left
		self.right	= right
		self.next	= nextnode


class SplayTree(BinarySearchTree):
	""" Tree structure for quick recent-access retirieval. """

	def _insert(self, current, key, item):
		""" Bottom-up insertion and rotation for faster node access. """
		if current is None:
			self.head =  Node(key, item, None, None, self.head)
			return self.head

		res = self.cmpfunc(key, current.key)

		if res < 0:
			if current.left is None:
				self.head = Node(key, item, None, None, self.head)
				current.left = self.head
				current = current._rotate_right()
				return current

			res = self.cmpfunc(key, current.left.key)

			# Left-Left Case: 
			if res < 0:
				current.left.left = self._insert(current.left.left, key, item)

				if self.added is True:
					current = current._rotate_right()
					current = current._rotate_right()

			# Left-Right Case: 
			elif res > 0:
				current.left.right = self._insert(current.left.right)

				if self.added is True:
					current.left = current.left._rotate_left()
					current = current._rotate_right()

			# Key Exists: 
			else:
				self.added = False
				current.left.item = item

		elif res > 0:
			if current.right is None:
				self.head = Node(key, item, None, None, self.head)
				current.right = self.head
				current = current._rotate_left()
				return current

			res = self.cmpfunc(key, current.right.key)

			# Right-Left Case:
			if res < 0:
				current.right.left = self._insert(current.right.left, key, item)

				if self.added is True:
					current.right = current.right._rotate_right()
					current = current._rotate_left()

			# Right-Right Case: 
			elif res > 0:
				current.right.right = self._insert(current.right.right, key, item)

				if self.added is True:
					current = current._rotate_left()
					current = current._rotate_left()

			# Key Exists: 
			else:
				self.added = False
				current.right.item = item
		else:
			self.added = False
			current.item = item

		return current

	def insert(self, key = None, item = None):
		""" Adds item in tree by comparing key. 
			If key already associated with value in structure item will be overwritten. 
			Return True if inserted, return False if overwritten. 
			Make ready for easier access of inserted item. """
		self.added = True
		self.root = self._insert(self.root, key, item)

		if self.added is True:
			self.children += 1

		return self.added


def cmpint(a, b):
	return a - b


if __name__ == '__main__':
	s = SplayTree(cmpint)
	for i in range(32):
		if s.insert(i, i) is False:
			exit("Duplicate insert.")

	print("Last insert is {}".format(i))
	print("Root: {}".format(s.root.item))