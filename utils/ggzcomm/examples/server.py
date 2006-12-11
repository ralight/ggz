#!/usr/bin/env python

import socket
import time
import select
import connectx_server
import sys

sock = socket.socket()
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind(("127.0.0.1", 10000))
sock.listen(128)
conn = sock.accept()

print "connection from client", conn
(csock, cdata) = conn

def cb(name, message):
	print "- message is", name

proto = connectx_server.connectx()
proto.ggzcomm_set_notifier_callback(cb)
proto.ggzcomm_set_socket(csock)

print "require options :)"
req = connectx_server.reqoptions()
req.minboardwidth = 3
req.maxboardwidth = 20
req.minboardheight = 3
req.maxboardheight = 15
req.minconnectlength = 4
req.maxconnectlength = 7
proto.ggzcomm_reqoptions(req)

while 1:
	(r, w, x) = select.select([csock], [], [])
	print "message from client!"
	#try:
	proto.ggzcomm_network_main()
	#except:
	#	print "ouch! the game is over by accident"
	#	sys.exit(1)

