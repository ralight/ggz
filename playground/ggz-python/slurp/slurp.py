#!/usr/bin/env python

"""
Slurp!
"""

from OpenGL.GL import *
import pygame
from pygame.locals import *
from random import *
from Numeric import *

xpos = 0
ypos = 0
textures1 = [0,0]
textures2 = [0,0]
texture1 = [[],[]]
texture2 = [[],[]]
starships = []
bullets = []
angle = 0
shipobject = [[],[]]
enemyobject = [[],[]]
bulletobject = [[], []]
shiptex = [0,0]
enemytex = [0,0]
bullettex = [0,0]

def resize((width, height)):
    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

def init():
    global texture1
    global texture2
    global starships
    global shipobject
    global enemyobject
    global bulletobject

    glShadeModel(GL_SMOOTH)
    glClearColor(0.0, 0.0, 0.0, 0.0)
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0)
    glEnable(GL_COLOR_MATERIAL)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)
    texture = pygame.image.load("starfield.png")
    texture1 = pygame.image.tostring(texture, "RGBX", 1)
    glBindTexture(GL_TEXTURE_2D, textures1[0])
    texture = pygame.image.load("starfield.png")
    texture2 = pygame.image.tostring(texture, "RGBX", 1)
    glBindTexture(GL_TEXTURE_2D, textures2[0])
    texture = pygame.image.load("ship.png")
    shipobject = pygame.image.tostring(texture, "RGBX", 1)
    glBindTexture(GL_TEXTURE_2D, shiptex[0])
    texture = pygame.image.load("enemy.png")
    enemyobject = pygame.image.tostring(texture, "RGBX", 1)
    glBindTexture(GL_TEXTURE_2D, enemytex[0])
    texture = pygame.image.load("bullet.png")
    bulletobject = pygame.image.tostring(texture, "RGBX", 1)
    glBindTexture(GL_TEXTURE_2D, bullettex[0])

    for i in range(10):
        ship = (randrange(-75, 75), randrange(-75, 75), randrange(-75, 75), randrange(-75, 75))
        starships.append(ship)

def draw():
    global xpos
    global ypos
    global texture1
    global texture2
    global starships
    global angle
    global shipobject
    global bulletobject
    global bullets

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity()

    glPushMatrix()

    glRotatef(angle, 0.0, 0.0, 1.0)

    glScalef(3.0, 3.0, 3.0)

    """ Background """

    glTranslatef(xpos, ypos, 0)

    glEnable(GL_TEXTURE_2D)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture1);

    glColor3f(1.0, 0.0, 0.0)
    glBegin(GL_QUADS)
    glTexCoord2f(0.0, 0.0)
    glVertex3f(-1.0, -1.0, 0.0)
    glTexCoord2f(0.0, 10.0)
    glVertex3f(-1.0, 1.0, 0.0)
    glTexCoord2f(10.0, 10.0)
    glVertex3f(1.0, 1.0, 0.0)
    glTexCoord2f(10.0, 0.0)
    glVertex3f(1.0, -1.0, 0.0)
    glEnd()

    glDisable(GL_TEXTURE_2D)

    """ Background blend """

    glTranslatef(xpos, ypos, 0)

    glScalef(2.0, 2.0, 1.0)

    glEnable(GL_TEXTURE_2D)
    glEnable(GL_BLEND)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture2);

    glColor4f(1.0, 1.0, 0.0, 0.5)
    glBegin(GL_QUADS)
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-1.0, -1.0, 0.1)
    glTexCoord2f(0.0, 10.0);
    glVertex3f(-1.0, 1.0, 0.1)
    glTexCoord2f(10.0, 10.0);
    glVertex3f(1.0, 1.0, 0.1)
    glTexCoord2f(10.0, 0.0);
    glVertex3f(1.0, -1.0, 0.1)
    glEnd()

    glDisable(GL_BLEND)
    glDisable(GL_TEXTURE_2D)

    glPopMatrix()

    """ Ship """

    glPushMatrix()

    glTranslatef(xpos, ypos, 0)

    glEnable(GL_TEXTURE_2D)
    glEnable(GL_BLEND)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, shipobject);

    glColor3f(1.0, 1.0, 1.0)
    glBegin(GL_QUADS)
    glTexCoord2f(0.0, 0.0)
    glVertex3f(-0.2, -0.2, 0.1)
    glTexCoord2f(0.0, 1.0)
    glVertex3f(-0.2, 0.2, 0.1)
    glTexCoord2f(1.0, 1.0)
    glVertex3f(0.2, 0.2, 0.1)
    glTexCoord2f(1.00, 0.0)
    glVertex3f(0.2, -0.2, 0.1)
    glEnd()

    glDisable(GL_BLEND)
    glDisable(GL_TEXTURE_2D)

    glPopMatrix()

    """ Bullets """

    glPushMatrix()

    glTranslatef(xpos, ypos, 0)

    glEnable(GL_TEXTURE_2D)
    glEnable(GL_BLEND)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, bulletobject);

    for bulletpos in bullets:
        (x, y) = bulletpos

        glTranslatef(x, y, 0)

        glColor3f(1.0, 1.0, 1.0)
        glBegin(GL_QUADS)
        glTexCoord2f(0.0, 0.0)
        glVertex3f(-0.02, -0.02, 0.1)
        glTexCoord2f(0.0, 1.0)
        glVertex3f(-0.02, 0.02, 0.1)
        glTexCoord2f(1.0, 1.0)
        glVertex3f(0.02, 0.02, 0.1)
        glTexCoord2f(1.00, 0.0)
        glVertex3f(0.02, -0.02, 0.1)
        glEnd()

        glTranslate(-x, -y, 0)

    glDisable(GL_BLEND)
    glDisable(GL_TEXTURE_2D)

    glPopMatrix()

    """ Minimap """

    glPushMatrix()

    glTranslatef(0.6, -0.9, 0.0)
    glScalef(0.3, 0.3, 0.3)

    glEnable(GL_BLEND)

    glColor4f(0.0, 0.0, 0.4, 0.2)
    glBegin(GL_QUADS)
    glVertex3f(0.0, 0.0, 0.2)
    glVertex3f(0.0, 1.0, 0.2)
    glVertex3f(1.0, 1.0, 0.2)
    glVertex3f(1.0, 0.0, 0.2)
    glEnd()

    """ Self on minimap """

    glColor4f(0.0, 0.0, 0.2, 0.2)
    glBegin(GL_QUADS)
    glVertex3f(0.5 - xpos * 0.6 + 0.04, 0.5 - ypos * 0.6 + 0.04, 0.3)
    glVertex3f(0.5 - xpos * 0.6 + 0.04, 0.5 - ypos * 0.6 - 0.04, 0.3)
    glVertex3f(0.5 - xpos * 0.6 - 0.04, 0.5 - ypos * 0.6 - 0.04, 0.3)
    glVertex3f(0.5 - xpos * 0.6 - 0.04, 0.5 - ypos * 0.6 + 0.04, 0.3)
    glEnd()

    """ Ships on minimap """

    for i in range(10):
        ship = starships[i]
        x = ship[0] / 100.0
        y = ship[1] / 100.0
        glColor4f(0.4, 0.0, 0.0, 0.3)
        glBegin(GL_QUADS)
        glVertex3f(0.5 - x * 0.6 + 0.0125, 0.5 - y * 0.6 + 0.0125, 0.3)
        glVertex3f(0.5 - x * 0.6 + 0.0125, 0.5 - y * 0.6 - 0.0125, 0.3)
        glVertex3f(0.5 - x * 0.6 - 0.0125, 0.5 - y * 0.6 - 0.0125, 0.3)
        glVertex3f(0.5 - x * 0.6 - 0.0125, 0.5 - y * 0.6 + 0.0125, 0.3)
        glEnd()

    glDisable(GL_BLEND)

    glPopMatrix()

