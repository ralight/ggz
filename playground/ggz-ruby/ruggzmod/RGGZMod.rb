#!/usr/bin/env ruby
#
# High-level interface around low-level GGZMod wrapper
# Copyright (C) 2007, 2008 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

require "GGZMod"

$singleton = nil

# This class represents a GGZ player or spectator.
# It holds information from the 'rankings', 'stats' and 'info' events.
class Player
	attr_reader :name, :position, :score, :realname, :photo

	def initialize(name)
		@name = name
		@position = nil
		@score = nil
		@realname = nil
		@photo = nil
	end

	def setrankings(position, score)
		@position = position
		@score = score
	end

	def setinfo(realname, photo)
		@realname = realname
		@photo = photo
	end

	def to_s
		s = "<GGZ Player #{@name}"
		if @realname or @photo
			s += " (realname #{@realname}, photo #{@photo})"
		end
		if @position or @score
			s += " (score #{@score}, position #{@position})"
		end
		s += ">"
		return s
	end
end

# This class represents a GGZ player who currently spectates
class Spectator < Player
	attr_reader :seatnum

	def initialize(name, seatnum)
		super(name)
		@seatnum = seatnum
	end

	def to_s
		player = super()
		return "<GGZ spectator ##{@seatnum}; player: #{player}>"
	end
end

# This class represents a GGZ player who currently plays
class Seat < Player
	attr_reader :seatnum, :type, :hostname

	def initialize(name, seatnum, type)
		super(name)
		@seatnum = seatnum
		@type = type
		@hostname = nil
	end

	def sethostname(hostname)
		@hostname = hostname
	end

	def typename()
		typenames = {
			GGZMod::SEATNONE => nil,
			GGZMod::SEATOPEN => "open",
			GGZMod::SEATBOT => "bot",
			GGZMod::SEATPLAYER => "player",
			GGZMod::SEATRESERVED => "reserved",
			GGZMod::SEATABANDONED => "abandoned"
		}
		return typenames[@type]
	end

	def to_s
		if @type != GGZMod::SEATOPEN and @type != GGZMod::SEATNONE:
			player = super()
		end
		type = typename()
		playerstr = nil
		hoststr = nil
		if player
			playerstr = ", player #{player}"
		end
		if @hostname
			hoststr = ", hostname #{@hostname}"
		end
		return "<GGZ seat ##{@seatnum} (type #{type}#{playerstr}#{hoststr})>"
	end
end

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
		@players = Hash.new
		# holds information on all players and spectators, key is player name
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
			GGZMod::EVENTRANKINGS => "rankings",
			GGZMod::EVENTERROR => "error"
		}
		name = names[id]
		puts "== Event handler called! id = " + id.to_s + ", name = " + name.to_s

		if id == GGZMod::EVENTSTATS
			# FIXME: data not filled yet
		end

		if id == GGZMod::EVENTINFO
			data.each do |info|
				(realname, photo, host) = ranking
				# FIXME: name is missing here!
				name = nil
				if not @players[name]
					@players[name] = Player.new(name)
				end
				@players[name].setinfo(realname, photo, host)
			end
		end

		if id == GGZMod::EVENTRANKINGS
			data.each do |ranking|
				(name, position, score) = ranking
				if not @players[name]
					@players[name] = Player.new(name)
				end
				@players[name].setposition(position, score)
			end
		end
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

