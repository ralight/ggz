// KGGZ includes
#include <kchat.h>

// Qt includes
#include <qapplication.h>

// KDE includes
#include <kcomponentdata.h>

// Main function, including the about data
int main(int argc, char **argv)
{
	QApplication a(argc, argv);

	KComponentData data("tabledialog");

	KChat chat(NULL, false);
	chat.setFromNickname("from-nick");
	chat.addPlayer("to-nick-1");
	chat.addPlayer("to-nick-2");
	chat.show();

	return a.exec();
}

