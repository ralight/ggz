#ifndef KEEPALIVE_UNITFACTORY_H
#define KEEPALIVE_UNITFACTORY_H

class QCanvasPixmapArray;

class UnitFactory
{
	public:
		UnitFactory();
		~UnitFactory();
		QCanvasPixmapArray *load(const char *image);
};

#endif

