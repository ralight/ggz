# Keepalive scripted extensions
# Copyright (C) 2002 Josef Spillner, dr_maux@users.sourceforge.net
# Published under GNU GPL conditions

# Global variable
ref = 0

# Count the number of players
def playercount():
	print "Count the number of players..."
	return 99

# Unhide the map
def fogofwar():
	global ref
	print "Erase all fog of war"
	ref = ref + 1
	print "Reference: ", str(ref)
	return 0

# Direct invokation
if __name__ == '__main__':
	fogofwar()
	fogofwar()

