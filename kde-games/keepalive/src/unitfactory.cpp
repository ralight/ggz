#include "unitfactory.h"

#include <qpixmap.h>
#include <qimage.h>
#include <qbitmap.h>
#include <qcanvas.h>

#include "config.h"

#define KEEPALIVE_DIR GGZDATADIR "/keepalive"

UnitFactory::UnitFactory()
{
}

UnitFactory::~UnitFactory()
{
}

QCanvasPixmapArray *UnitFactory::load(const char *image)
{
	QPixmap pixtmp;
	QImage imtmp;
	QCanvasPixmapArray *a;
	int width, height;
	QString builtin;

	QImage im(QString("/usr/share/games/freecraft/default/graphics/alliance/units/%1.png").arg(image));
	if(im.isNull()) im = QImage(QString("/usr/lib/games/freecraft/data/graphics/alliance/units/%1.png").arg(image));
	if(im.isNull()) im = QImage(QString("/usr/share/games/freecraft/default/graphics/alliance/units/knight.png"));
	if(im.isNull())
	{
		// fall back to built-in images
		builtin = image;
		if(builtin == "gryphon_rider") return NULL;
		else if(builtin == "footman") im = QImage(KEEPALIVE_DIR "/man.png");
		else if(builtin == "knight") im = QImage(KEEPALIVE_DIR "/avatar.png");
		else if(builtin == "mage") im = QImage(KEEPALIVE_DIR "/avatar.png");
		else if(builtin == "archer") im = QImage(KEEPALIVE_DIR "/avatar.png");
		else return NULL;
	}

	if(builtin)
	{
		a = new QCanvasPixmapArray();
		pixtmp.convertFromImage(im);
		for(int i = 0; i < 40; i++)
			a->setImage(i, new QCanvasPixmap(pixtmp, QPoint(16, 16)));
	}
	else
	{
		// Schema for FreeCraft FcMP:
		// 1:[north,northwest,...,south,...,northeast], 2:[...], ..., 5:[...]
		// 5 sequences á 8 directions, thus each character has 40 images
		// east directions are mirrored

		width = im.width() / 5;
		height = width;

		a = new QCanvasPixmapArray();
		for(int j = 0; j < 5; j++)
		{
			for(int i = 0; i < 5; i++)
			{
				imtmp = im.copy(i * width, j * height, width, height);
				pixtmp.convertFromImage(imtmp);
				pixtmp.setMask(pixtmp.createHeuristicMask());
				a->setImage(j * 8 + i, new QCanvasPixmap(pixtmp, QPoint(width / 2, height / 2)));

				if((i >= 1) && (i <= 3))
				{
					imtmp = imtmp.mirror(true, false);
					pixtmp.convertFromImage(imtmp);
					pixtmp.setMask(pixtmp.createHeuristicMask());
					a->setImage(j * 8 + 8 - i, new QCanvasPixmap(pixtmp, QPoint(width / 2, height / 2)));
				}
			}
		}
	}

	return a;
}

