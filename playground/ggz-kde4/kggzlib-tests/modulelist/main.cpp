// Player list includes
#include <kggzlib/modulelist.h>
#include <kggzcore/module.h>

// Qt includes
#include <qapplication.h>

#include <kcomponentdata.h>

// Main function, including the about data
int main(int argc, char **argv)
{
	QApplication a(argc, argv);

	KComponentData data("modulelist");

	ModuleList *list = new ModuleList();

	KGGZCore::Module mod1;
	mod1.setName("First Module");
	mod1.setFrontend("somefrontend");

	KGGZCore::Module mod2;
	mod2.setName("Second Module");
	mod2.setFrontend("otherfrontend");

	list->addModule(mod1);
	list->addModule(mod2);

	list->show();

	return a.exec();
}

