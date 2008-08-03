// Connection dialog includes
#include <connectiondialog.h>
#include <ggzserver.h>

// Qt includes
#include <qapplication.h>

// KDE includes
//#include <kapplication.h>
#include <kcomponentdata.h>

// Main function, including the about data
int main(int argc, char **argv)
{
	QApplication a(argc, argv);
	//KApplication a;

	KComponentData data("connectiondialog");

	ConnectionDialog *dlg = new ConnectionDialog();

	GGZServer server;
	server.setUri("ggz://live.ggzgamingzone.org");
	server.setName("Josef's server");
	server.setLoginType("guest");
	server.setIcon("http://us.ggzgamingzone.org/~josef/cestmoi2.png");
	dlg->addServer(server);

	GGZServer server2;
	server2.setUri("ggz://live.ggzgamingzone.org");
	server2.setName("GGZ Live server");
	server2.setLoginType("registered");
	server2.setIcon("/home/josef/home.checkout/projects/ggz-trunk/playground/ggz-kde4/kggzlib-tests/connection-dialog/ggzlogo.png");
	dlg->addServer(server2);
	//mymap2["icon"] = "ggzlogo.png";

	GGZServer server3;
	server3.setUri("ggz://localhost:5688");
	server3.setName("Honest Harry's Server");
	server3.setLoginType("guest");
	server3.setIcon("honestharry.png");
	dlg->addServer(server3);

	return a.exec();
}

