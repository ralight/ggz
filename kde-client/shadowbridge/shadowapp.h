#ifndef SHADOW_APP_H
#define SHADOW_APP_H

#include "shadowcontainer.h"
#include "shadowclient.h"
#include "shadowclientggz.h"
#include "shadownet.h"

#include <kmainwindow.h>

class ShadowApp : public KMainWindow
{
	Q_OBJECT
	public:
		ShadowApp();
		~ShadowApp();
		enum MenuItems
		{
			menuadd,
			menuaddggz,
			menustart,
			menuquit,
			menudebugin,
			menudebugout
		};
	public slots:
		void slotMenu(int id);
		void slotClient(const char *name, const char *commandline);
	private:
		ShadowContainer *container;
		ShadowClient *client;
		ShadowClientGGZ *clientggz;
		ShadowNet *net;
};

#endif

