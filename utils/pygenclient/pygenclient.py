#! /usr/local/bin/python1.5

from gtk import *
import socket
import sys
from os import getpid
from string import atoi

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

def ggz_send(self):
  text_area = win.get_data("text")
  text = self.get_text()
  int32 = 0
  if (text[-1] == "i"):
    int32 = 1
    text = text[:-1]
  data = eval(text)
  if (data > 255):
    int32 = 1
  if (int32):
    data = socket.htonl(data)
  for i in range(4):
    sock.send(chr(data % 256))
    if (data%256 > 32 and data%256 < 126):
      char = chr(data%256)
    else:
      char = " "
    s = "%5d %5x %5s\n" % (data%256, data%256, char)
    text_area.insert(text_area.get_style().font, text_area.get_colormap().alloc("red"),
    text_area.get_style().white, s)
    data = data / 256
    if (not int32):
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
