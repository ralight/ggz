# GGZ Cards description file
Name = GGZCards-Suaro
ProtocolEngine = GGZCards
ProtocolVersion = 1
Description = GGZ Cards game module for Suaro
Author = Jason Short
Version = 0.0.4
Homepage = http://ggz.sourceforge.net

# PlayersAllowed and BotsAllowed should be repeated as many times as
# necessary to specify the valid distinct options which can appear

PlayersAllowed = 2

BotsAllowed = 0
BotsAllowed = 1

# Set AllowLeave to 1 if game supports players leaving during gameplay
AllowLeave = 1

# ExecutablePath is figured relative to GameDir unless starting with /
ExecutablePath = ggzd.ggzcards --game=suaro

# GameDisabled is a quick way to turn off the game if necessary
#GameDisabled
