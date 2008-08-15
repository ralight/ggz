// Connection dialog includes
#include <kggzlib/tableconfiguration.h>

// Qt includes
#include <qapplication.h>

// KDE includes
//#include <kapplication.h>
#include <kcomponentdata.h>

// Main function, including the about data
int main(int argc, char **argv)
{
	QApplication a(argc, argv);

	KComponentData data("connectiondialog");

	TableConfiguration dlg(NULL);
	dlg.exec();

	return a.exec();
}

