// Connection dialog includes
#include "connectiondialog.h"

// Qt includes
#include <qapplication.h>

// Main function, including the about data
int main(int argc, char **argv)
{
	QApplication a(argc, argv);

	ConnectionDialog *dlg = new ConnectionDialog();
	Q_UNUSED(dlg);

	return a.exec();
}

