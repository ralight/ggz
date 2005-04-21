#!/usr/bin/ruby
#
# Ruby implementation of the GGZComm source generator
# Copyright (C) 2002 - 2005 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

# Version number
$version = 0.2

# Use xml parsing library
require 'xmlparser'

# Storage class for a single <event> and its contents
# Contains a tree of data definitions, sequences and conditions
class GGZCommEvent
	def initialize(name, section)
		@datalist = Array.new
		@evallist = nil
		@name = name
		@section = section
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
		@datalist.push ["%EVAL", [], 0]
	end

	def endevaluate
		@datalist.push "%ENDEVAL"
	end

	def setevallist(list)
		@evallist = list
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
				when "%EVAL"
					#seq += 1
				when "%ENDEVAL"
					#seq -= 1
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
		maxseq = 0

		@datalist.each do |data, type, option|
			case data
				when "%SEQ"
					seq += 1
					if seq > maxseq then maxseq = seq end
				when "%ENDSEQ"
					seq -= 1
			end
		end
		seq = 0

		for i in (1..maxseq)
			x += "\tint i" + i.to_s + ";\n"
		end
		if maxseq > 0 then
			x += "\n"
		end

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
					vname = "i" + seq.to_s
					x = x + indent + "\tif(" + count.to_s + " > " + maxcount.to_s + ") ggzcomm_error();\n"
					x = x + indent + "\tfor(" + vname + " = 0; " + vname + " < " + count.to_s + "; " + vname + "++)\n"
					x = x + indent + "\t{\n"
					index = "[" + vname + "]"
				when "%ENDSEQ"
					seq -= 0
					count = 0
					x = x + indent + "}\n"
					index = ""
				when "%EVAL"
					seq += 1
					x = x + indent + "\tif("
					i = 0
					@evallist.each do |name, result, type|
						if type == "equal"
							type = "=="
						elsif type == "unequal"
							type = "!="
						elsif type == "smaller"
							type = "<"
						elsif type == "larger"
							type = ">"
						end
						if i > 0 then
							x = x + " || "
						end
						x = x + "(variables." + name + index + " " + type + " " + result + ")"
						i += 1
					end
					x = x + ")\n"
					x = x + indent + "\t{\n"
				when "%ENDEVAL"
					seq -= 1
					x = x + indent + "}\n"
				else
					myvars[data] = 1
					esname = type
					case type
						when "string"
							type = "char*"
							if @section == role then
								esname = "string"
							else
								esname = "string_alloc"
							end
						when "byte"
							type = "char"
							esname = "char"
					end
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

	def network_cpp(role)
		x = ""
		seq = 0

		data = @name
		if @section == role then
			x += "\t*socket << " + data + ";\n"
			x += "\tif(ret < 0) emit signalError();"
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
					end
					vname = "i" + seq.to_s
					x = x + indent + "\tif(" + count.to_s + " > " + maxcount.to_s + ") emit signalError();\n"
					x = x + indent + "\tfor(int " + vname + " = 0; " + vname + " < " + count.to_s + "; " + vname + "++)\n"
					x = x + indent + "\t{\n"
					index = "[" + vname + "]"
				when "%ENDSEQ"
					seq -= 0
					count = 0
					x = x + indent + "}\n"
					index = ""
				when "%EVAL"
					seq += 1
					x = x + indent + "\tif("
					i = 0
					@evallist.each do |name, result, type|
						if type == "equal"
							type = "=="
						elsif type == "unequal"
							type = "!="
						elsif type == "smaller"
							type = "<"
						elsif type == "larger"
							type = ">"
						end
						if i > 0 then
							x = x + " || "
						end
						x = x + "(" + name + index + " " + type + " " + result + ")"
						i += 1
					end
					x = x + ")\n"
					x = x + indent + "\t{\n"
				when "%ENDEVAL"
					seq -= 1
					x = x + indent + "}\n"
				else
					myvars[data] = 1
					if @section == role then
						x += indent + "\t*socket << " + data + index + ";\n"
					else
						x += indent + "\t*socket >> " + data + index + ";\n"
					end
					x += indent + "\tif(ret < 0) emit signalError();\n"
			end
		end

		if @section != role then
			x += "\temit signalNotification(" + name + ");\n"
		end

		return x
	end

	def network_python(role)
		x = ""
		seq = 0

		esname = "int"
		data = @name
		if @section == role then
			x += "\t\tret = ggz_write_" + esname + "(fd, " + data + ")\n"
			x += "\t\tif ret < 0:\n"
			x += "\t\t\traise Exception()\n"
		end

		myvars = Hash.new

		index = ""
		@datalist.each do |data, type, option|
			indent = "\t"
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
						count = "self." + count
					end
					vname = "i" + seq.to_s
					x = x + indent + "\tif " + count.to_s + " > " + maxcount.to_s + ":\n"
					x = x + indent + "\t\traise Exception()\n"
					x = x + indent + "\tfor " + vname + " in range(0, " + count.to_s + "):\n"
					index = "[" + vname + "]"
				when "%ENDSEQ"
					seq -= 0
					count = 0
					index = ""
				when "%EVAL"
					seq += 1
					x = x + indent + "\tif "
					i = 0
					@evallist.each do |name, result, type|
						if type == "equal"
							type = "=="
						elsif type == "unequal"
							type = "!="
						elsif type == "smaller"
							type = "<"
						elsif type == "larger"
							type = ">"
						end
						if i > 0 then
							x = x + " or "
						end
						x = x + "(self." + name + index + " " + type + " " + result + ")"
						i += 1
					end
					x = x + ":\n"
				when "%ENDEVAL"
					seq -= 1
				else
					myvars[data] = 1
					esname = type
					if @section == role then
						x += indent + "\tret = ggz_write_" + esname + "(fd, self." + data + index + ")\n"
					else
						x += indent + "\tret = ggz_read_" + esname + "(fd, self." + data + index + ")\n"
					end
					x += indent + "\tif ret < 0:\n"
					x += indent + "\t\traise Exception()\n"
			end
		end

		if @section != role then
			x += "\t\tif notifier:\n"
			x += "\t\t\tnotifier(" + name + ")\n"
		end

		return x
	end
