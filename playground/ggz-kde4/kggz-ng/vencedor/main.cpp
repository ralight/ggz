#include "vencedor.h"

#include <QApplication>
#include <QTextCodec>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));

	QString url;
	if(argc == 2)
		url = argv[1];
	else
		url = "ggz://guest9999@localhost:5688";

	Vencedor *vencedor = new Vencedor(url);
	Q_UNUSED(vencedor);

	return app.exec();
}

