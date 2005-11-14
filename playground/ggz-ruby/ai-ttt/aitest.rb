#!/usr/bin/env ruby

require "TTTAI"

puts "Test of Ruby bindings for the Tic-Tac-Toe AI"

board = Array.new(9)
board[4] = 1
board[0] = 0
board[1] = 0

ai = TTTAI.new
move = ai.ai_findmove(0, 1, board)

puts "Move result:"
puts move

