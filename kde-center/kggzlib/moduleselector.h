#ifndef MODULESELECTOR_HEADER
#define MODULESELECTOR_HEADER

#include <qdialog.h>

#include <kggzcore/module.h>

#include "kggzlib_export.h"

class QPushButton;
class ModuleList;

class KGGZLIB_EXPORT ModuleSelector : public QDialog
{
	Q_OBJECT

	public:
		ModuleSelector(QWidget *parent);
		~ModuleSelector();

		void setModules(QList<KGGZCore::Module> modules);
		void setGameType(QString protengine, QString protversion);
		KGGZCore::Module module();

	private slots:
		void slotSelected(const KGGZCore::Module& module);

	private:
		ModuleList *m_modules;
		QPushButton *m_button;
		KGGZCore::Module m_module;
		QList<KGGZCore::Module> m_knownmodules;
		bool m_knowsmodules;
};

#endif
