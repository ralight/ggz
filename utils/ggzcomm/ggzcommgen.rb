#!/usr/bin/ruby
#
# Ruby implementation of the GGZComm source generator
# Copyright (C) 2002 Josef Spillner, dr_maux@users.sourceforge.net
# Published under GNU GPL conditions

# Global variables
# scalar:engine
# scalar:version
# scalar:protocol
# scalar:event
# list:eventlist
# list:evallist

# Use xml parsing library
require 'xmlparser'

# Storage class
class GGZCommEvent
	def initialize(name)
		@datalist = Array.new
		#@tmplist = Array.new
		@name = name
		@eval = 0
	end
	def add(var, type)
		@datalist.push [var, type]
	end
	def sequence(count)
		@datalist.push ["%SEQ", count]
	end
	def endsequence
		@datalist.push "%ENDSEQ"
	end
	def evaluate
		@eval = 1
	end
	def name
		return @name
	end
	def decl
		x = ""
		seq = 0
		@datalist.each do |data, type|
			case data
				when "%SEQ"
					seq = 1
					count = type
				when "%ENDSEQ"
					seq = 0
					count = 0
				else
					if x.length > 0 then
						x = x + ", "
					end
					case type
						when "string"
							type = "char*"
						when "byte"
							type = "char"
					end
					x = x + type
					if seq == 1 then
						x = x + "*"
					end
					x = x + " " + data
			end
		end
		return x
	end
end

# The class definition
class GGZComm

	# Initialize all non-scalar values
	def initialize
		@eventlist = Array.new
		@evallist = Array.new
	end

	# Load an XML file and setup all internal structures
	def load(file)
		@protocol = file
		section = "toplevel"
		parser = XMLParser.new
		f = File.new(file + ".protocol")
		# cut off xml header
		f.gets
		while input = f.read
			puts "Input: " + input
			parser.parse input do |type, name, data|
				#if !name then return end
				#puts "=> type = " + type.to_s + ", name = " + name.to_s + ", data = " + data.to_s
				if name then tag = name.downcase end
				case type
					when XMLParser::START_ELEM
						case tag
							when "ggzcomm"
								if section == "toplevel"
									data.each do |key, value|
										puts key + "=" + value
										case key
											when "engine"
												@engine = value
											when "version"
												@version = value
											else
												error "Unknown attribute '" + key + "' for ggzcomm, only 'version' and 'engine' are allowed."
										end
									end
								else
									error "The 'ggzcomm' tag may only appear as top-level tag."
								end
							when "protocol"
								section = "protocol"
							when "server"
								section = "server"
							when "client"
								section = "client"
							when "event"
								if section == "client" || section == "server"
									data.each do |key, value|
										puts "event:" + key + "=" + value
										case key
											when "name"
												@event = GGZCommEvent.new(value)
												@eventlist.push @event
											else
												error "Events must not carry any attributes except 'name'."
										end
									end
								else
									error "The 'event' tag is only valid in 'server' or 'client'."
								end
							when "sequence"
								xcount = 0
								data.each do |key, value|
									puts "sequence:" + key + "=" + value
									case key
										when "count"
											xcount = value
										else
											error "No attribute other than 'count' can be given for 'sequence'"
									end
								end
								if xcount
									@event.sequence(xcount)
								else
									error "Tag 'sequence' needs attribute 'count'."
								end
							when "eval"
								@event.evaluate
								@evallist.push "%EVAL"
							when "condition"
								xname = 0
								xresult = 0
								data.each do |key, value|
									puts "eval:" + key + "=" + value
									case key
										when "name"
											xname = value
										when "result"
											xresult = value
										else
											error "Attribute for 'condition' must be 'name' or 'result'"
									end
								end
								if xname && xresult
									@evallist.push [xname, xresult]
								else
									error "Tag 'condition' requires both 'name' and 'result' as attribute."
								end
							when "def"
								data.each do |key, value|
									puts "def:" + key + "=" + value
									case key
										when "name"
											xname = value
										when "value"
											xvalue = value
										else
											error "Don't know anything about a '" + key + "' for the 'def' tag."
									end
								end
							when "link"
								data.each do |key, value|
									puts "link:" + key + "=" + value
									case key
										when "client"
											xclient=value
										when "server"
											xserver=value
										when "followup"
											xfollowup=value
										else
											error "No '" + key + "' attribute allowed for the 'link' tag."
									end
								end
							when "data"
								if section == "client" || section == "server"
									xname=0
									xtype=0
									data.each do |key, value|
										puts "data:" + key + "=" + value
										case key
											when "name"
												xname=value
											when "type"
												xtype=value
											else
												error "Only 'name' and 'type' may be present in the 'data' tag."
										end
									end
									if xname && xtype
										@event.add(xname, xtype)
									else
										error "Missing one of the attributes 'name' or 'type' for the 'data' tag."
									end
								else
									error "The 'data' tag may only be used in 'server' or 'client'."
								end
						end
					when XMLParser::END_ELEM
						case tag
							when "sequence"
								@event.endsequence
							when "eval"
								@evallist.reverse_each do |name, result|
									if name == "%SEQ"
										@evallist.pop
										break
									end
									@evallist.pop
								end
						end
					when XMLParser::CDATA
						#puts "cdata"
				end
			end
		end
	end

	# Output all internal structures in a certain format
	def output(format)
		case format
			when "c"
				f = File.new(@protocol + "_generated.h", "w")
				f.puts "/* Generated by GGZComm/ruby */"
				f.puts "/* Protocol '" + @engine + "', version '" + @version + "' */"
				f.puts ""
				proto = @protocol.upcase
				f.puts "#ifndef GGZCOMM_" + proto + "_H"
				f.puts "#define GGZCOMM_" + proto + "_H"
				f.puts ""
				@eventlist.each do |event|
					f.print "void ggzcomm_" + event.name + "("
					f.print event.decl
					f.puts ");"
				end
				f.puts ""
				f.puts "#endif"
				f.puts ""
				f.close

				f = File.new(@protocol + "_generated.c", "w")
				f.puts "/* Generated by GGZComm/ruby */"
				f.puts "/* Protocol '" + @engine + "', version '" + @version + "' */"
				f.puts ""
				f.puts "#include \"" + @protocol + "_generated.h" + "\""
				f.puts "#include <stdlib.h>"
				f.puts ""
				@eventlist.each do |event|
					f.print "void ggzcomm_" + event.name + "("
					f.print event.decl
					f.puts ")"
					f.puts "{"
					f.puts "}"
					f.puts ""
				end
				f.puts "void ggzcomm_error()"
				f.puts "{"
				f.puts "\texit(-1);"
				f.puts "}"
				f.puts ""
				f.close
			when "c++"
			else
				error "Unknown output format '" + format + "', only 'c' and 'c++' are allowed"
		end
	end

	# Bail out with an error message
	def error(errormessage)
		puts "Error!"
		puts errormessage
		exit -1
	end
end

# Test application
comm = GGZComm.new
comm.load("tictactoe")
comm.output("c")

