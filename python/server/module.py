# !usr/bin/env python3.6

# Author: Marius Ingebrigtsen

import pygame as pg
from random import randint


class Ball(pg.sprite.Sprite):
	""" Ball. """

	def __init__(self, players, surface):
		""" Init. """
		super().__init__()

		self.vel_vect	= pg.Vector2(randint(-5, 5), randint(-2, 2))
		self.rect		= pg.Rect(0, 0, 20, 20)
		self.rect.center= (400, 400)
		self.radius		= int(self.rect.w / 2)

		self.players	= players
		self.surface	= surface

	def update(self):
		""" Update. """
		self.move()
		self.draw()

	def move(self):
		""" Move. """
		nextpos = self.rect.move(self.vel_vect)

		if nextpos.x - self.vel_vect.x < 0 or nextpos.right + self.vel_vect.x > 800:
			self.vel_vect.x *= -1
		elif nextpos.y - self.vel_vect.y < 0 or nextpos.y + self.vel_vect.y > 800:
			self.vel_vect.y *= -1
		else:
			for player in self.players:
				if pg.sprite.collide_rect(self, player):
					self.vel_vect.x *= -1
					break

		self.rect = self.rect.move(self.vel_vect)

	def draw(self):
		""" Draw. """
		pg.draw.circle(self.surface, (255, 255, 255), self.rect.center, self.radius)


class Player(pg.sprite.Sprite):
	""" Player. """

	def __init__(self, player, surface):
		""" Init. """
		super().__init__()

		self.move	= 15
		x = 10 if player == 1 else 780
		y = 800 / 2
		self.rect	= pg.Rect(x, y, 10, 50)

		self.surface = surface

	def update(self):
		""" Update. """
		self.draw()

	def up(self):
		""" Up. """
		self.rect.y -= self.move

	def down(self):
		""" Down. """
		self.rect.y += self.move

	def draw(self):
		""" D.R.A.W. """
		pg.draw.rect(self.surface, (255, 255, 255), self.rect)


class Pong:
	""" Game. """
	def __init__(self):
		""" Init. """
		pg.init()
		self.surface = pg.display.set_mode( (800, 800) )
		pg.display.set_caption("Pong the Gaem")

		self.clock = pg.time.Clock()
		self.done = False

		self.player = Player(1, self.surface)
		self.players = pg.sprite.Group()
		self.players.add(self.player)
		self.ball = Ball(self.players, self.surface)

	def play(self):
		""" Play. """
		self.done = False

		while not self.done:
			self.surface.fill( (0,0,0) )

			self.ball.update()
			self.players.update()

			pg.display.update()

			self.events()
			
			self.clock.tick(60)

		pg.quit()

	def events(self):
		""" Events. """
		for event in pg.event.get():
			if event.type == pg.KEYDOWN:
				if event.key == pg.K_UP:
					self.player.up()
				elif event.key == pg.K_DOWN:
					self.player.down()

			if event is pg.QUIT or (event.type == pg.KEYDOWN and event.key == pg.K_ESCAPE):
				self.done = True


if __name__ == "__main__":
	p = Pong()

	p.play()

	exit()