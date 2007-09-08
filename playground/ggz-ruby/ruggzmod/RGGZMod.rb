#!/usr/bin/env ruby
#
# High-level interface around low-level GGZMod wrapper
# Copyright (C) 2007 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

require "GGZMod"

$singleton = nil

class RGGZMod
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

def ggzmod_handler(id, data)
	puts "## Callback called! id = " + id.to_s
	$singleton.event(id, data)
end

