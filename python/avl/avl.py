# !usr/bin/env python3.6
# Author Marius Ingebrigtsen

from sys import path

path.insert(1, "../bst/")

from bst import Node, BinarySearchTree


def subtree(node):
	""" Return nodes' level. """

	if node is None:
		return 0
	return node.level


class Node(Node):
	""" Node for insertion into AVL-Tree. """

	def __init__(self, key, item, nextnode):
		""" Creation of new node takes key, item and next node in next-sequence. """
		
		self.key	= key
		self.item	= item
		self.left	= None
		self.right	= None
		self.level	= 1
		self.next	= nextnode

	def update_level(self):
		""" Update level. """
		
		if subtree(self.left) > subtree(self.right):
			self.level	= subtree(self.left) + 1
		else:
			self.level	= subtree(self.right) + 1 

	def _rotate_left(self):
		""" Rotate left on node. """

		child = self.right
		self.right = child.left
		child.left = self

		self.update_level()
		child.update_level()

		return child

	def _rotate_right(self):
		""" Rotate right on node. """
		
		child = self.left
		self.left = child.right
		child.right = self

		self.update_level()
		child.update_level()

		return child


class AVLTree(BinarySearchTree):
	""" AVL-Tree structure. """

	def __init__(self, cmpfunc):
		""" Initialization takes compare function for type of items to insert into tree. 
			Function must return -1 if first argument is less than second, 0 if equal, 
			and 1 if first argument is greater than the second. """

		self.cmpfunc	= cmpfunc
		self.root		= None
		self.head		= None
		self.inserted	= True
		self.children	= 0

	def _insert(self, current, key, item):
		""" Recursive insert. """
		
		if current is None:
			self.head = Node(key, item, self.head)
			return self.head

		res = self.cmpfunc(key, current.key)

		if res < 0:
			current.left = self._insert(current.left, key, item)

			current.update_level()

			balance = subtree(current.left) - subtree(current.right)

			if balance > 1:
				res = self.cmpfunc(key, current.left.key)

				# LEFT-LEFT CASE: 
				if res < 0:
					current = current._rotate_right()
				# LEFT-RIGHT CASE: 
				elif res > 0:
					pass

		elif res > 0:
			current.right = self._insert(current.right, key, item)

			current.update_level()

		else:
			current.item = item
			self.inserted = False

		current.update_level()

		return current
	
	def insert(self, key, item):
		""" Adds item in tree by comparing key. 
			If key already associated with value in structure, then item will be overwritten. 
			Return True if inserted, return False if overwritten. """
		
		self.inserted = True

		self.root = self._insert(self.root, key, item)

		if self.inserted:
			self.children += 1

		return self.inserted


if __name__ == '__main__':
	pass