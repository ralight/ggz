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
		self.user = None
		self.password = None

		r_uri = re.compile("(?:([^:]+):(\/\/)?)?(?:([^@:]+)(?::([^@]+))?@)?([^:\/]+)?(?::(\d+))?(?:\/(\S+))?")
		m_uri = r_uri.match(s)

		#print "#", m_uri.groups()
		protocol = m_uri.group(1)
		remote = m_uri.group(2)
		user = m_uri.group(3)
		password = m_uri.group(4)
		host = m_uri.group(5)
		if m_uri.group(6):
			port = int(m_uri.group(6))
		else:
			port = 0
		path = m_uri.group(7)

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
		self.user = user
		self.password = password

		#print s
		#print "# -> proto=", self.protocol, "host=", self.host, "port=", self.port, "path=", self.path, "(remote)=", remote
		#print "   # user=", self.user, "password=", self.password

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

	u = Uri("jabber:user@localhost")
	u = Uri("ggz://user@localhost")
	u = Uri("ggz://user:pass@localhost:5688")

