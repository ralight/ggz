#!/usr/bin/env python
#
# Metacle - Aggregation of metaservers
# Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import commonuri

class MetaTransport:
	def __init__(self):
		self.transports = ["file"]

	def fetch(self, url):
		uri = commonuri.Uri(url)

		try:
			f = open(uri.path)
		except:
			print "Error: Could not load resource %s." % url
			return None
		content = f.readlines()
		f.close()

		return " ".join(content)

