from gi.repository import Gtk, Gdk, GObject, Gst, GLib
import cairo
import sys
import json
import zmq

builder = Gtk.Builder()
builder.add_from_file("ui.glade")

window = builder.get_object("window1")
screen = builder.get_object("drawingarea1")
window.show_all()

fbuf = b""

class Handler :
	def __init__(self, display) :
		self.display = display
	
	def display_draw(self, widget, cr) :
		width = widget.get_allocation().width
		height = widget.get_allocation().height
		
		cr.set_source_rgb(0,0,0)
		cr.paint()
		border = 10
		cr.translate(border, border)
		sc = min((width-2*border)/128,(height-2*border)/64)
		cr.scale(sc, sc)
		cr.set_source_rgb(16/255,26/255,26/255)
		cr.rectangle(0,0,128,64)
		cr.fill()
		cr.set_source_rgb(194/255,1,250/255)
		pxd = .05
		for i,b in enumerate(fbuf) :
			for j in range(8) :
				if b&(1<<j) :
					cr.rectangle(i%128+pxd,(i//128)*8+j+pxd,1-2*pxd,1-2*pxd)
					cr.fill()
		#cr.set_source_rgb(0,0,0)
		#
		#
		#print("draw")
		#self.display.draw(widget, cr)
		
	def button_mode_clicked(self, widget) :
		pass
	
	def button_light_clicked(self, widget) :
		print("light")
		
	def button_mode(self, widget) :
		sock.send(b"m")
		
	def button_select(self, widget) :
		sock.send(b"s")
		
	def button_up(self, widget) :
		sock.send(b"u")
	
	def button_down(self, widget) :
		sock.send(b"d")
	
	
	def quit(self, widget) :
		Gtk.main_quit()
builder.connect_signals(Handler(0))	

ctx = zmq.Context()

sock = ctx.socket(zmq.REQ)
#ock.setsockopt(zmq.SUBSCRIBE, b"")
sock.connect("ipc://displaysim")


def zmq_callback(queue, condition):
	global fbuf
	while sock.getsockopt(zmq.EVENTS) & zmq.POLLIN:
		rx = sock.recv()
		fbuf = rx
		screen.queue_draw()
		print("rx")
	
		
	return True

GLib.io_add_watch(sock.getsockopt(zmq.FD), GLib.IO_IN|GLib.IO_ERR|GLib.IO_HUP	, zmq_callback)

Gtk.main()
