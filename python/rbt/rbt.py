# !usr/bin/env python3.8
from sys import path
path.insert(1, "../bst/")
from bst import BinarySearchTree, Node


class Node(Node):
	""" Node for rbt-structure. """

	def __init__(self, key = None, item = None, nextnode = None):
		""" Create and assign value to node. """

		self.key	= key
		self.item	= item
		self.color	= "red"
		self.left	= None
		self.right	= None
		self.child	= None
		self.next	= nextnode

	def split(self):
		""" Attempt to discover 2-3-4-Tree equivalent of 4-node, and then 'split' them. """

		try:
			if self.left.color == "red" and self.right.color == "red":
				self.color = "red"
				self.left.color = self.right.color = "black"
		except AttributeError as NoneNode:
			return

	def right_left_case(self, fromright = 0/1):
		""" Check for Red Right-Left Case on current. """

		child = self

		try:
			if fromright and current.color == "red" and current.left.color == "red":
				child = self._rotate_right()

		except AttributeError as NoneNode:
			return child
		finally:
			return child

	def left_left_case(self):
		""" Check for Red Left-Left Case on current. """

		child = self

		try:
			if self.left.color == "red" and self.left.left.color == "red":
				child = self._rotate_right()

				child.color = "black"
				self.color = "red"

		except AttributeError as NoneNode:
			return child
		finally:
			return child

	def left_right_case(self, fromright = 0/1):
		""" Check for Red Left-Right Case on current. """

		child = self

		try:
			if not fromright and current.color == "red" and current.right.color == "red":
				child = self._rotate_left()

		except AttributeError as NoneNode:
			return child
		finally:
			return child

	def right_right_case(self):
		""" Check for Red Right-Right Case on current. """

		child = self

		try:
			if self.right.color == "red" and self.right.right.color == "red":
				child = self._rotate_left()

				child.color = "black"
				self.color = "red"

		except AttributeError as NoneNode:
			return child
		finally:
			return child


class RedBlackTree(BinarySearchTree):
	""" Self-balancing binary search tree. """

	# Red-Black Tree Insert: #
	def _insert(self, current = Node, key = None, item = None, fromright = 0/1):
		""" Recursive bottom-up insert. """

		if current is None:
			self.head = Node(key, item, self.head)	# Link new node to next-structure with new node as the new head. 
			return self.head

		current.split()

		res = self.cmpfunc(key, current.key)

		if res < 0:
			
			current.left = self._insert(current.left, key, item, 0)

			current = current.right_left_case(fromright)

			current = current.left_left_case()

		elif res > 0:
			
			current.right = self._insert(current.right, key, item, 1)

			current = current.left_right_case(fromright)

			current = current.right_right_case()

		else:
			current.item = item
			self.inserted = False

		return current

	def insert(self, key = None, item = None):
		""" Add item in RBT according to value of key. 
			Return True if inserted, False otherwise. """

		self.inserted = True

		self.root = self._insert(self.root, key, item, 0)
		
		self.root.color = "black"
		
		if self.inserted:
			self.children += 1

		return self.inserted

	# Red-Black Tree; Destroy: #
	def _percolate(self, current = Node, leftrotate = True, fromright = 0/1):
		""" Alternate left- and right-rotation until node have one leaf child. """

		current.split()

		if current.left is None:
			return current.right
		elif current.right is None:
			return current.left
		elif leftrotate:

			current = current._rotate_left()

			current.left = self._percolate(current.left, False)

			current.right_left_case(fromright)

			current.left_left_case()

		else:

			current = current._rotate_right()

			current.right = self._percolate(current.right, True)

			current.left_right_case(fromright)

			current.right_right_case()

		return current

	def _remove(self, current = Node, key = None):
		""" Recursive search for matching key. """

		if current is None:
			return None

		res = self.cmpfunc(key, current.key)

		if res < 0:
			current.left = self._remove(current.left, current, key)
		elif res > 0:
			current.right = self._remove(current.right, current, key)
		else:
			self.item = current.item
			current = self._percolate(current, True, 0)
			self.children -= 1

		return current

	def remove(self, key = None):
		""" Removes item assosiated with key. """

		self.root		= self._remove(self.root)
		self.root.color	= "black"

	def pop(self, key = None):
		""" Removes and return item assosiated with key. 
			Return None if key not in bst. """

		self.item		= None
		self.root		= self._remove(self.root, key)
		self.root.color	= "black"
		return self.item


if __name__ == "__main__":
	pass