#!/usr/local/bin/ruby

# Guru module: blackboard to remember meetings
# Copyright (C) 2001 Josef Spillner, dr_maux@user.sourceforge.net
# Published under GNU GPL conditions

# Commands:
# guru meeting								-> list all meetings
# guru meeting add 2001-12-24 xmas			-> add whatever text (no special format)
# guru meeting remove 2001-12-24 xmas		-> remove that entry again

databasedir = "/tmp"

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
	print "Planned meetings: ", @notes.join " - "
	print "\n"
  end
end

if (ARGV[1] != "meeting")
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

if ARGV[2] == "add"
  add = ARGV[3..ARGV.length]
  date = add.join " "
  m.add date
  dump = 1
end
if ARGV[2] == "remove"
  rm = ARGV[3..ARGV.length]
  date = rm.join " "
  m.remove date
  dump = 1
end

if ARGV[2] == nil
  m.tell
end

if dump == 1
  File.open(databasedir + "/meetings", "w+") do |f|
    Marshal.dump(m, f)
  end
end

