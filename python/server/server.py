# !usr/bin/env python3.6

# Author: Marius Ingebrigtsen

import socket
import pickle


class Server:
	""" Server class for use with arbitrary game-module. """
	def __init__(self, module = None, ip = None, port = None, fps = 60):
		""" module:		Game-object to use with servers. 
						Must have a 'update'-method taking a client and message sent from the client-side. 
						Must have a 'status'-method returning a list with information pertaining to the game that connected clients need to recieve. 
						Must have a 'add_client'-method performing all the neccessary internal module management with use of client class. 
			ip: 		Set to '127.0.0.1' if argument not provided. 
			port:		Set to '43567' if argument not provided. 
			fps:		Frames per second intended to run the program with (set to 60 without default). """
		self.connected = {}
		self.client_count = 0
		self.module		= module
		self.ip			= "127.0.0.1" if port is None else ip
		self.port		= 43567 if port is None else port
		self.time		= 1 / fps
		self.done		= False

		if self.module is None:
			raise TypeError("Invalid module {}.".format(self.module))

	def serve(self):
		""" Continuously recieve, send and wait. """
		with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as self.socket:
			self.socket.bind( ("", self.port) )
			self.socket.setblocking(False)

			while not self.done:
				self.recieve()

				self.module.update()

				self.send()

				time.sleep(self.sleep)

	def recieve(self):
		""" Recieve all pending messages, and connect and update local client references. """
		recieving = True

		while recieving:
			try:
				msg, addr = self.socket.recvfrom(8192)
			except socket.error:
				recieving = False
			else:
				msg = pickle.loads(msg)
				if msg.id not in self.connected.keys():
					self.connect(msg, addr)

				elif addr != self.connected[msg.id].addr:
					self.socket.sendto(pickle.dumps(Exception("Client-ID already taken.")), addr)
					continue

				else:
					for client in self.connected:
						if msg.id == client.id:
							client.update(msg)

	def connect(self, msg, addr):
		""" Add and return new client to local server reference. """
		c = Client(msg, addr, self.module)
		c.update(msg)
		self.connected[msg.id] = c
		self.client_count += 1
		print("User {} connected.".format(msg.id))

	def send(self):
		""" Retrieve entire status of game from module and send it to all clients. """
		status = pickle.dump(self.module.status())

		for client in self.connected.values():
			self.socket.sendto(status, client.addr)


class Client:
	"""  """

	def __init__(self, msg, addr, module):
		"""  """
		self.id		= msg.id
		self.addr	= addr
		self.module	= module
		self.module.add_client(self)

	def update(self, msg):
		"""  """
		self.module.update_client(self)


if __name__ == "__main__":
	pass