end

# The class definition for the whole communication protocol
# Contains handlers for all XML tags, the parser and the output
class GGZComm

	# Initialize all non-scalar values
	def initialize
		@eventlist = Array.new
		@constants = Hash.new
		@clientlinks = Hash.new
		@serverlinks = Hash.new
		@verbose = false
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
		@evallist = []
	end

	def handle_event_condition(data)
		xname = 0
		xresult = 0
		xtype = "equal"
		data.each do |key, value|
			if @verbose then
				puts "eval:" + key + "=" + value
			end
			case key
				when "name"
					xname = value
				when "result"
					xresult = value
				when "type"
					xtype = value
				else
					error "Attribute for 'condition' must be 'name' or 'result'"
			end
		end
		if xname && xresult
			if xtype != "equal" and xtype != "smaller" and xtype != "larger" and xtype != "unequal" then
				error "Tag 'condition' does not know operator type '" + xtype + "'."
			end
			@evallist.push [xname, xresult, xtype]
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
								@event.endevaluate
								@event.setevallist(@evallist)
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
				next if name == '%SEQ'
				#if checkvars[name] and checkvars[name] != type then
				if checkvars[name] then
					error "Variable of name '" + name + "' is already in use."
				end
				checkvars[name] = type
			end
		end

		basename = @protocol + "_" + role

		if format == "c" and library == "easysock" then
			f = File.new(basename + ".h", "w")
			f.puts "/* Generated by GGZComm/ruby version " + $version.to_s + " */"
			f.puts "/* Protocol '" + @engine + "', version '" + @version + "' */"
			f.puts ""
			proto = @protocol.upcase
			f.puts "#ifndef GGZCOMM_" + proto + "_H"
			f.puts "#define GGZCOMM_" + proto + "_H"
			f.puts ""
			@eventlist.each do |event|
				f.puts "#define " + event.name + " " + @constants[event.name]
				@constants[event.name] = nil
			end

			i = 0
			@eventlist.each do |event|
				i += event.decl(true).length
			end
			if i > 0
				f.puts ""
				f.puts "struct variables_t"
				f.puts "{"
				@eventlist.each do |event|
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

			f = File.new(basename + ".c", "w")
			f.puts "/* Generated by GGZComm/ruby version " + $version.to_s + " */"
			f.puts "/* Protocol '" + @engine + "', version '" + @version + "' */"
			f.puts ""
			f.puts "#include \"" + basename + ".h" + "\""
			f.puts "#include <stdlib.h>"
			f.puts "#include <ggz.h>"
			if @constants.length > 0 then
				f.puts ""
				@constants.each do |name, value|
					next if not value
					f.puts "#define " + name + " " + value
				end
			end
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
			f.puts "\tggz_read_int(fd, &opcode);"
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
			f = File.new(basename + ".h", "w")
			f.puts "/* Generated by GGZComm/ruby version " + $version.to_s + " */"
			f.puts "/* Protocol '" + @engine + "', version '" + @version + "' */"
			f.puts ""
			proto = @protocol.upcase
			f.puts "#ifndef GGZCOMM_" + proto + "_H"
			f.puts "#define GGZCOMM_" + proto + "_H"
			f.puts ""
			f.puts "#include <qobject.h>"
			f.puts ""
			f.puts "class QDataStream;"
			f.puts ""
			f.puts "class " + @engine + " : QObject"
			f.puts "{"
			f.puts "\tpublic:"

			f.puts "\tenum Opcodes"
			f.puts "\t{"
			i = 0
			@eventlist.each do |event|
				if i > 0 then
					f.puts ","
				end
				f.print "\t\t" + event.name + " = " + @constants[event.name]
				@constants[event.name] = nil
				i += 1
			end
			f.puts ""
			f.puts "\t};"

			i = 0
			@eventlist.each do |event|
				i += event.decl(true).length
			end
			if i > 0
				f.puts ""
				@eventlist.each do |event|
					if event.decl(true) != "" then
						f.puts event.decl(true);
					end
				end
			end

			f.puts ""
			f.puts "\t" + @engine + "();"
			f.puts ""
			f.puts "\tvoid ggzcomm_network_main();"
			f.puts ""
			@eventlist.each do |event|
				if event.section != role then
					f.puts "\tvoid ggzcomm_" + event.name + "();"
				end
			end
			f.puts ""
			f.puts "\tvoid ggzcomm_set_fd(int usefd);"
			f.puts ""
			f.puts "\tsignals:"
			f.puts "\tvoid signalNotification(int message);"
			f.puts "\tvoid signalError();"
			f.puts ""
			f.puts "\tprivate:"
			@eventlist.each do |event|
				if event.section == role then
					f.puts "\tvoid ggzcomm_" + event.name + "();"
				end
			end
			f.puts ""
			f.puts "\tint fd;"
			f.puts "\tint ret;"
			f.puts "\tint requirelink;"
			f.puts "\tint nextlink;"
			f.puts "\tQDataStream *socket;"
			f.puts "};"
			f.puts ""
			f.puts "#endif"
			f.puts ""
			f.close

			f = File.new(basename + ".cpp", "w")
			f.puts "/* Generated by GGZComm/ruby version " + $version.to_s + " */"
			f.puts "/* Protocol '" + @engine + "', version '" + @version + "' */"
			f.puts ""
			f.puts "#include \"" + basename + ".h" + "\""
			f.puts "#include <qsocketdevice.h>"
			if @constants.length > 0 then
				f.puts ""
				@constants.each do |name, value|
					next if not value
					f.puts "#define " + name + " " + value
				end
			end
			f.puts ""
			f.puts @engine + "::" + @engine + "()"
			f.puts ": QObject()"
			f.puts "{"
			f.puts "\trequirelink = 0;"
			f.puts "\tfd = -1;"
			f.puts "\tsocket = NULL;"
			f.puts "}"
			f.puts ""
			@eventlist.each do |event|
				#if event.section != role then
				#	f.print "static "
				#end
				f.puts "void " + @engine + "::ggzcomm_" + event.name + "(void)"
				f.puts "{"
				f.puts event.network_cpp(role)
				if links[event.name] then
					f.puts ""
					f.puts "\t\trequirelink = 1;"
					f.puts "\t\tnextlink = " + links[event.name] + ";"
				end
				f.puts "}"
				f.puts ""
			end
			f.puts "void " + @engine + "::ggzcomm_network_main()"
			f.puts "{"
			f.puts "\tint opcode;"
			f.puts "\t*socket >> opcode;"
			f.puts ""
			f.puts "\tif(requirelink)"
			f.puts "\t{"
			f.puts "\t\tif(opcode != nextlink) emit signalError();"
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
			f.puts "void " + @engine + "::ggzcomm_set_fd(int usefd)"
			f.puts "{"
			f.puts "\tfd = usefd;"
			f.puts "\tQSocketDevice *sock = new QSocketDevice(fd, QSocketDevice::Stream);"
			f.puts "\tsocket = new QDataStream(sock);"
			f.puts "}"
			f.puts ""
			f.close
		elsif format == "python" and library == "socket"
			#error "Currently unsupported"
			f = File.new(basename + ".py", "w")
			f.puts "### Generated by GGZComm/ruby version " + $version.to_s
			f.puts "### Protocol '" + @engine + "', version '" + @version + "'"
			f.puts ""
			f.puts "class " + @engine + ":"
			if @constants.length > 0 then
				f.puts ""
				@constants.each do |name, value|
					next if not value
					f.puts "\t" + name + " = " + value
				end
			end
			f.puts ""
			f.puts "\tnotifier = None"
			f.puts "\tfd = -1"
			f.puts "\tret = 0"
			f.puts "\trequirelink = 0"
			f.puts "\tnextlink = 0"
			f.puts ""
			@eventlist.each do |event|
				if event.section != role then
					#f.print "static "
				end
				f.puts "\tdef ggzcomm_" + event.name + "():"
				f.puts event.network_python(role)
				if links[event.name] then
					f.puts ""
					f.puts "\t\trequirelink = 1"
					f.puts "\t\tnextlink = " + links[event.name]
				end
				f.puts ""
			end
			f.puts "\tdef ggzcomm_network_main():"
			f.puts "\t\topcode = 0"
			f.puts "\t\t#ggz_read_int(&opcode)"
			f.puts ""
			f.puts "\t\tif(requirelink):"
			f.puts "\t\t\tif opcode != nextlink:"
			f.puts "\t\t\t\traise Exception()"
			f.puts "\t\t\trequirelink = 0"
			f.puts ""
			@eventlist.each do |event|
				if event.section != role
					f.puts "\t\tif opcode == " + event.name + ":\n"
					f.puts "\t\t\tggzcomm_" + event.name() + "()\n"
				end
			end
			f.puts ""
			f.puts "\tdef ggzcomm_set_notifier_callback(cb):"
			f.puts "\t\tnotifier = cb"
			f.puts ""
			f.puts "\tdef ggzcomm_set_fd(usefd):"
			f.puts "\t\tfd = usefd"
			f.puts ""
			f.close
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

