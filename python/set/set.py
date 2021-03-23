# !usr/bin/env python3.8
from sys import path
path.insert(1, "../rbt")
from rbt import RedBlackTree, Node


class Node(Node):
	""" Change node-structure to not hold keys. """

	def __init__(self, item = None, nextnode = None):
		""" Create and assign value to node. """

		self.item	= item
		self.color	= "red"
		self.left	= None
		self.right	= None
		self.child	= None
		self.next	= nextnode


class Set(RedBlackTree):
	""" Class for mathematical sets that builds on RBT-implementation. """

	# Set Union: #
	def _union_wcopy(self, current = Node, union = set, copyfunc = None):
		""" Copy all items in self into union-set. 
			NOTE: When called this methods 'self' may refer to 'other'. """

		if current is None:
			return

		union.add(copyfunc(current.item))

		self._union_wcopy(current.left, union, copyfunc)
		self._union_wcopy(current.right, union, copyfunc)

	def _union(self, current = Node, union = set):
		""" Add all items in self into union-set. 
			NOTE: When called this methods 'self' may refer to 'other'. """

		if current is None:
			return

		union.add(current.item)

		self._union(current.left, union)
		self._union(current.right, union)

	def union(self, other = set, copyfunc = None):
		""" Set union with optional copyfunc. 
			If other-argument is not a Set a TypeError is raised. """

		if not isinstance(other, Set):
			raise TypeError
		elif copyfunc is not None:
			union = self.copy(copyfunc)
			other._union_wcopy(other.root, union, copyfunc)
		else:
			union = self.copy(None)
			other._union(other.root, union)
		return union

	def __or__(self, other = set):
		""" Set union with '^'-operator. """

		return self.union(other)

	def __add__(self, other = set):
		""" Set union with '+'-operator. """

		return self.__or__(other)

	# Set Intersection: #
	def _intersection_wcopy(self, current = Node, other = set, intersect = set, copyfunc = None):
		""" Copy items in common between self and other into intersect-set. """

		if current is None:
			return
		elif other.search(current.item) is not None:
			intersect.add(copyfunc(current.item))

		self._intersection_wcopy(current.left, other, intersect, copyfunc)
		self._intersection_wcopy(current.right, other, intersect, copyfunc)

	def _intersection(self, current = Node, other = set, intersect = set):
		""" Add only item in common between self and other into intersect-set. """

		if current is None:
			return
		elif other.search(current.item) is not None:
			intersect.add(current.item)

		self._intersection(current.left, other, intersect)
		self._intersection(current.right, other, intersect)

	def intersection(self, other = set, copyfunc = None):
		""" Set intersection with optional copyfunc. 
			If other-argument is not a Set a TypeError is raised. """

		if not isinstance(other, Set):
			raise TypeError

		intersect = Set(self.cmpfunc)

		if copyfunc is not None:
			self._intersection_wcopy(self.root, other, intersect, copyfunc)
		else:
			self._intersection(self.root, other, intersect)

		return intersect

	def __and__(self, other = set):
		""" Set intersection with '&'-operator. """

		return self.intersection(other, None)

	def __mul__(self, other = set):
		""" Set intersection with '*'-operator. """

		return self.__and__(other)

	# Set Difference: #
	def _difference_wcopy(self, current = Node, other = set, differ = set, copyfunc = None):
		""" Copy items in self but not other into difference-set. """

		if current is None:
			return
		elif other.search(current.item) is None:
			differ.add( copyfunc(current.item) )

		self._difference(current.left, other, differ, copyfunc)
		self._difference(current.right, other, differ, copyfunc)

	def _difference(self, current = Node, other = set, differ = set):
		""" Add items in self but not other into difference-set. """

		if current is None:
			return
		elif other.search(current.item) is None:
			differ.add(current.item)

		self._difference(current.left, other, differ)
		self._difference(current.right, other, differ)

	def difference(self, other = set, copyfunc = None):
		""" Set difference with optional copyfunc. 
			If other-argument is not a Set a TypeError is raised. """

		if isinstance(other, Set):
			raise TypeError
		
		differ = Set(self.cmpfunc)
		
		if copyfunc is not None:
			self._difference_wcopy(self.root, other, differ, copyfunc)
		else:
			self._difference(self.root, other, differ)

		return differ

	def __xor__(self, other):
		""" Set difference with '|'-operator. """

		return self.difference(other, None)

	def __sub__(self, other = set):
		""" Set union with '-'-operator. """

		return self.__xor__(other)

	# Set Copy: #
	def _bst_insert(self, current = Node, item = None):
		""" Insert like a regular Binary Search Tree. 
			NOTE: 
			When this method is called; 'self' may actually be 'new' from blue-copy functions. """

		if current is None:
			self.head = Node(item, self.head)
			return self.head

		res = self.cmpfunc(item, current.item)

		if res < 0:
			current.left	= self._bst_insert(current.left, item)
		elif res > 0:
			current.right	= self._bst_insert(current.right, item)

		return current

	def _rbt_bluecopy_wcopy(self, current = Node, new = set, copyfunc = None):
		""" Traverse self-tree and insert copy of every node-item into new set. """

		if current is None:
			return

		new.root = new._bst_insert(new.root, copyfunc(current.item) )

		self._bst_insert(current.left, new, copyfunc)
		self._bst_insert(current.right, new, copyfunc)

	def _rbt_bluecopy(self, current = Node, new = set):
		""" Traverse self-tree and insert every node-item into new set. """

		if current is None:
			return

		new.root = new._bst_insert(new.root, current.item)

		self._bst_insert(current.left, new)
		self._bst_insert(current.right, new)

	def copy(self, copyfunc = None):
		""" Return new instance of Set with all items in self. 
			If 'copyfunc' is not None, then it is used to return new instances of items in set. """

		new = Set(self.cmpfunc)

		if copyfunc is not None:
			self._rbt_bluecopy_wcopy(self.root, new, copyfunc)
		else:
			self._rbt_bluecopy(self.root, new)

		return new


if __name__ == "__main__":
	pass