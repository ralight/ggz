#!/usr/bin/env python
# Copyright (C) 2002 Josef Spillner <josef@ggzgamingzone.org>

"""
Dresden - The Flood Game
This game is dedicated to the victims and the helpers of the flood catastrophy
everywhere.
As a Dresden inhabitant, I hope that it'll make us laugh about this one day.
"""

import pygame, pygame.transform, pygame.image
from random import *
from math import *
from pygame.locals import *
from Numeric import *
import os, sys

path = sys.argv[0]
path = os.path.abspath(path)

if path.startswith("/usr/local"):
	DATAPATH = "/usr/local/share/ggz/dresden/"
elif path.startswith("/usr"):
	DATAPATH = "/usr/share/ggz/dresden/"
else:
	DATAPATH = "./"

RES = array((800, 600))
TILES = zeros([16, 20], Int)

tide = 10

def main():
    pygame.init()
    pygame.display.set_caption("Dresden")

    screen = pygame.display.set_mode(RES, 0, 24)
    water = pygame.Surface((RES[0], RES[1]), 0, 24)

    guy = pygame.image.load(DATAPATH + "saggse.png")
    stone = pygame.image.load(DATAPATH + "stone.png")

    font = pygame.font.Font(None, 48)
    flood = font.render("Flood!", 1, (255, 255, 200))

    angle = 0
    level = 0
    guy_x = 100
    guy_y = 200
    nscore = 0
    gogame = 1

    TILES[14][12] = 1

    while 1:

        pygame.event.pump()

        keyinput = pygame.key.get_pressed()

        if keyinput[K_ESCAPE] or pygame.event.peek(QUIT):
            return

        if gogame == 1:
            if keyinput[K_RIGHT]:
                if(guy_x < water.get_width() - 150 - 10):
                    guy_x = guy_x + 10
            if keyinput[K_LEFT]:
                if(guy_x > 0 + 10):
                    guy_x = guy_x - 10
            if keyinput[K_SPACE]:
                if guy_y < water.get_height():
                    guy_y = guy_y + 20
            if (keyinput[K_LSHIFT] or keyinput[K_RSHIFT]):
                if TILES[guy_x / 50][(water.get_height() - guy_y) / 30] == 1:
                    TILES[guy_x / 50][(water.get_height() - guy_y) / 30] = 0
                    nscore = nscore + 10
                if TILES[(guy_x + 150) / 50][(water.get_height() - guy_y) / 30] == 1:
                    TILES[(guy_x + 150) / 50][(water.get_height() - guy_y) / 30] = 0
                    nscore = nscore + 10

        if guy_y > 200:
            guy_y = guy_y - 5

        x = randrange(0, 49)
        if (x == 0):
            x = randrange(0, 15)
            y = randrange(0, 14)
            TILES[x][y] = 1

        water.fill((0, 0, 0))

        for j in range(20):
            for i in range(16):
                if TILES[i][j] == 0:
                    water.blit(stone, (i * 50, j * 30))

        for j in range(20):
            for i in range(16):
                if TILES[i][j] == 1:
                    level = level + 0.05
                    for l in range(j * 30 + 20, water.get_height() - level):
                        for k in range(i * 50 + l / 50, i * 50 + 49 - l / 50):
                           blue = 200 + (l + k - angle * 10) % 55
                           water.set_at((k, l), (0, 0, blue))

        water.blit(guy, (guy_x, water.get_height() - guy_y))

        if gogame == 1:
            angle = angle + 0.1
            wave = sin(angle) * tide
            level = level + 0.07

        water.fill((0, 0, 255), ([0, water.get_height() - level + 1, water.get_width(), level]))
        water.fill((0, 0, 255), ([0, water.get_height() - level, water.get_width(), tide + wave]))

        water.blit(flood, (20, 30))

        if gogame == 1:
            nscore = nscore + 1
        score = font.render("Score: " + str(nscore), 1, (255, 255, 200))
        water.blit(score, (160, 30))

        if level > 180:
            gameover = font.render("Game Over", 1, (255, 255, 200))
            water.blit(gameover, (350, 30))
            gogame = 0

        screen.blit(water, (0, 0))

        pygame.display.update()

if __name__ == '__main__': main()

