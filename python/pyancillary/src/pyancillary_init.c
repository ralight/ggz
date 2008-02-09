/**********************************************/
/*                                            */
/* PyAncillary - Python wrapper for ggz_*_fd  */
/* Copyright (C) 2008 Josef Spillner          */
/* josef@ggzgamingzone.org                    */
/* Published under GNU GPL conditions         */
/*                                            */
/**********************************************/

/**********************************************/
/* Include files                              */
/**********************************************/

#include <Python.h>
#include <ggz.h>

/***********************************************/
/* Python wrapper methods for libggz functions */
/***********************************************/

static PyObject *pyancillary_write_fd(PyObject *self, PyObject *args)
{
	int socket;
	int fd;
	int ret;

	if(!PyArg_ParseTuple(args, "ii", &socket, &fd)) return NULL;
	ret = ggz_write_fd(socket, fd);
	if(ret == -1) return Py_None;
	return Py_BuildValue("(i)", 0);
}

static PyObject *pyancillary_read_fd(PyObject *self, PyObject *args)
{
	int socket;
	int fd;
	int ret;

	if(!PyArg_ParseTuple(args, "i", &socket)) return NULL;
	ret = ggz_read_fd(socket, &fd);
	if(ret == -1) return Py_None;
	return Py_BuildValue("(i)", fd);
}

/**********************************************/
/* Function lookup table                      */
/**********************************************/

static PyMethodDef pyancillary_methods[] =
{
	{"read_fd", pyancillary_read_fd, METH_VARARGS},
	{"write_fd", pyancillary_write_fd, METH_VARARGS},
	{NULL, NULL}
};

/**********************************************/
/* Initialization code and constants          */
/**********************************************/

void initancillary(void)
{
	PyObject *ancillary;

	ancillary = Py_InitModule("ancillary", pyancillary_methods);
}

