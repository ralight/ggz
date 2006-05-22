#!/usr/bin/env python
#
# Metacle - Aggregation of metaservers
# Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import socket
import xml.dom.minidom
import commonuri

class MetaTransport:
	def __init__(self):
		self.transports = ["ggzmeta"]

	def fetch(self, url):
		uri = commonuri.Uri(url)
		if not uri.port:
			uri.port = 15689

		sock = socket.socket()
		sock.connect((uri.host, uri.port))
		sock.send("<?xml version=\"1.0\"?>")
		sock.send("<query class=\"ggz\" type=\"connection\"/>")
		sock.send("\n")

		response = ""
		while 1:
			s = sock.recv(1024)
			print "<>", s
			if not s:
				break
			response += s

			try:
				dom = xml.dom.minidom.parseString(response)
				#print "yeah :)"
				break
			except:
				#print "nope :("
				pass

		#print "### RESPONSE:", response
		return response

		if 1 == 0:
			# FIXME: error handling
			print "Error: Could not load resource %s." % url
			return None

if __name__ == "__main__":
	print "*ggz metaserver transport test*"
	m = MetaTransport()
	m.fetch("ggzmeta://localhost:15689")

