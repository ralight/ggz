#!/usr/bin/env ruby

require "socket"

class GGZRawSocket < Socket
	def puti(n)
		n1 = (n >> 24) & 0xff
		n2 = (n >> 16) & 0xff
		n3 = (n >> 8) & 0xff
		n4 = (n >> 0) & 0xff
		self.putc(n1)
		self.putc(n2)
		self.putc(n3)
		self.putc(n4)
	end

	def geti()
		n1 = self.getc
		n2 = self.getc
		n3 = self.getc
		n4 = self.getc
		n = (n1 << 24) + (n2 << 16) + (n3 << 8) + n4
		return n
	end

	def putb(n)
		self.putc(n)
	end

	def getb()
		n = self.getc
		return n
	end

	def puts(s)
		self.puti(s.length)
		self.write(s)
	end

	def gets()
		length = self.geti()
		s = self.read(length)
		return s
	end
end

class TTTProto
	::SND_MOVE = 0
	::REQ_SYNC = 1
	::MSG_SEAT = 0
	::MSG_PLAYERS = 1
	::MSG_MOVE = 2
	::MSG_GAMEOVER = 3
	::REQ_MOVE = 4
	::RSP_MOVE = 5
	::SND_SYNC = 6

	::MOVE_OK = 0
	::MOVE_ERR_STATE = -1
	::MOVE_ERR_TURN = -2
	::MOVE_ERR_BOUND = -3
	::MOVE_ERR_FULL = -4

	def send_seat(p)
		puts "   * Send: seat number to seat " + p.to_s
		puts "     Seat: " + p.to_s
		s = GGZRawSocket.for_fd($server.get_seat_fd(p))
		s.puti(::MSG_SEAT)
		s.puti(p)
	end

	def send_players(p)
		puts "   * Send: playerlist to seat " + p.to_s
		s = GGZRawSocket.for_fd($server.get_seat_fd(p))
		s.puti(::MSG_PLAYERS)
		num = $server.get_num_seats
		for i in 0..num - 1
			type = $server.get_seat_type(i)
			puts "     Type at seat " + i.to_s + ": " + type.to_s
			s.puti(type)
			if type != GGZDMod::SEATOPEN then
				name = $server.get_seat_name(i)
				puts "     Name at seat " + i.to_s + ": " + name.to_s
				s.puts(name)
			end
		end
	end

	def broadcast_players
		puts "#  Broadcast player list."
		num = $server.get_num_seats
		for i in 0..num - 1
			fd = $server.get_seat_fd(i)
			if fd != -1 then
				send_players(i)
			end
		end
	end

	def send_movestatus(p, status)
		puts "   * Send: move status to seat " + p.to_s
		puts "     Status: " + status.to_s
		fd = $server.get_seat_fd(p)
		if fd != -1 then
			s = GGZRawSocket.for_fd($server.get_seat_fd(p))
			s.puti(::RSP_MOVE)
			s.putb(status)
		end
	end

	def send_move(p, move)
		puts "   * Send: move information to seat " + p.to_s
		puts "     Move: " + move.to_s
		s = GGZRawSocket.for_fd($server.get_seat_fd(p))
		s.puti(::MSG_MOVE)
		s.puti(1 - p)
		s.puti(move)
	end

	def send_gameover(p, winner)
		puts "   * Send: gameover to seat " + p.to_s
		puts "     Winner: " + winner.to_s
		s = GGZRawSocket.for_fd($server.get_seat_fd(p))
		s.puti(::MSG_GAMEOVER)
		s.putb(winner)
	end

	def broadcast_gameover(winner)
		puts "#  Broadcast gameover event."
		num = $server.get_num_seats
		for i in 0..num - 1
			fd = $server.get_seat_fd(i)
			if fd != -1 then
				send_gameover(i, winner)
			end
		end
	end

	def request_move(p)
		puts "   * Send: move request to seat " + p.to_s
		s = GGZRawSocket.for_fd($server.get_seat_fd(p))
		s.puti(::REQ_MOVE)
	end

	def event(id, data)
		if id == GGZDMod::EVENTSTATE then
			puts "#  The game state was changed."
		elsif id == GGZDMod::EVENTJOIN then
			num = data
			puts "#  A player joined the game."
			puts "#   Seat: " + num.to_s
			puts "#   Name: " + $server.get_seat_name(num)
		elsif id == GGZDMod::EVENTLEAVE then
			puts "#  A player left the game."
		elsif id == GGZDMod::EVENTSEAT then
			puts "#  A player changed the seat."
		elsif id == GGZDMod::EVENTDATA then
			num = data
			puts "#  A player sent data."
			puts "#   Seat: " + num.to_s
		elsif id == GGZDMod::EVENTERROR then
			puts "#  An error occurred."
		else
			puts "#  The event is unknown. Error."
		end
	end
end

