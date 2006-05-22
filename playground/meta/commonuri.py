#!/usr/bin/env python
#
# Metacle - Aggregation of metaservers
# Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

# [protocol:[//]][host][:port][[/]path]
# defaults:
#  protocol = file (without //)
#  host = -- (required, but not possible with file)
#            (still possible with absolute paths, e.g. file:///tmp/x.txt)
#  port = -- (depends on protocol, e.g. 15689 for ggz)
#  path = "" (empty) (but required if no host is given)

import re

class Uri:
	def __init__(self, s):
		self.protocol = None
		self.host = None
		self.port = None
		self.path = None

		#r_uri = re.compile("(?:(\S+):(?:\/\/)?)?(\S+)?(?::(\d+))?(?:\/(\S+))?")
		r_uri = re.compile("(?:([^:]+):(\/\/)?)?([^:\/]+)?(?::(\d+))?(?:\/(\S+))?")
		m_uri = r_uri.match(s)

		#print "#", m_uri.groups()
		protocol = m_uri.group(1)
		remote = m_uri.group(2)
		host = m_uri.group(3)
		if m_uri.group(4):
			port = int(m_uri.group(4))
		else:
			port = 0
		path = m_uri.group(5)

		if protocol == "file" and not path:
			path = host
			host = None

		if not remote:
			if host and path:
				path = host + "/" + path
				host = None

		if protocol and host:
			if not port and not path and not remote:
				port = host
				host = protocol
				protocol = None

		self.protocol = protocol
		self.host = host
		self.port = port
		self.path = path

		#print "# ->", self.protocol, self.host, self.port, self.path, remote

if __name__ == "__main__":
	u = Uri("file:foo.txt")
	u = Uri("file:foobar/foo.txt")
	u = Uri("file:///foo.txt")
	u = Uri("ggz://localhost")
	u = Uri("ggz://localhost:15689")
	u = Uri("ggz://localhost:15689/#Tic-Tac-Toe")
	u = Uri("http://localhost/foobar")
	u = Uri("localhost")
	u = Uri("localhost:2000")

