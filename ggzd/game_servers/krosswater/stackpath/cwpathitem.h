/////////////////////////////////////////////
//                                         //
// Stackpath 0.1                           //
//                                         //
// Copyright (C) 2001, 2002 Josef Spillner //
// dr_maux@users.sourceforge.net           //
// The MindX Open Source Project           //
// http://mindx.sourceforge.net            //
//                                         //
// Published under GNU GPL conditions.     //
//                                         //
/////////////////////////////////////////////

// Stackpath includes
#include "pathitem.h"

/**
* This is a useful helper class for Pathitem. It should be used when handling
* maps of any kind.
*/
class CWPathitem : public Pathitem
{
	public:
		/** Constructor: Directly invoking the Pathitem one. */
		CWPathitem(int width, int height, int** field);
		/** Destructor */
		~CWPathitem();
		/** Checkes whether a field in the specified column is set. If not, do so when mode = force. */
		int available(int column, int mode);
		/** After processing, get backtrace list, or NULL. */
		Pathitem* result();
		/** Change a value manually */
		void setValue(int x, int y, int value);
		/** Get a value */
		int value(int x, int y);
		/** Extended check within one column */
		void postprocess(int column);

		/** Possible modes for available() */
		enum Modes
		{
			force,
			noforce
		};
};

