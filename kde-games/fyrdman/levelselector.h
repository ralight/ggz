#ifndef FYRDMAN_LEVELSELECTOR_H
#define FYRDMAN_LEVELSELECTOR_H

#include <qdialog.h>
#include <qptrlist.h>

class QComboBox;
class QTextEdit;
class Level;

class LevelSelector : public QDialog
{
	Q_OBJECT
	public:
		LevelSelector(QWidget *parent = NULL, const char *name = NULL);
		~LevelSelector();

		void addLevel(Level *level);
		QString level();

	protected slots:
		void accept();
		void slotActivated(int id);

	private:
		QComboBox *combo;
		QTextEdit *desc;
		QPtrList<Level> m_levels;
};

#endif

