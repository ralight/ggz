#!/usr/bin/env python
#
# Metacle - Aggregation of metaservers
# Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

class MetaProtocol:
	def __init__(self):
		self.services = ["liquidwar"]

	def load(self, url):
		list = []

		try:
			f = open(url)
		except:
			print "Error: Could not load resource %s." % url
			return None

		lines = f.readlines()
		f.close()

		for line in lines:
			line = line.strip()
			fields = line.split(",")
			if len(fields) != 9:
				continue

			#host,port,game,version,uptime,busyplayers,maxplayers,password,comment
			gameserver = {}
			gameserver["gamename"] = "liquidwar"
			gameserver["uri"] = fields[0] + ":" + fields[1]
			# FIXME: game?
			gameserver["version"] = fields[3]
			#gameserver["state"]
			gameserver["topic"] = fields[8]
			gameserver["players"] = fields[6]
			try:
				available = int(fields[6]) - int(fields[5])
			except:
				available = 0
			gameserver["available"] = str(available)

			list.append(gameserver)

		return list

