#include "kcm_ggz_drs.h"
#include "kcm_ggz_drs.moc"

KCMGGZDRS::KCMGGZDRS(QWidget *parent, const char *name)
: KCMGGZPane(parent, name)
{
	load();
}

KCMGGZDRS::~KCMGGZDRS()
{
}

void KCMGGZDRS::load()
{
}

void KCMGGZDRS::save()
{
}

QString KCMGGZDRS::caption()
{
	return "DRS System";
}

extern "C"
{
	KCMGGZPane *kcmggz_init(QWidget *parent, const char *name)
	{
		return new KCMGGZDRS(parent, name);
	}
}

