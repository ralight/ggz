#!/usr/bin/ruby
#
# Ruby implementation of the GGZComm source generator
# Copyright (C) 2002 - 2005 Josef Spillner <josef@ggzgamingzone.org>
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
	def initialize(name, section)
		@datalist = Array.new
		#@tmplist = Array.new
		@name = name
		@section = section
		@eval = 0
	end

	def add(var, type)
		@datalist.push [var, type, 0]
	end

	def sequence(count, maxcount)
		@datalist.push ["%SEQ", count, maxcount]
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

	def section
		return @section
	end

	def datalist
		return @datalist
	end

	def decl(definition)
		x = ""
		seq = 0
		@datalist.each do |data, type, option|
			case data
				when "%SEQ"
					seq += 1
					count = type
				when "%ENDSEQ"
					seq -= 1
					count = 0
				else
					if x.length > 0 then
						if not definition then
							x = x + ", "
						end
					end
					case type
						when "string"
							type = "char*"
						when "byte"
							type = "char"
					end
					if definition then
						x = x + "\t"
					end
					x = x + type
					for i in (1..seq)
						x = x + "*"
					end
					if not definition then
						x = x + " " + data
					else
						x = x + " " + data + ";\n"
					end
			end
		end
		if x == "" and not definition then
			x = "void"
		end
		return x
	end

	def network(role)
		x = ""
		seq = 0

