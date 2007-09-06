#!/usr/bin/env ruby
#
# High-level interface around low-level GGZMod wrapper
# Copyright (C) 2007 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

require "GGZMod"

$singleton = nil

class RGGZMod
	def initialize
		if not ENV['GGZMODE']
			# FIXME: we silently go over this in ggzmod (legacy relict?)
			raise "Error: not in GGZ mode"
		end
		@server = GGZMod.new
		ret = @server.connect
		if not ret
			raise "Error: could not connect to GGZ core client"
		end
		#ret = $server.dispatch
		$singleton = self
	end

	def event(id, data)
	end
end

def ggzmod_handler(id, data)
	puts "## Callback called!"
	$singleton.event(id, data)
end

