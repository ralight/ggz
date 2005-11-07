#!/usr/bin/env ruby

require "GGZDMod"

puts "Test of Ruby bindings for GGZDMod"

server = GGZDMod.new
#server.connect
server.loop

puts "Player data event id:"
puts GGZDMod::EVENTDATA

#server.callback_test(:callback)

puts "Done."

