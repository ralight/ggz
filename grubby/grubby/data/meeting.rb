#!/usr/bin/ruby

# Guru module: blackboard to remember meetings
# Copyright (C) 2001, 2002 Josef Spillner, dr_maux@user.sourceforge.net
# Published under GNU GPL conditions

# Commands:
# guru meeting								-> list all meetings
# guru meeting add 2001-12-24 xmas			-> add whatever text (no special format)
# guru meeting remove 2001-12-24 xmas		-> remove that entry again

databasedir = ENV['HOME'] + "/.ggz/grubby"

####################################################################################

class GuruMeeting
  def initialize
    @notes = Array.new
  end
  def add(date)
    @notes.push(date)
  end
  def remove(date)
    @notes.delete(date)
  end
  def tell
	print "Planned meetings: ", @notes.join(" - ")
	#print "\n"
	$stdout.flush
	sleep 1
  end
end

input = $stdin.gets.chomp.split(/\ /)

if (input[1] != "meeting")
  exit
end

m = nil
begin
  File.open(databasedir + "/meetings") do |f|
    m = Marshal.load(f)
  end
rescue
  m = GuruMeeting.new
end

dump = 0

if input[2] == "add"
  add = input[3..input.length]
  date = add.join " "
  m.add date
  dump = 1
end
if input[2] == "remove"
  rm = input[3..input.length]
  date = rm.join " "
  m.remove date
  dump = 1
end

if input[2] == nil
  m.tell
end

if dump == 1
  File.open(databasedir + "/meetings", "w+") do |f|
    Marshal.dump(m, f)
  end
end