#		if @section != role then
#			x += decl(true);
#			if @datalist.length > 0 then
#				x += "\n"
#			end
#		end

		esname = "int"
		data = @name
		if @section == role then
			x += "\tret = ggz_write_" + esname + "(fd, " + data + ");\n"
			x += "\tif(ret < 0) ggzcomm_error();\n"
		end

		myvars = Hash.new

		index = ""
		@datalist.each do |data, type, option|
			indent = ""
			for i in (1..seq)
				indent += "\t"
			end
			case data
				when "%SEQ"
					seq += 1
					count = type
					maxcount = option
					if myvars[count] then
						if not maxcount then
							error "Sequence using variable '" + count + "' needs maxcount argument."
						end
						count = "variables." + count
					end
					x = x + indent + "\tif(" + count.to_s + " > " + maxcount.to_s + ") ggzcomm_error();\n"
					x = x + indent + "\tfor(int i = 0; i < " + count.to_s + "; i++)\n"
					x = x + indent + "\t{\n"
					index = "[i]"
				when "%ENDSEQ"
					seq -= 0
					count = 0
					x = x + indent + "}\n"
					index = ""
				else
					myvars[data] = 1
					esname = type
					case type
						when "string"
							type = "char*"
							esname = "string_alloc"
						when "byte"
							type = "char"
							esname = "char"
					end
					#x = x + type
					#if seq == 1 then
					#	x = x + "*"
					#end
					#x = x + " " + data
					####x += type + " x;";
					if @section == role then
						x += indent + "\tret = ggz_write_" + esname + "(fd, variables." + data + index + ");\n"
					else
						x += indent + "\tret = ggz_read_" + esname + "(fd, &variables." + data + index + ");\n"
					end
					x += indent + "\tif(ret < 0) ggzcomm_error();\n"
			end
		end

		if @section != role then
			x += "\tif(notifier_func) (notifier_func)(" + name + ");\n"
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
		@constants = Hash.new
		@clientlinks = Hash.new
		@serverlinks = Hash.new
		@verbose = false
		#@verbose = true
	end

	def handle_toplevel(data)
		data.each do |key, value|
			if @verbose then
				puts key + "=" + value
			end
			case key
				when "engine"
					@engine = value
				when "version"
					@version = value
				else
					error "Unknown attribute '" + key + "' for ggzcomm, only 'version' and 'engine' are allowed."
			end
		end
	end

	def handle_event(data, section)
		data.each do |key, value|
			if @verbose then
				puts "event:" + key + "=" + value
			end
			case key
				when "name"
					@event = GGZCommEvent.new(value, section)
					@eventlist.push @event
				else
					error "Events must not carry any attributes except 'name'."
			end
		end
	end

	def handle_event_sequence(data)
		xcount = 0
		xmaxcount = 0
		data.each do |key, value|
			if @verbose then
				puts "sequence:" + key + "=" + value
			end
			case key
				when "count"
					xcount = value
				when "maxcount"
					xmaxcount = value
				else
					error "No attribute other than 'count' can be given for 'sequence'"
			end
		end
		if xcount
			@event.sequence(xcount, xmaxcount)
		else
			error "Tag 'sequence' needs attribute 'count'."
		end
	end

	def handle_event_eval(data)
		@event.evaluate
		@evallist.push "%EVAL"
	end

	def handle_event_condition(data)
		xname = 0
		xresult = 0
		data.each do |key, value|
			if @verbose then
				puts "eval:" + key + "=" + value
			end
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
	end

	def handle_definitions_def(data)
		xname = nil
		xvalue = nil
		data.each do |key, value|
			if @verbose then
				puts "def:" + key + "=" + value
			end
			case key
				when "name"
					xname = value
				when "value"
					xvalue = value
				else
					error "Don't know anything about a '" + key + "' for the 'def' tag."
			end
		end
		if xname and xvalue then
			@constants[xname] = xvalue
		else
			error "Incorrect definition for '" + xname.to_s + "' = '" + xvalue.to_s + "'."
		end
	end

	def handle_protocol_link(data)
		xclient = nil
		xserver = nil
		xfollowup = nil
		mode = nil
		data.each do |key, value|
			if @verbose then
				puts "link:" + key + "=" + value
			end
			case key
				when "client"
					xclient = value
					if mode == nil then
						mode = "client"
					elsif mode == "server"
						mode = "server-client"
					else
						error "No 'client' attribute allowed here."
					end
				when "server"
					xserver = value
					if mode == nil then
						mode = "server"
					elsif mode == "client"
						mode = "client-server"
					else
						error "No 'server' attribute allowed here."
					end
				when "followup"
					xfollowup = value
				else
					error "No '" + key + "' attribute allowed for the 'link' tag."
			end
		end
		# no followups yet - XXX FIXME!!!
		if (xclient and xserver) then
			if mode == "client-server"
				@clientlinks[xclient] = xserver
				@serverlinks[xserver] = xclient
			elsif mode == "server-client"
				@serverlinks[xserver] = xclient
				@clientlinks[xclient] = xserver
			end
		else
			error "Incorrect link for '" + xclient.to_s + "', '" + xserver.to_s + "'."
		end
	end

	def handle_event_data(data)
		xname = 0
		xtype = 0
		data.each do |key, value|
			if @verbose then
				puts "data:" + key + "=" + value
			end
			case key
				when "name"
					xname = value
				when "type"
					xtype = value
				else
					error "Only 'name' and 'type' may be present in the 'data' tag."
			end
		end
		if xname && xtype
			@event.add(xname, xtype)
		else
			error "Missing one of the attributes 'name' or 'type' for the 'data' tag."
		end
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
			if input == "" then break end
			if @verbose then
				puts "Input: "
				print input
			end
			parser.parse input do |type, name, data|
				#if !name then return end
				#puts "=> type = " + type.to_s + ", name = " + name.to_s + ", data = " + data.to_s
				if name then tag = name.downcase end
				case type
					when XMLParser::START_ELEM
						case tag
							when "ggzcomm"
								if section == "toplevel"
									handle_toplevel(data)
								else
									error "The 'ggzcomm' tag may only appear as top-level tag."
								end
							when "protocol"
								section = "protocol"
							when "server"
								section = "server"
							when "client"
								section = "client"
							when "definitions"
								section = "definitions"
							when "event"
								if section == "client" || section == "server"
									handle_event(data, section)
								else
									error "The 'event' tag is only valid in 'server' or 'client'."
								end
							when "sequence"
								if section == "client" || section == "server"
									handle_event_sequence(data)
								else
									error "The 'sequence' tag is only valid in 'server' or 'client'."
								end
							when "eval"
								if section == "client" || section == "server"
									handle_event_eval(data)
								else
									error "The 'eval' tag is only valid in 'server' or 'client'."
								end
							when "condition"
								if section == "client" || section == "server"
									handle_event_condition(data)
								else
									error "The 'condition' tag is only valid in 'server' or 'client'."
								end
							when "def"
								if section == "definitions"
									handle_definitions_def(data)
								else
									error "The 'def' tag is only valid in 'definitions'."
								end
							when "link"
								if section == "protocol"
									handle_protocol_link(data)
								else
									error "The 'link' tag is only valid in 'protocol'."
								end
							when "data"
								if section == "client" || section == "server"
									handle_event_data(data)
								else
									error "The 'data' tag may only be used in 'server' or 'client'."
								end
							else
								error "Don't know how to process XML tag " + tag + "."
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
	def output(format, library, role)
		@eventlist.each do |event|
			if not @constants[event.name] then
				error "Missing value definition for '" + event.name + "'"
			end
		end

		if role == "client" then
			links = @clientlinks
			revlinks = @serverlinks
		elsif role == "server"
			links = @serverlinks
			revlinks = @clientlinks
		else
			error "Unknown role '" + role + "'"
		end

		checkvars = Hash.new
		@eventlist.each do |event|
			event.datalist.each do |name, type|
				if checkvars[name] then
					error "Variable of name '" + name + "' is already in use."
				end
				checkvars[name] = type
			end
		end

		if format == "c" and library == "easysock" then
			f = File.new(@protocol + "_generated.h", "w")
			f.puts "/* Generated by GGZComm/ruby */"
			f.puts "/* Protocol '" + @engine + "', version '" + @version + "' */"
			f.puts ""
			proto = @protocol.upcase
			f.puts "#ifndef GGZCOMM_" + proto + "_H"
			f.puts "#define GGZCOMM_" + proto + "_H"
			f.puts ""
			@eventlist.each do |event|
				f.puts "#define " + event.name + " " + @constants[event.name]
			end

			i = 0
			@eventlist.each do |event|
				#if event.section != role then
				#	i += event.decl(true).length
				#end
				i += event.decl(true).length
			end
			if i > 0
				f.puts ""
				f.puts "struct variables_t"
				f.puts "{"
				@eventlist.each do |event|
					#if event.section != role then
					#	f.puts event.decl(true);
					#end
					if event.decl(true) != "" then
						f.puts event.decl(true);
					end
				end
				f.puts "};"
				f.puts "struct variables_t variables;"
			end

			f.puts ""
			f.puts "void ggzcomm_network_main(void);"
			f.puts ""
			@eventlist.each do |event|
				if event.section == role then
					f.print "void ggzcomm_" + event.name + "("
					#f.print event.decl(false)
					f.print "void"
					f.puts ");"
				end
			end
			f.puts ""
			f.puts "typedef void (*notifier_func_type)(int opcode);"
			f.puts "typedef void (*error_func_type)(void);"
			f.puts ""
			f.puts "void ggzcomm_set_fd(int usefd);"
			f.puts "void ggzcomm_set_notifier_callback(notifier_func_type f);"
			f.puts "void ggzcomm_set_error_callback(error_func_type f);"
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
			f.puts "static notifier_func_type notifier_func = NULL;"
			f.puts "static error_func_type error_func = NULL;"
			f.puts "static int fd = -1;"
			f.puts "static int ret;"
			f.puts "static int requirelink = 0;"
			f.puts "static int nextlink;"
			f.puts ""
			f.puts "static void ggzcomm_error(void);"
			f.puts ""
			@eventlist.each do |event|
				if event.section != role then
					f.print "static "
				end
				f.print "void ggzcomm_" + event.name + "("
				#if event.section == role then
				#	f.print event.decl(false)
				#else
				#	f.print "void"
				#end
				f.print "void"
				f.puts ")"
				f.puts "{"
				f.puts event.network(role)
				if links[event.name] then
					f.puts ""
					f.puts "\trequirelink = 1;"
					f.puts "\tnextlink = " + links[event.name] + ";"
				end
				f.puts "}"
				f.puts ""
			end
			f.puts "void ggzcomm_network_main(void)"
			f.puts "{"
			f.puts "\tint opcode;"
			f.puts "\tggz_read_int(&opcode);"
			f.puts ""
			f.puts "\tif(requirelink)"
			f.puts "\t{"
			f.puts "\t\tif(opcode != nextlink) ggzcomm_error();"
			f.puts "\t\trequirelink = 0;"
			f.puts "\t}"
			f.puts ""
			f.puts "\tswitch(opcode)"
			f.puts "\t{"
			@eventlist.each do |event|
				if event.section != role
					f.puts "\t\tcase " + event.name + ":\n"
					f.puts "\t\t\tggzcomm_" + event.name() + "();\n"
					f.puts "\t\t\tbreak;"
				end
			end
			f.puts "\t}"
			f.puts "}"
			f.puts ""
			f.puts "void ggzcomm_set_notifier_callback(notifier_func_type f)"
			f.puts "{"
			f.puts "\tnotifier_func = f;"
			f.puts "}"
			f.puts ""
			f.puts "void ggzcomm_set_error_callback(error_func_type f)"
			f.puts "{"
			f.puts "\terror_func = f;"
			f.puts "}"
			f.puts ""
			f.puts "void ggzcomm_set_fd(int usefd)"
			f.puts "{"
			f.puts "\tfd = usefd;"
			f.puts "}"
			f.puts ""
			f.puts "static void ggzcomm_error(void)"
			f.puts "{"
			f.puts "\tif(error_func) (error_func)();"
			f.puts "}"
			f.puts ""
			f.close
		elsif format == "c++" and library == "qt"
			error "Currently unsupported"
		else
			str = "Unknown output format and library combination" + "\n"
			str += "Format was: '" + format + "', only 'c' and 'c++' are allowed" + "\n"
			str += "Library was: '" + library + "', only 'easysock' and 'qt' are allowed"
			error str
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
#comm.load("helloworld")
comm.load("sequence")
#comm.output("c", "easysock", "client")
comm.output("c", "easysock", "server")

