#! /usr/bin/env python

from gtk import *
from types import *
import socket
import sys
from os import getpid
from string import *

# Connect to ggz socket
def ggz_connect(name):
  sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
  addr = "/tmp/" + name + "." + `getpid()`
  print addr
  sock.connect(addr)
  return sock

def handle_ggz(source, condition):
  text = win.get_data("text")
  data = ord(source.recv(1)[0])
  if (data > 32 and data < 126):
    char = chr(data)
  else:
    char = " "
  s = "%5d %5x %5s\n" % (data, data, char)
  text.insert_defaults(s)

# dtype = 0 -> int8
# dtype = 1 -> int32
# dtype = 2 -> char
# dtype = 3 -> string
def ggz_send(self):
  text_area = win.get_data("text")
  text = self.get_text()
  dtype = 0
  # Check if mandatory int32
  if (text[-1] == "i"):
    dtype = 1
    text = text[:-1]
  try:
    data = eval(text)
  except NameError:
    data = text
  if type(data) is StringType:
    # Ok, I wanna send a string
    if (dtype == 1):
      data = data + "i"
      dtype = 0
    dtype = 3
  if (dtype == 0 and data > 255):
    dtype = 1
  if (dtype == 1):
    data = socket.htonl(data)
  for i in range(4):
    if (dtype == 1 or dtype == 0):
      sock.send(chr(data % 256))
    else:
      sock.send(data)
    if ( (dtype == 0 or dtype == 1) and data%256 > 32 and data%256 < 126):
      char = chr(data%256)
    elif (dtype == 3):
      char = data
    else:
      char = " "
    if (dtype == 0 or dtype == 1):
      s = "%5d %5x %5s\n" % (data%256, data%256, char)
    else:
      s = "%5s\n" % (char)
    text_area.insert(text_area.get_style().font, text_area.get_colormap().alloc("red"),
    text_area.get_style().white, s)
    if (dtype == 0 or dtype == 1):
      data = data / 256
    if (dtype != 1):
      break
  self.set_text("")

# Create main window
global win
win = GtkWindow()
win.connect("destroy", mainquit)
win.connect("delete_event", mainquit)
win.set_title("Python Generic Client for GGZ")
win.show()

# Create VBox
vbox = GtkVBox(FALSE, 0)
win.add(vbox)
vbox.show()

# Create Text
text = GtkText()
text.set_editable(FALSE)
win.set_data("text", text)
text.show()

# Create scrolled window
scroll_win = GtkScrolledWindow(text.get_hadjustment(), text.get_vadjustment())
scroll_win.set_policy(1, 1)
scroll_win.add(text)
vbox.add(scroll_win)
scroll_win.show()

# Create entry
entry = GtkEntry()
vbox.pack_start(entry, FALSE, FALSE, 0)
entry.connect("activate", ggz_send)
entry.show()

# Connect to ggz server
global sock
sock = ggz_connect(sys.argv[1])

# Add a input handler
input_add(sock, 1<<0, handle_ggz)

mainloop()
