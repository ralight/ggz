#ifndef KEXTTABCTL_H
#define KEXTTABCTL_H

#include <ktabctl.h>

class KExtTabCtl : public KTabCtl
{
	public:
		KExtTabCtl(QWidget *parent = NULL, const char *name = NULL);
		~KExtTabCtl();
		void showTab(int i);
};

#endif

