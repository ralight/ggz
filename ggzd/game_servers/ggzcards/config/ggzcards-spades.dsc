# GGZ Cards description file
Name = GGZCards-Spades
Version = 0.0.4
ProtocolEngine = GGZCards
ProtocolVersion = 1
Description = GGZ game module for trick-taking card games
Author = Jason Short, Rich Gade
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
ExecutablePath = ggzd.ggzcards --game=spades

# GameDisabled is a quick way to turn off the game if necessary
#GameDisabled
