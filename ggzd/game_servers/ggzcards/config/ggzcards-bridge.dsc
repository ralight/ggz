# $Id: ggzcards-bridge.dsc 2189 2001-08-23 07:59:17Z jdorje $

Name = GGZCards-Bridge
ProtocolEngine = GGZCards
ProtocolVersion = 1
Description = GGZ Cards game module for Bridge
Author = Jason Short
Version = 0.0.4
Homepage = http://ggz.sourceforge.net

# PlayersAllowed and BotsAllowed should be repeated as many times as
# necessary to specify the valid distinct options which can appear
PlayersAllowed = 4

BotsAllowed = 0
BotsAllowed = 1
BotsAllowed = 2
BotsAllowed = 3

# Set AllowLeave to 1 if game supports players leaving during gameplay
AllowLeave = 1

# ExecutablePath is figured relative to GameDir unless starting with /
ExecutablePath = ggzd.ggzcards --game=bridge --option=open_hands:0

# GameDisabled is a quick way to turn off the game if necessary
# GameDisabled
