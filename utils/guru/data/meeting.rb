#!/usr/local/bin/ruby
# Guru module: blackboard to remember meetings

# guru meeting
# guru meeting add blah foo 2001-12-24 xmas bashing

database = "/tmp/gurumeeting"

class GuruMeeting
  def initialize
    @notes = Array.new
  end
  def add(date)
    @notes.push(date)
  end
  def remove
    puts "not implemented yet"
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
  File.open(database) do |f|
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

if ARGV[2] == nil
  m.tell
end

if dump == 1
  File.open(database, "w+") do |f|
    Marshal.dump(m, f)
  end
end

