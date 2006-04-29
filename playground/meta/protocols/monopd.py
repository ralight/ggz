#!/usr/bin/env python
#
# Metacle - Aggregation of metaservers
# Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import xml.dom.minidom
import commonxml

class MetaProtocol:
	def __init__(self):
		self.services = ["monopd"]

	def load(self, url):
		list = []

		try:
			dom = xml.dom.minidom.parse(url)
		except:
			print "Error: Could not load resource %s." % url
			return None

		monopigator = dom.documentElement

		servers = commonxml.find(monopigator, "server")
		for server in servers:
			host = server.getAttribute("host")
			port = server.getAttribute("port")
			version = server.getAttribute("version")
			users = server.getAttribute("users")

			gameserver = {}
			gameserver["gamename"] = "monopd"
			gameserver["uri"] = host + ":" + port
			gameserver["version"] = version
			gameserver["players"] = users

			list.append(gameserver)

		return list

