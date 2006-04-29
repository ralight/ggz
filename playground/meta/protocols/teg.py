#!/usr/bin/env python
#
# MetAggregator - Aggregation of metaservers
# Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import xml.dom.minidom
import commonxml

class MetaProtocol:
	def __init__(self):
		self.services = ["teg"]

	def load(self, url):
		list = []

		try:
			dom = xml.dom.minidom.parse(url)
		except:
			print "Error: Could not load resource %s." % url
			return None

		teg = dom.documentElement

		# FIXME: version attribute handling needed?

		entries = commonxml.find(teg, "entry")
		for entry in entries:
			name = commonxml.findone_value(entry, "name")
			port = commonxml.findone_value(entry, "port")
			version = commonxml.findone_value(entry, "version")
			comment = commonxml.findone_value(entry, "comment")

			gameserver = {}
			gameserver["gamename"] = "teg"
			gameserver["uri"] = name + ":" + port
			gameserver["version"] = version
			gameserver["topic"] = comment

			list.append(gameserver)

		return list