file = nil
language = nil
interface = nil
role = nil

for index in (0..ARGV.length - 1)
	arg = ARGV[index]
	case arg
		when "-h", "--help":
			puts "GGZ Communication Protocol Generator (ggzcommgen)"
			puts "Call: ggzcommgen.rb [options] <filename> (for filename.protocol)"
			puts "[-l | --language ] The language to use: c, c++, python"
			puts "[-i | --interface] The networking library: ggz, qt, mnet, socket"
			puts "[-r | --role     ] The usage role: client, server"
			puts ""
			puts "[-h | --help     ] This help screen"
			puts "[-v | --version  ] Display version number"
			exit
		when "-r", "--role":
			index += 1
			arg = ARGV[index]
			role = arg
		when "-l", "--language":
			index += 1
			arg = ARGV[index]
			language = arg
		when "-i", "--interface":
			index += 1
			arg = ARGV[index]
			interface = arg
		when "-v", "--version":
			puts $version
			exit
		else
			if arg =~ /^-/
				puts "Unknown option: " + arg
				exit -1
			else
				file = arg
			end
	end
end
if not file
	puts "No file given."
	exit -1
end
if not interface
	puts "Need to specify a library interface."
	exit -1
end
if not language
	puts "Need to specify a language."
	exit -1
end
if not role
	puts "Need to specify a role."
	exit -1
end

# Test application
comm = GGZComm.new
comm.load(file)
comm.output(language, interface, role)

