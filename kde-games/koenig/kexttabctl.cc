#include "kexttabctl.h"

KExtTabCtl::KExtTabCtl(QWidget *parent, const char *name)
: KTabCtl(parent, name)
{
}

KExtTabCtl::~KExtTabCtl()
{
}

void KExtTabCtl::showTab(int i)
{
	KTabCtl::showTab(i);
}

