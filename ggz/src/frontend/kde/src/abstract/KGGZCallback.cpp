// Declaration
#include "KGGZCallback.h"

// Abstract KGGZ classes
#include "KGGZCommon.h"

KGGZCallback::KGGZCallback(void* pointer, int opcode)
{
	KGGZDEBUGF("** created callback atom, type: %i\n", opcode);
	m_pointer = pointer;
	m_opcode = opcode;
}

KGGZCallback::~KGGZCallback()
{
	KGGZDEBUGF("** destroyed callback atom, type: %i\n", m_opcode);
}

void* KGGZCallback::pointer()
{
	return m_pointer;
}

int KGGZCallback::opcode()
{
	return m_opcode;
}
