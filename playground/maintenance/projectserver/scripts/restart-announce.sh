#!/bin/sh
# GGZ Gaming Zone Maintenance Script
# Announce a server restart
# Run as user ggz

ggz-cmd live.ggzgamingzone.org \
	checkonline ***** \
	announce \
	"Maintenance: Server will be restarted in 2 minutes"


