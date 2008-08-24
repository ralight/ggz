// Connection dialog includes
#include <kggzlib/connectiondialog.h>
#include <kggzlib/ggzserver.h>

// Qt includes
#include <qapplication.h>

// KDE includes
#include <kcomponentdata.h>

// Main function, including the about data
int main(int argc, char **argv)
{
	QApplication a(argc, argv);

	KComponentData data("connectiondialog");

	ConnectionDialog dlg(NULL);
	return dlg.exec();
}

