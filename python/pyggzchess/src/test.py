#!/usr/bin/env python
#
# Test game client for ggzchess
# Written in Python

import ggzchess

print ">> test pyggzchess"
ggzchess.init(ggzchess.WHITE, 4)

print ">> debug"
ggzchess.output()

print ">> move pawn away"
ret = ggzchess.move(12, 28, 0)
ret = ggzchess.move(28, 36, 0)
ret = ggzchess.move(36, 44, 0)

ret = ggzchess.move(53, 44, 0)
ret = ggzchess.move(44, 36, 0)
ret = ggzchess.move(52, 44, 0)

print ">> try a king move"
ret = ggzchess.move(4, 12, 0)
ret = ggzchess.move(12, 20, 0)
ret = ggzchess.move(20, 28, 0)
ret = ggzchess.move(28, 36, 0)
ret = ggzchess.move(36, 44, 0)

ret = ggzchess.move(44, 52, 0)

print ">> debug after result", ret
ggzchess.output()

