//////////////////////////////////////////////////////////////////////
// KConnectX
// Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

#ifndef KCONNECTX_OPT_H
#define KCONNECTX_OPT_H

// KDE includes
#include <kdialogbase.h>

class QSlider;
class QLabel;

class Opt : public KDialogBase
{
	Q_OBJECT
	public:
		// Constructor
		Opt(QWidget *parent = NULL, const char *name = NULL);

		void setBoardHeightRange(char min, char max);
		void setBoardWidthRange(char min, char max);
		void setConnectLengthRange(char min, char max);

		char boardHeight();
		char boardWidth();
		char connectLength();

	private slots:
		void slotWidth(int width);
		void slotHeight(int height);
		void slotLength(int length);

	private:
		QSlider *m_boardheight, *m_boardwidth, *m_connectlength;
		QLabel *m_lboardheight, *m_lboardwidth, *m_lconnectlength;
};

#endif

