#ifndef FYRDMAN_SKEL
#define FYRDMAN_SKEL

#include <dcopobject.h>

class FyrdmanDcop : virtual public DCOPObject
{
	K_DCOP
	k_dcop:
		virtual ASYNC newLevel(QString level) = 0;
};

#endif
