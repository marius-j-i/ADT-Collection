# !usr/bin/env python3.8
### Author: Marius Ingebrigtsen ###


class Node:
	""" Node for bst-structure. """

	def __init__(self, key = None, item = None, nextnode = None):
		""" Assign values. """

		self.key	= key
		self.item	= item
		self.left	= None
		self.right	= None
		self.next	= nextnode

	def _rotate_left(self):
		""" Rotate left on node. """

		child = self.right
		self.right = child.left
		child.left = self

		return child

	def _rotate_right(self):
		""" Rotate right on node. """
		
		child = self.left
		self.left = child.right
		child.right = self

		return child


class BSTIterator:
	""" Iterator class for Binary Search Tree. """

	def __init__(self, node = Node):
		""" Assign start-node. """

		self.head = self.node = node

	def hasnext(self):
		""" Return False if iterator is exhausted, True otherwise. """

		if self.node is None:
			return False
		return True

	def next(self):
		""" Return current item and iterate node. """

		if self.node is None:
			raise StopIteration

		item = self.node.item
		self.node = self.node.next

		return item

	def __next__(self):
		""" So that class works for python 2 and 3. """

		return self.next()

	def reset(self):
		""" Set iterator to start of iteration. """

		self.node = self.head


class BinarySearchTree:
	""" Tree class consistent of nodes with left and right nodes. 
		Left of a node are items of less value than current, and greater to right. """

	def __init__(self, cmpfunc):
		""" Assign compare function. """

		self.root		= None
		self.head		= None
		self.children	= 0
		self.inserted	= True
		self.depth		= 0
		self.item		= None
		self.cmpfunc	= cmpfunc

	# Binary Search Tree; Insert, Add, Append, etc.: #
	def _insert(self, current = Node, key = None, item = None):
		""" Bottom-up insertion. """

		if current is None:
			self.head = Node(key, item, self.head)
			return self.head

		res = self.cmpfunc(key, current.key)

		if res < 0:
			current.left	= self._insert(current.left, key, item)
		elif res > 0:
			current.right	= self._insert(current.right, key, item)
		else:
			current.item = item
			self.inserted = False
		return current

	def insert(self, key = None, item = None):
		""" Adds item in tree by comparing key. 
			If key already associated with value in structure item will be overwritten. 
			Return True if inserted, return False if overwritten. """

		self.inserted = True

		self.root = self._insert(self.root, key, item)

		if self.inserted is True:
			self.children += 1

		return self.inserted

	def add(self, item = None):
		""" Insert item as key and item. """

		return self.insert(item, item)

	def append(self, item = None):
		""" Insert item as key and item. """

		return self.insert(item, item)

	# Binary Search Tree; Destroy: #
	def _percolate(self, current = Node, leftrotate = True):
		""" Alternate left- and right-rotation until node have one leaf child. """

		if current.left is None:
			return current.right
		elif current.right is None:
			return current.left
		elif leftrotate:
			current = current._rotate_left()
			current.left = self._percolate(current.left, False)
		else:
			current = current._rotate_right()
			current.right = self._percolate(current.right, True)

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
			current = self._percolate(current, True)
			self.children -= 1

		return current

	def remove(self, key = None):
		""" Removes item assosiated with key. """

		self.root = self._remove(self.root)

	def pop(self, key = None):
		""" Removes and return item assosiated with key. 
			Return None if key not in bst. """

		self.item = None
		self.root = self._remove(self.root, key)
		return self.item

	def clear(self):
		""" Removes all references to keys and items in BST. """

		self.root = None

	# Binary Search Tree; Search: #
	def _search(self, current = Node, key = None):
		""" Recursive search for key. """

		if current is None:
			return None

		res = self.cmpfunc(key, current.key)

		if res < 0:
			return self._search(current.left, key)
		elif res > 0:
			return self._search(current.right, key)
		else:
			return current.item

	def search(self, key = None):
		""" Return item associated with key. 
			Return None if key not in BST. """

		return self._search(self.root, key)

	def contains(self, key = None):
		""" Return True if key in BST. 
			Return False otherwise. """

		if self._search(self.root, key) is not None:
			return True
		return False

	# Binary Search Tree; Size / Depth: #
	def _depth(self, current = Node, level = 0):
		""" Recursive traversal and assignment of deepest recursion-depth. """

		if current is None:
			return

		self.depth = self.depth if self.depth > level else level

		self._depth(current.left, level+1)
		self._depth(current.right, level+1)

	def __len__(self, optional = False):
		""" Return size / length of BST. 
			If optional is True, return tuple with size and depth of tree. """

		if optional:
			self.depth = 0
			self._depth(self.root, 0)
			return self.children, self.depth

		return self.children

	# Binary Search Tree; Sort / Mergesort: #
	def _split(self, head = Node):
		""" Divide node-sequence in two. """

		slow = head
		fast = head.next

		while fast is not None and fast.next is not None:
			slow = slow.next
			fast = fast.next.next

		ndhead = slow.next
		slow.next = None

		return ndhead

	def _merge(self, a = Node, b = Node):
		""" Connect two sorted node-sequences. """

		if self.cmpfunc(a.key, b.key) < 0:
			head = a
			a = a.next
		else:
			head = b
			b = b.next
		tail = head

		while a is not None and b is not None:
			if self.cmpfunc(a.key, b.key) < 0:
				tail.next = a
				a = a.next
			else:
				tail.next = b
				b = b.next
			tail = tail.next

		if a is None:
			tail.next = b
		else:
			tail.next = a

		return head

	def _mergesort(self, head = Node):
		""" Split nodes, and merge sorted sequences. """

		if head is None or head.next is None:
			return head

		ndhead = self._split(head)

		head	= self._mergesort(head)
		ndhead	= self._mergesort(ndhead)

		return self._merge(head, ndhead)

	def sort(self):
		""" Sort BST head-node sequence. """

		self.head = self._mergesort(self.head)

	# Binary Search Tree; Iteration: #
	def _issorted(self, current = Node):
		""" Return True if node-sequence is in sorted order. 
			Return False otherwise. """

		while current is not None:
			cmpnode = current.next
			while cmpnode is not None:
				if self.cmpfunc(current.key, cmpnode.key) > 0:
					return False
				cmpnode = cmpnode.next
			current = current.next
		return True

	def __iter__(self, optional = False):
		""" Return iterator object for BST-structure. 
			If optional argument is True, then the sequence returned will be sorted. """

		if optional and not self._issorted(self.head):
			self.head = self._mergesort(self.head)

		return BSTIterator(self.head)

	# Binary Search Tree; String Representation: #
	def _cast_to_str(self, current = Node, rep = list):
		""" Recursive concatenation of items starting at bottom-left / least item. """

		if current is None:
			return

		self._cast_to_str(current.left, rep)
		rep.append( str(current.item) )
		self._cast_to_str(current.right, rep)

	def __str__(self):
		""" Return concatenated composition of items in BST. 
			NOTE: All inserted items must have a __str__-magic method. 
			If an item does not have this an AttributeError is raised. """

		if self.root is None:
			return "'[]'"

		rep = []
		
		try:
			self._cast_to_str(self.root, rep)
		except AttributeError:
			raise AttributeError
		
		strrep = "'[" + rep.pop(0)

		for string in rep:
			strrep += ", " + string

		return strrep + "]'"

	def __repr__(self):
		""" Return string representation. """

		return self.__str__()

	# Binary Search Tree; Indexing: #
	def _getfirst(self, current = Node):
		""" Return first / least item in RBT. """

		while current is not None:
			tmp = current
			current = current.left
		return tmp.item

	def _getlast(self, current = Node):
		""" Return last / largest item in RBT. """

		while current is not None:
			tmp = current
			current = current.right
		return tmp.item

	def _getitem(self, n = 0):
		""" Iterate BST and return n'th-item. """

		iterator = self.__iter__(True) # Must iterate in sorted order. 
		i = 0

		while i <= n:
			item = next(iterator)
			i += 1

		return item

	def __getitem__(self, index = 0):
		""" Return item at index-position. """

		if index < -1 or index >= self.children or self.children == 0:
			raise IndexError
		elif index == 0:
			return self._getfirst(self.root)
		elif index == -1:
			return self._getlast(self.root)
		else:
			return self._getitem(index)