#!/usr/bin/env python
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import sys
import pygame
from pygame.locals import *
import sdlnewstuff
import playertable

def main(fullscreen):
	""" Pygame setup """

	pygame.init()
	pygame.display.set_caption("Get Hot New Stuff Test")

	if fullscreen:
		screen = pygame.display.set_mode((1024, 768), DOUBLEBUF)
		pygame.display.toggle_fullscreen()
	else:
		screen = pygame.display.set_mode((800, 600), DOUBLEBUF)

	pygame.display.flip()

	#sdlnewstuff.gethotnewstuff("fyrdman")
	playertable.show()

if __name__ == "__main__":
	if len(sys.argv) == 2 and sys.argv[1] == "-f":
		main(1)
	else:
		main(0)

