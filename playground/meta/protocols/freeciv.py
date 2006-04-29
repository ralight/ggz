#!/usr/bin/env python
#
# Metacle - Aggregation of metaservers
# Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import xml.dom.minidom
import commonxml

class MetaProtocol:
	def __init__(self):
		self.services = ["freeciv"]

	def load(self, url):
		list = []

		try:
			dom = xml.dom.minidom.parse(url)
		except:
			print "Error: Could not load resource %s." % url
			return None

		rss = dom.documentElement
		#print "RSS", rss

		channels = commonxml.find(rss, "channel")
		channel = channels[0]
		#print "CHANNEL", channel

		items = commonxml.find(channel, "item")
		for item in items:
			descriptions = commonxml.find(item, "description")
			description = descriptions[0]

			gameserver = {}
			gameserver["gamename"] = "freeciv"

			tmp_host = None
			tmp_port = None
			tmp_version = None
			tmp_patches = None

			textparts = description.childNodes
			for textpart in textparts:
				if textpart.nodeType == textpart.TEXT_NODE:
					text = textpart.nodeValue
					item = text
					item = item.strip()
					#print "-item-", item

					entry = item.split(":")
					if len(entry) < 2:
						continue
					keyword = entry[0].strip()
					value = ":".join(entry[1:]).strip()

					#print "KEY", keyword, value
					if keyword == "Host":
						tmp_host = value
					elif keyword == "Port":
						tmp_port = value
					elif keyword == "Version":
						tmp_version = value
					elif keyword == "Patches":
						if value != "none":
							tmp_patches = value
					elif keyword == "State":
						gameserver["state"] = value
					elif keyword == "Topic":
						gameserver["topic"] = value
					elif keyword == "Players":
						gameserver["players"] = value
					elif keyword == "Available":
						gameserver["available"] = value
					elif keyword == "Serverid":
						pass
					else:
						print "???", keyword

			if tmp_host and tmp_port:
				gameserver["uri"] = "freeciv://" + tmp_host + ":" + tmp_port

			if tmp_version:
				if tmp_patches:
					tmp_version += " (" + tmp_patches + ")"
				gameserver["version"] = tmp_version

			# FIXME state:
			# Pregame, Running, Game Ended, Unknown

			# FIXME version/patches needed separately?

			#print "Gameserver", gameserver
			list.append(gameserver)

		return list

