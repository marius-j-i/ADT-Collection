# !usr/bin/env python3
### Author: Marius Ingebrigtsen ###

from sys import path


def cmpint(a, b):
	""" Comparison function for numbers. """

	return a - b

class EntryIterator:
	""" Iterator class for map entry. """

	def __init__(self, entry = MapEntry):
		""" Assign entry values. """

		self.head = self.entry = entry

	def next(self):
		""" Return next entry in sequence. """

		try:
			while True:
				yield self.entry
				self.entry = self.entry.next
		except AttributeError:
			raise StopIteration

	def __next__(self):
		""" Function of both python 2 and 3. """

		return self.next()

class MapEntry:
	""" Class for entries into hashmap. """

	def __init__(self, key = None, value = None, hashv = None, nextnode = None):
		""" Create entry and assign values. """

		self.key	= key
		self.value	= value
		self.hashv	= hashv
		self.next	= nextnode

	def iter(self):
		""" Return iterator over entry. """

		return EntryIterator(self)

	def __iter__(self):
		""" Function of both python 2 and 3. """

		return self.iter()


class HashMap:
	""" Hash-Map ADT. 
		Collision strategy is chained addressing with Red-Black Tree structures. """

	# Map Create: #
	def __init__(self, cmpfunc = cmpint, hashfunc = hash, STARTENTRIES = 2**10):
		""" Hash-Map Structure. """

		if STARTENTRIES < 2**10:
			STARTENTRIES = 2**10

		self.table		= [None for i in range(STARTENTRIES)] # Essentially calloc(). 
		self.entries	= 0
		self.maxentries	= STARTENTRIES
		self.cmpfunc	= cmpfunc
		self.hashfunc	= hashfunc

	# Map Destroy: #
	def clear(self):
		""" Empties all entries. """

		self.table		= [None for i in range(self.maxentries)]
		self.entries	= 0

	# Map Put: #
	def _resizeput(self, key = None, value = None):
		""" Same functionality as 'put()', but without size- or containment-checks. """

		hashv	= self.hashfunc(key)
		indx	= hashv % self.maxentries

		if self.table[indx] is None:
			self.table[indx] = MapEntry(key, value, hashv, None)
		else:
			self.table[indx] = MapEntry(key, value, hashv, self.table[indx])

		self.entries += 1

	def _resize(self):
		""" Increase scope of map, and re-address each entry. """

		self.entries	= 0
		self.maxentries	*= 2
		oldtable		= self.table
		self.table		= [None for i in range(self.maxentries)]

		for indx in oldtable:
			if indx is not None:
				for entry in indx:
					self._resizeput(entry.key, entry.value)

	def put(self, key = None, value = None):
		""" Maps key to value. 
			If key already in map, value is overwritten by argument-value. 
			Return True if put into map. 
			Return False if value overwritten. """

		if self.entries >= self.maxentries:
			self._resize()

		hashv	= self.hashfunc(key)
		indx	= hashv % self.maxentries

		if self.table[indx] is None:
			self.table[indx] = MapEntry(key, value, hashv, None)

		else:
			for entry in self.table[indx]:
				if hashv == entry.hashv and self.cmpfunc(key, entry.key) == 0:
					entry.value = value
					return False

			self.table[indx] = MapEntry(key, value, hashv, self.table[indx])

		self.entries += 1

		return True

	# Map Has Key: #
	def haskey(self, key = None):
		""" Return True if key is associated with value in map.
			Return False otherwise. """

		hashv	= self.hashfunc(key)
		indx	= hashv % self.maxentries

		if self.table[indx] is not None:
			for entry in self.table[indx]:
				if hashv == entry.hashv and self.cmpfunc(key, entry.key) == 0:
					return True
		return False

	# Map Get: #
	def get(self, key = None):
		""" Return value associated with key. 
			Return None if key not associated in map. """

		hashv	= self.hashfunc(key)
		indx	= hashv % self.maxentries

		if self.table[indx] is not None:
			for entry in self.table[indx]:
				if hashv == entry.hashv and self.cmpfunc(key, entry.key) == 0:
					return entry.value
		return None

	# Map Size: #
	def __len__(self):
		""" Size / length of map. """

		return self.entries


if __name__ == "__main__":
	pass