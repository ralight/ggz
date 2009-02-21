// Player list includes
#include <kggzlib/moduledialog.h>

// Qt includes
#include <qapplication.h>

#include <kcomponentdata.h>

// Main function, including the about data
int main(int argc, char **argv)
{
	QApplication a(argc, argv);

	KComponentData data("moduledialog");

	ModuleDialog *dialog = new ModuleDialog(NULL);

	dialog->show();

	return a.exec();
}