def main():
    global xpos
    global ypos
    global angle
    global bullets

    pygame.init()
    pygame.display.set_caption("Slurp")

    screen = pygame.display.set_mode((1024, 768), OPENGL | DOUBLEBUF | FULLSCREEN, 0)

    pygame.mouse.set_visible(0)

    resize((1024, 768))
    init()

    ticks = pygame.time.get_ticks()
    frames = 0
    while 1:

        pygame.event.pump()

        keyinput = pygame.key.get_pressed()

        if keyinput[K_ESCAPE] or pygame.event.peek(QUIT):
            break

        speed = 0.001
        if keyinput[K_LSHIFT] or keyinput[K_RSHIFT]:
            speed = 0.01
        if keyinput[K_DOWN]:
            if ypos < 0.75:
                ypos = ypos + speed
        if keyinput[K_UP]:
            if ypos > -0.75:
                ypos = ypos - speed
        if keyinput[K_RIGHT]:
            if xpos > -0.75:
                xpos = xpos - speed
            if keyinput[K_LCTRL] or keyinput[K_RCTRL]:
                angle = angle + 1
        if keyinput[K_LEFT]:
            if xpos < 0.75:
                xpos = xpos + speed
            if keyinput[K_LCTRL] or keyinput[K_RCTRL]:
                angle = angle - 1
        if keyinput[K_SPACE]:
            bulletpos = (-0.1, 0.0)
            bullets.append(bulletpos)
            bulletpos = (+0.1, 0.0)
            bullets.append(bulletpos)

        for i in range(10):
            ship = starships[i]
            xdir = 0
            ydir = 0
            if ship[2] != ship[0]:
                xdir = (ship[2] - ship[0]) / abs(ship[2] - ship[0])
            if ship[3] != ship[1]:
                ydir = (ship[3] - ship[1]) / abs(ship[3] - ship[1])
            if ship[2] == ship[0] and ship[1] == ship[3]:
                ship = (ship[0], ship[1], randrange(-75, 75), randrange(-75, 75))
            ship = (ship[0] + xdir, ship[1] + ydir, ship[2], ship[3])
            starships[i] = ship

        for i in range(len(bullets)):
            (x, y) = bullets[i]
            bullets[i] = (x, y + 0.02)

        draw()

        pygame.display.flip()

        frames = frames + 1
    print "fps: %d" % ((frames * 1000) / (pygame.time.get_ticks() - ticks))

if __name__ == '__main__': main()

