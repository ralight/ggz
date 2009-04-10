#include "util.h"

#include "ggzprofile.h"

#include <kconfiggroup.h>

#include <qurl.h>

void Util::saveprofile(const GGZProfile& profile, KConfigGroup& cg, KSharedConfig::Ptr conf)
{
	GGZServer server = profile.ggzServer();
	cg.writeEntry("Uri", server.uri());
	cg.writeEntry("Api", server.api());
	cg.writeEntry("Name", server.name());
	cg.writeEntry("Icon", server.icon());

	cg.writeEntry("Username", profile.username());
	cg.writeEntry("Password", profile.password());
	cg.writeEntry("Roomname", profile.roomname());
	cg.writeEntry("Realname", profile.realname());
	cg.writeEntry("Email", profile.email());
	cg.writeEntry("LoginType", (int)profile.loginType());

	conf->sync();
}

GGZProfile Util::loadprofile(const KConfigGroup& cg) const
{
	GGZServer server;
	server.setUri(cg.readEntry("Uri"));
	server.setApi(cg.readEntry("Api"));
	server.setName(cg.readEntry("Name"));
	server.setIcon(cg.readEntry("Icon"));

	GGZProfile profile;
	profile.setGGZServer(server);
	profile.setUsername(cg.readEntry("Username"));
	profile.setPassword(cg.readEntry("Password"));
	profile.setRoomname(cg.readEntry("Roomname"));
	profile.setRealname(cg.readEntry("Realname"));
	profile.setEmail(cg.readEntry("Email"));
	profile.setLoginType((GGZProfile::LoginType)cg.readEntry("LoginType").toInt());

	return profile;
}

QPixmap Util::composite(QPixmap bottom, QPixmap top)
{
	QPixmap comp;

	QImage topim = top.toImage();
	QImage bottomim = bottom.toImage();

	int width = qMin(bottom.width(), top.width());
	int height = qMin(bottom.height(), top.height());

	for(int j = 0; j < height; j++)
	{
		for(int i = 0; i < width; i++)
		{
			if(qAlpha(topim.pixel(i, j)))
				bottomim.setPixel(i, j, topim.pixel(i, j));
		}
	}

	comp = comp.fromImage(bottomim);

	return comp;
}

GGZProfile Util::uriToProfile(const QString& uri)
{
	QUrl qurl(uri);
	if((qurl.scheme().isEmpty()) || (qurl.host().isEmpty()))
	{
		// Even in tolerant parsing mode, the scheme must be present
		// When a password is given, username becomes scheme and hostname becomes empty
		qurl = QUrl("ggz://" + uri);
	}
	if(qurl.port() == -1)
		qurl.setPort(5688);

	GGZProfile profile;
	if(!qurl.userName().isEmpty())
		profile.setUsername(qurl.userName());
	if(!qurl.password().isEmpty())
		profile.setPassword(qurl.password());
	if(!qurl.path().isEmpty())
		profile.setRoomname(qurl.path());
	// FIXME: parse path correctly according to GGZ URI draft

	if(!qurl.password().isEmpty())
		profile.setLoginType(GGZProfile::registered);
	else
		profile.setLoginType(GGZProfile::guest);

	QString serveruri = qurl.toString(QUrl::RemoveUserInfo | QUrl::RemovePath);

	GGZServer server;
	server.setUri(serveruri);
	profile.setGGZServer(server);

	return profile;
}

// FIXME: use kimagefx?
QPixmap Util::greyscale(QPixmap orig)
{
	QPixmap comp;
	QRgb rgb;

	QImage origim = orig.toImage();

	for(int j = 0; j < orig.height(); j++)
	{
		for(int i = 0; i < orig.width(); i++)
		{
			rgb = origim.pixel(i, j);
			if(qAlpha(rgb))
			{
				int mix = (qRed(rgb) + qGreen(rgb) + qBlue(rgb)) / 3;
				origim.setPixel(i, j, qRgb(mix, mix, mix));
			}
		}
	}

	comp = comp.fromImage(origim);

	return comp;
}
