/**********************************************/
/*                                            */
/* PyRSVGSDL - Python/SDL bindings for rsvg   */
/* Copyright (C) 2004 Josef Spillner          */
/* josef@ggzgamingzone.org                    */
/* Published under GNU GPL conditions         */
/*                                            */
/**********************************************/

/**********************************************/
/* Include files                              */
/**********************************************/

#include <Python.h>

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <librsvg/rsvg.h>

/**********************************************/
/* Prototypes                                 */
/**********************************************/

/**********************************************/
/* Definitions                                */
/**********************************************/

/**********************************************/
/* Global variables                           */
/**********************************************/

static PyObject *rsvg = NULL;

/*****************************************************/
/* GGZChess object methods just like in C            */
/*****************************************************/

static PyObject *pyrsvgsdl_render(PyObject *self, PyObject *args)
{
	char *filename;
	int width, height;
	int ret;

	GError *error = NULL;
//	const int width = 128;
//	const int height = 128;

	if(!PyArg_ParseTuple(args, "sii", &filename, &width, &height)) return NULL;

	g_type_init();
	GdkPixbuf *buf = rsvg_pixbuf_from_file_at_size(filename, width, height, &error);

/*	if(buf)
	{
		guchar *foo = gdk_pixbuf_get_pixels(buf);
		for(int i = 0; i < width; i++)
			for(int j = 0; j < height; j++)
			{
				unsigned int pixel = *(unsigned int*)(foo + (j * width + i) * 4);
				int a, r, g, b;
				r = (pixel >> 24) & 0xFF;
				g = (pixel >> 16) & 0xFF;
				b = (pixel >> 8) & 0xFF;
				a = (pixel >> 0) & 0xFF;
				pixel = (a << 24) + (r << 16) + (g << 8) + (b << 0);
				*(unsigned int*)(foo + (j * width + i) * 4) = pixel;
			}
	}
	else tmp = QPixmap(filename + ".png");
*/
	ret = 0;

	return Py_BuildValue("s#",
		gdk_pixbuf_get_pixels(buf),
		gdk_pixbuf_get_width(buf) * gdk_pixbuf_get_height(buf) * 4);
}

/**********************************************/
/* Function lookup table                      */
/**********************************************/

static PyMethodDef pyrsvgsdl_methods[] =
{
	{"render", pyrsvgsdl_render, METH_VARARGS},
	{NULL, NULL, 0}
};

/**********************************************/
/* Initialization code and constants          */
/**********************************************/

void initrsvgsdl(void)
{
	rsvg = Py_InitModule("rsvgsdl", pyrsvgsdl_methods);
}

