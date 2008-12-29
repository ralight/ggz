# GGZ Starterpack for C: Game client description file
# This file is being installed with ggz-config into the GGZ game module
# registry, so that it can be found by any GGZ core client. The registration
# is not necessary for game clients with embedded GGZ mode, however for those
# modversion.h contains the necessary values extracted from module.dsc (this
# file) so the game client knows about its own name and version.

[ModuleInfo]
Name = GGZ Sample Client for C
Version = 1.0
Author = You!
CommandLine = ggz-sample-client-c
Frontend = gtk
Homepage = http://www.ggzgamingzone.org/
ProtocolEngine = Starterpack
ProtocolVersion = 1.0
