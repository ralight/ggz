# $Id: ggzcards-hearts.dsc 2189 2001-08-23 07:59:17Z jdorje $

Name = GGZCards-Hearts
ProtocolEngine = GGZCards
ProtocolVersion = 1
Description = GGZ Cards game module for Hearts
Author = Jason Short
Version = 0.0.4
Homepage = http://ggz.sourceforge.net

# PlayersAllowed and BotsAllowed should be repeated as many times as
# necessary to specify the valid distinct options which can appear

PlayersAllowed = 3
PlayersAllowed = 4
PlayersAllowed = 5
PlayersAllowed = 6

BotsAllowed = 0
BotsAllowed = 1
BotsAllowed = 2
BotsAllowed = 3
BotsAllowed = 4
BotsAllowed = 5

# Set AllowLeave to 1 if game supports players leaving during gameplay
AllowLeave = 1

# ExecutablePath is figured relative to GameDir unless starting with /
ExecutablePath = ggzd.ggzcards --game=hearts --option=open_hands:0

# GameDisabled is a quick way to turn off the game if necessary
# GameDisabled
