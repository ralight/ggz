/**********************************************/
/*                                            */
/* PyRSVGSDL - Python/SDL bindings for rsvg   */
/* Copyright (C) 2004, 2005 Josef Spillner    */
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

/**********************************************/
/* Object definitions                         */
/**********************************************/

staticforward PyTypeObject pyrsvgsdl_SurfaceType;
staticforward PyMethodDef pyrsvgsdl_surface_methods[];

typedef struct
{
	PyObject_HEAD
	GdkPixbuf *buf;
}
pyrsvgsdl_SurfaceObject;

static void pyrsvgsdl_delete_surface(pyrsvgsdl_SurfaceObject *self)
{
	if(self->buf)
	{
		g_object_unref(self->buf);
	}

	PyObject_Del(self);
}

static PyTypeObject pyrsvgsdl_SurfaceType =
{
	PyObject_HEAD_INIT(NULL)
	0,
	"RSVG SDL surface",
	sizeof(pyrsvgsdl_SurfaceObject),
	0,
	(destructor)pyrsvgsdl_delete_surface,
};

static PyObject *pyrsvgsdl_surface_getattr(PyObject *self, char *name)
{
	PyObject *ret = NULL;

	ret = Py_FindMethod(pyrsvgsdl_surface_methods, self, name);

	return ret;
}

static PyObject *pyrsvgsdl_new_surface(PyObject *self, PyObject *args)
{
	pyrsvgsdl_SurfaceObject *surface;

	pyrsvgsdl_SurfaceType.ob_type = &PyType_Type;

	pyrsvgsdl_SurfaceType.tp_methods = (struct PyMethodDef*)pyrsvgsdl_surface_methods;
	pyrsvgsdl_SurfaceType.tp_getattr = (getattrfunc)pyrsvgsdl_surface_getattr;

	surface = PyObject_New(pyrsvgsdl_SurfaceObject, &pyrsvgsdl_SurfaceType);
	surface->buf = NULL;

	return (PyObject*)surface;
}

/*************************************************/
/* RSVG object methods just like in C            */
/*************************************************/

static PyObject *pyrsvgsdl_render(PyObject *self, PyObject *args)
{
	char *filename;
	int width, height;
	pyrsvgsdl_SurfaceObject *surface;

	GError *error = NULL;

	if(!PyArg_ParseTuple(args, "sii", &filename, &width, &height)) return NULL;

	g_type_init();
	GdkPixbuf *buf = rsvg_pixbuf_from_file_at_size(filename, width, height, &error);

	surface = (pyrsvgsdl_SurfaceObject*)pyrsvgsdl_new_surface(NULL, NULL);
	surface->buf = buf;
	return (PyObject*)surface;
}

static PyObject *pyrsvgsdl_surface_width(pyrsvgsdl_SurfaceObject *self, PyObject *args)
{
	int ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;

	ret = gdk_pixbuf_get_width(self->buf);

	return Py_BuildValue("i", ret);
}

static PyObject *pyrsvgsdl_surface_height(pyrsvgsdl_SurfaceObject *self, PyObject *args)
{
	int ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;

	ret = gdk_pixbuf_get_height(self->buf);

	return Py_BuildValue("i", ret);
}

static PyObject *pyrsvgsdl_surface_data(pyrsvgsdl_SurfaceObject *self, PyObject *args)
{
	GdkPixbuf *buf;

	if(!PyArg_ParseTuple(args, "")) return NULL;

	buf = self->buf;

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

static PyMethodDef pyrsvgsdl_surface_methods[] =
{
	{"data", pyrsvgsdl_surface_data, METH_VARARGS},
	{"width", pyrsvgsdl_surface_width, METH_VARARGS},
	{"height", pyrsvgsdl_surface_height, METH_VARARGS},
	{NULL, NULL, 0}
};

/**********************************************/
/* Initialization code and constants          */
/**********************************************/

void initrsvgsdl(void)
{
	rsvg = Py_InitModule("rsvgsdl", pyrsvgsdl_methods);
}

