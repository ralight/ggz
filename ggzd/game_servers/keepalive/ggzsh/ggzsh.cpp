#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

// Include files
#include "ggzsh.h"
#include <Python.h>
#include <iostream>
#include <cstring>

// Global variables
PyObject *pxDict;

// Constructor
GGZScriptingHost::GGZScriptingHost()
{
	pxDict = NULL;

	Py_Initialize();
}

// Destructor
GGZScriptingHost::~GGZScriptingHost()
{
	Py_Finalize();
}

// Add an external script resource
void GGZScriptingHost::addScript(const char *scriptname)
{
	PyObject *pName, *pModule;

	pName = PyString_FromString(scriptname);
	pModule = PyImport_Import(pName);

	if(pModule) pxDict = PyModule_GetDict(pModule);
	else
	{
		/*PyErr_Print();*/
		cout << "Failed to load '" << scriptname << "'" << endl;
		return;
	}

	if(pModule) Py_DECREF(pModule);
	Py_DECREF(pName);
}

// Call a method from the available scripts
void GGZScriptingHost::executeMethod(const char *methodstring)
{
	PyObject *pxFunc;
	PyObject *pArgs, *pValue;
	int i, result;
	char *tmp;
	char *function;
	char *argument;
	int argc;

	// Safer
	if(!pxDict) return;

	// Count number of arguments
	tmp = strdup(methodstring);
	argc = -1;
	argument = strtok(tmp, " ");
	if(argument) function = strdup(argument);
	while(argument)
	{
		argument = strtok(NULL, " ");
		argc++;
	}
	free(tmp);
	if(argc < 0)
	{
		cerr << "Command error" << endl;
		return;
	}

	// Lookup method
	pxFunc = PyDict_GetItemString(pxDict, function);
	if(!pxFunc && PyCallable_Check(pxFunc))
	{
		cerr << "Cannot find function '" << function << "'" << endl;
		free(function);
		return;
	}

	// Build arguments vector
	tmp = strdup(methodstring);
	argument = strtok(tmp, " ");
	pArgs = PyTuple_New(argc);
	for(i = 0; i < argc; ++i)
	{
		argument = strtok(NULL, " ");
		pValue = PyInt_FromLong(atoi(argument));
		if(!pValue)
		{
			cerr << "Cannot convert argument" << endl;
			free(function);
			return;
		}
		PyTuple_SetItem(pArgs, i, pValue);
	}
	free(tmp);

	// Call the method with the arguments
	pValue = PyObject_CallObject(pxFunc, pArgs);
	if(pValue != NULL)
	{
		cerr << "Result of call: " << PyInt_AsLong(pValue) << endl;
		Py_DECREF(pValue);
	}
	else
	{
		/*PyErr_Print();*/
		cerr << "Call failed" << endl;
	}

	free(function);
	Py_DECREF(pArgs);
}

