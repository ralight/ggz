#!/usr/bin/env ruby

require "GGZDMod"

puts "Connecting to GGZ server..."

server = GGZDMod.new
server.connect
server.loop

puts "Done."

