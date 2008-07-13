// SVG support for Qt through librsvg
// Copyright (C) 2005 - 2006 Josef Spillner <josef@ggzgamingzone.org>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "qsvgpixmap.h"

#include "config.h"

#include <qpixmap.h>
#include <qimage.h>

#ifdef HAVE_RSVG
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <librsvg/rsvg.h>
#endif

QPixmap QSvgPixmap::pixmap(QString filename, int width, int height)
{
	QPixmap tmp;
#ifdef HAVE_RSVG
	GError *error = NULL;
	bool bigendian = false;

	g_type_init();
	GdkPixbuf *buf = rsvg_pixbuf_from_file_at_size(filename.latin1(), width, height, &error);

	if(buf)
	{
		guchar *pixdata = gdk_pixbuf_get_pixels(buf);
		int test = 1;
		if(!(*(char*)&test & 0xFF)) bigendian = true;
		if(bigendian)
		{
			for(int i = 0; i < width; i++)
				for(int j = 0; j < height; j++)
				{
					unsigned int pixel = *(unsigned int*)(pixdata + (j * width + i) * 4);
					int a, r, g, b;
					r = (pixel >> 24) & 0xFF;
					g = (pixel >> 16) & 0xFF;
					b = (pixel >> 8) & 0xFF;
					a = (pixel >> 0) & 0xFF;
					pixel = (a << 24) + (r << 16) + (g << 8) + (b << 0);
					*(unsigned int*)(pixdata + (j * width + i) * 4) = pixel;
				}
		}
		else
		{
			for(int i = 0; i < width; i++)
				for(int j = 0; j < height; j++)
				{
					unsigned int pixel = *(unsigned int*)(pixdata + (j * width + i) * 4);
					int a, r, g, b;
					a = (pixel >> 24) & 0xFF;
					b = (pixel >> 16) & 0xFF;
					g = (pixel >> 8) & 0xFF;
					r = (pixel >> 0) & 0xFF;
					pixel = (a << 24) + (r << 16) + (g << 8) + (b << 0);
					*(unsigned int*)(pixdata + (j * width + i) * 4) = pixel;
				}
		}

		QImage im(gdk_pixbuf_get_pixels(buf), gdk_pixbuf_get_width(buf), gdk_pixbuf_get_height(buf),
			gdk_pixbuf_get_bits_per_sample(buf) * 4, 0, 0, QImage::IgnoreEndian);
		im.setAlphaBuffer(true);
		tmp.convertFromImage(im);
	}
	else
	{
		qDebug("QSvgPixmap: Error on file %s, falling back to PNG.", filename.utf8().data());
		tmp = QPixmap(filename + ".png");
	}
#else
	tmp = QPixmap(filename + ".png");
#endif
	if(tmp.isNull()) tmp = QPixmap(filename.replace("svg", "png"));
	return tmp;
}

