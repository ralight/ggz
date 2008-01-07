#!/usr/bin/env ruby
#
# High-level interface around low-level GGZMod wrapper
# Copyright (C) 2007 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

require "GGZMod"

$singleton = nil

# This class implements GGZ functionality for game clients.
# For games to be able to run on GGZ, derive from this class and add all the
# functionality that you need. To reimplement its methods, add 'super' as
# needed within the methods. Finally, instantiate an object of this class
# and call loop() on it.
class RGGZMod
	# Initialise the GGZ functionality for the game.
	# This constructor will raise an exception if this is not possible
	def initialize
		puts "== Initialisation"
		if not ENV['GGZMODE']
			# FIXME: we silently go over this in ggzmod (legacy relict?)
			raise "Error: not in GGZ mode"
		end
		@client = GGZMod.new
		ret = @client.connect
		if not ret
			raise "Error: could not connect to GGZ core client"
		end
		$singleton = self
	end

	# Arrival of events from the GGZ server through the GGZ core client.
	# Override thie method and handle as many events as possible.
	# The most important one is probably GGZMod::EVENTSERVER. Its data
	# will be a file descriptor for a direct connection to the game server.
	# Have a look at the GGZMod documentation for the meaning of all the
	# possible events and their data.
	def event(id, data)
		names = {
			GGZMod::EVENTSTATE => "state",
			GGZMod::EVENTSERVER => "server",
			GGZMod::EVENTPLAYER => "player",
			GGZMod::EVENTSEAT => "seat",
			GGZMod::EVENTSPECTATOR => "spectator",
			GGZMod::EVENTCHAT => "chat",
			GGZMod::EVENTSTATS => "stats",
			GGZMod::EVENTINFO => "info",
			GGZMod::EVENTERROR => "error"
		}
		name = names[id]
		puts "== Event handler called! id = " + id.to_s + ", name = " + name.to_s
	end

	# Enter the main loop and wait for events reported by the GGZ core client.
	# In nearly all cases you definitely want to override this method, or
	# simply not use it, since it blocks your application.
	# Instead, call get_control_fd() and add the returned file descriptor to the
	# event loop mechanism of your application toolkit.
	# For example, use QSocketNotifier with Qt4.
	def loop
		fd = @client.get_control_fd
		controlchannel = IO.new(fd, "w+")
		while true
			puts "=> select"
			ret = IO.select([controlchannel], nil, nil, nil)
			if ret
				puts "** dispatch"
				ret = @client.dispatch
				if ret
					## ?
				end
			end
			puts "<= select"
		end
	end
end

# Static global method which is being used as a callback.
# You do not need to reimplement this method, unless you want to
# override the RGGZMod default event handler.
def ggzmod_handler(id, data)
	puts "## Callback called! id = " + id.to_s
	$singleton.event(id, data)
end

