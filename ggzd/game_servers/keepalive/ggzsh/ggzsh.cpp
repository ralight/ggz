// Backend languages
#define GGZSH_PYTHON
#define GGZSH_RUBY

// Include files
#include "ggzsh.h"
#ifdef GGZSH_PYTHON
#include <Python.h>
#endif
#ifdef GGZSH_RUBY
#include <ruby.h>
#endif
#include <iostream>
#include <cstring>

// Global variables
#ifdef GGZSH_PYTHON
PyObject *pxDict;
#endif
#ifdef GGZSH_RUBY

#endif

// Constructor
GGZScriptingHost::GGZScriptingHost()
{
#ifdef GGZSH_PYTHON
	pxDict = NULL;
	Py_Initialize();
#endif
#ifdef GGZSH_RUBY
	ruby_init();
	ruby_script("ggzsh");
#endif
}

// Destructor
GGZScriptingHost::~GGZScriptingHost()
{
#ifdef GGZSH_PYTHON
	Py_Finalize();
#endif
}

// Add an external script resource
void GGZScriptingHost::addScript(const char *scriptname)
{
	int scriptmode = ModePython;

#ifdef GGZSH_PYTHON
	if(scriptmode == ModePython)
	{
		PyObject *pName, *pModule;

		pName = PyString_FromString(scriptname);
		pModule = PyImport_Import(pName);

		if(pModule) pxDict = PyModule_GetDict(pModule);
		else
		{
			PyErr_Print();
			std::cout << "Failed to load '" << scriptname << "'" << std::endl;
			return;
		}

		if(pModule) Py_DECREF(pModule);
		Py_DECREF(pName);
	}
#endif
#ifdef GGZSH_RUBY
	if(scriptmode == ModeRuby)
	{
		rb_load_file((char*)scriptname);
	}
#endif
}

// Call a method from the available scripts
void GGZScriptingHost::executeMethod(const char *methodstring)
{
	int scriptmode = ModePython;

#ifdef GGZSH_PYTHON
	if(scriptmode == ModePython)
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
			std::cerr << "Command error" << std::endl;
			return;
		}

		// Lookup method
		pxFunc = PyDict_GetItemString(pxDict, function);
		if(!pxFunc)
		{
			std::cerr << "Cannot find function '" << function << "'" << std::endl;
			free(function);
			return;
		}

		if(!PyCallable_Check(pxFunc))
		{
			std::cerr << "Cannot call function '" << function << "'" << std::endl;
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
				std::cerr << "Cannot convert argument" << std::endl;
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
			std::cerr << "Result of call: " << PyInt_AsLong(pValue) << std::endl;
			Py_DECREF(pValue);
		}
		else
		{
			/*PyErr_Print();*/
			std::cerr << "Call failed" << std::endl;
		}

		free(function);
		Py_DECREF(pArgs);
	}
#endif
#ifdef GGZSH_RUBY
	if(scriptmode == ModeRuby)
	{
		ruby_run();
	}
#endif
}

void GGZScriptingHost::information()
{
	std::cout << "GGZ Scripting Host" << std::endl;
	std::cout << "Backend languages:";
#ifdef GGZSH_PYTHON
	std::cout << " python [" << PY_MAJOR_VERSION << "." << PY_MINOR_VERSION << "." << PY_MICRO_VERSION << "]";
#endif
#ifdef GGZSH_RUBY
	std::cout << " ruby [" << /*RUBY_VERSION*/ /*ruby_show_version()*/ RUBY_LIB << "]";
#endif
	std::cout << std::endl;
	std::cout << std::flush;
}

