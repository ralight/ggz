Author = Rich Gade, Jason Short
Homepage = http://ggz.sourceforge.net
Version = 0.0.1
# GameDisabled is a quick way to turn off the game if necessary
#GameDisabled


# Generalized bridge-like game description
Name = GGZCards
Description = GGZ game module for Bridge-like card games
# PlayersAllowed and BotsAllowed should be repeated as many times as
# necessary to specify the valid distinct options which can appear
PlayersAllowed = 2
PlayersAllowed = 4
BotsAllowed = 0
# Set AllowLeave to 1 if game supports players leaving during gameplay
AllowLeave = 1
# ExecutablePath is figured relative to GameDir unless starting with /
ExecutablePath = ggzd.ggzcards


# An example description for a specific game
# Name = GGZCards-Spades
# Description = GGZ Game Module for Spades
# PlayersAllowed = 4
# BotsAllowed = 0
# AllowLeave = 1
# ExecutablePath = ggzd.lapocha --game=spades
