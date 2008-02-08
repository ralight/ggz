#!/usr/bin/ruby
#
# Test program simulating the capabilities of grubby's mod_embed
# Commands:
# (none)

debug = false

if $answer
	input = $answer[0]
	$answer = nil
	embedded = true
else
	input = gets
	embedded = false
end

if debug
	puts "Hello World."
	puts "The message I got is: <" + input + ">"
end

input.gsub!(/[0-9]/, '***')

if debug
	puts "Let's return <" + input + ">"
end

if embedded
	$answer[0] = input
else
	puts input
end

