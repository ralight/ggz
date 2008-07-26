#!/usr/bin/env python
#
# Metacle - Aggregation of metaservers
# Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import xml.dom.minidom
import commonxml

class MetaProtocol:
	def __init__(self):
		self.services = ["worldforge"]

	def load(self, content):
		list = []

		try:
			dom = xml.dom.minidom.parseString(content)
		except:
			print "Error: Could not parse content %s." % content
			return None

		metaquery = dom.documentElement

		# FIXME: status attribute handling needed! (but is child element too)

		servers = commonxml.find(metaquery, "server")
		for server in servers:
			address = commonxml.findone_value(server, "address")
			status = commonxml.findone_value(server, "status")
			name = commonxml.findone_value(server, "name")
			servertype = commonxml.findone_value(server, "servertype")
			ruleset = commonxml.findone_value(server, "ruleset")
			uptime = commonxml.findone_value(server, "uptime")
			ping = commonxml.findone_value(server, "ping")
			clients = commonxml.findone_value(server, "clients")
			builddate = commonxml.findone_value(server, "builddate")
			version = commonxml.findone_value(server, "version")

			if status != "valid":
				continue

			gameserver = {}
			gameserver["gamename"] = "worldforge" # use servertype? ruleset?
			gameserver["uri"] = address
			gameserver["version"] = version
			gameserver["topic"] = "Ruleset: " + ruleset + "; Uptime: " + str(uptime)
			gameserver["players"] = clients

			list.append(gameserver)

		return list

