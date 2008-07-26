#!/usr/bin/env python
#
# Metacle - Aggregation of metaservers
# Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import xml.dom.minidom
import commonxml

class MetaProtocol:
	def __init__(self):
		self.services = ["ggz"]

	def load(self, content):
		list = []

		try:
			dom = xml.dom.minidom.parseString(content)
		except:
			print "Error: Could not parse content %s." % content
			return None

		resultset = dom.documentElement

		results = commonxml.find(resultset, "result")
		for result in results:
			classname = result.getAttribute("class")
			uri = result.getAttribute("uri")
			comment = ""
			version = ""

			options = commonxml.find(result, "option")
			for option in options:
				name = option.getAttribute("name")
				value = commonxml.value(option)

				if name == "description":
					comment = value
				if name == "version":
					version = value

			# try to convert name as good as we can!
			classname = classname.lower()
			classname = classname.replace("ggzcards-", "")
			classname = classname.replace("gnu go", "go")
			classname = classname.replace("chinese chess", "cchess")

			gameserver = {}
			gameserver["gamename"] = classname
			gameserver["uri"] = uri
			gameserver["version"] = version
			gameserver["topic"] = comment

			list.append(gameserver)

		return list

