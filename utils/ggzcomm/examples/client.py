#!/usr/bin/env python

import socket
import time
import select
import connectx_client
import sys

sock = socket.socket()

try:
	sock.connect(("127.0.0.1", 10000))
except:
	print "ouch! connection refused"
	sys.exit(1)

print "connection to server", sock

def cb(name, message):
	print "- message is", name
	snd = connectx_client.sndoptions()
	snd.boardheight2 = 10
	snd.boardwidth2 = 10
	snd.connectlength2 = 5
	proto.ggzcomm_sndoptions(snd)

proto = connectx_client.connectx()
proto.ggzcomm_set_notifier_callback(cb)
proto.ggzcomm_set_socket(sock)

while 1:
	(r, w, x) = select.select([sock], [], [])
	print "message from server!"
	#try:
	proto.ggzcomm_network_main()
	#except:
	#	print "ouch! the game is over by accident"
	#	sys.exit(1)

