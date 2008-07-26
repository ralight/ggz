#!/usr/bin/env python
#
# Metacle - Aggregation of metaservers
# Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import commonuri
import urllib

class MetaTransport:
	def __init__(self):
		self.transports = ["http"]

	def fetch(self, url):
		#uri = commonuri.Uri(url)

		try:
			f = urllib.urlopen(url)
		except:
			print "Error: Could not load resource %s." % url
			return None
		content = f.readlines()
		f.close()

		return " ".join(content)

