#!/usr/local/bin/ruby

# Guru module: private messages among players
# Copyright (C) 2001 Josef Spillner, dr_maux@user.sourceforge.net
# Published under GNU GPL conditions

# Commands:
# guru do i have any messages
# guru tell grubby nice to meet myself :)
# guru alert grubby

databasedir = ENV['HOME'] + "/.ggz/grubby"

####################################################################################

class GuruMessages
  def initialize
    @msg = Array.new
	@alerts = Array.new
  end
  def add(fromplayer, player, message)
    @entry = Array.new
	newmessage = (fromplayer + " said: " + message.join(" ")).split(" ")
    @entry << player << newmessage
    @msg.push(@entry)
	print "OK, I make sure he gets the message."
	$stdout.flush
	sleep 1
  end
  def tell(player)
    len = @msg.length
	a = 0
    for i in 0..len
	  unless @msg[len-i] == nil
	    print @msg[len-i][1][0..@msg[len-i][1].length - 1].join(" ") + "\n" if player == @msg[len-i][0]
		if player == @msg[len-i][0]
  	      @msg.delete_at(len-i)
		  a = 1
		end
	  end
	end
	if a == 0
	  print "Sorry " + player + ", I guess you're not important enough to get any messages."
	end
	$stdout.flush
	sleep 1
  end
  def alert(fromplayer, player)
    @entry = Array.new << fromplayer << player
    @alerts.push(@entry)
	print "OK, I alert " + player + " when I see him."
	$stdout.flush
	sleep 1
  end
  def trigger(player)
    len = @alerts.length
	a = 0
    for i in 0..len
	  unless @alerts[len-i] == nil
  	    if player == @alerts[len-i][0]
	      print player + ": ALERT from " + @alerts[len-i][1] + "\n"
	      @alerts.delete_at(len-i)
		  a = 1
		end
	  end
	end
	if a == 1
	  $stdout.flush
	  sleep 1
	  return 1
	end
	return 0
  end
end

input = $stdin.gets.chomp.split(/\ /)

mode = 0
if (input[1] == "do") && (input[2] == "i") && (input[3] == "have") &&
  (input[4] == "any") && (input[5] == "messages")
  mode = 1
  player = ARGV[0]
end
if (input[1] == "tell")
  mode = 2
  fromplayer = ARGV[0]
  player = input[2]
  message = input[3..input.length]
end
if(input[1] == "alert")
  mode = 3
  fromplayer = ARGV[0]
  player = input[2]
end

m = nil
begin
  File.open(databasedir + "/messages") do |f|
    m = Marshal.load(f)
  end
rescue
  m = GuruMessages.new
end

if mode == 0
  ret = m.trigger ARGV[0]
  if ret == 0
    exit
  end
end
if mode == 1
  if player != nil
    m.tell player
  else
    print "If you mind telling me who you are?"
    $stdout.flush
	sleep 1
  end
end
if mode == 2
  m.add fromplayer, player, message
end
if mode == 3
  m.alert fromplayer, player
end

File.open(databasedir + "/messages", "w+") do |f|
  Marshal.dump(m, f)
end

