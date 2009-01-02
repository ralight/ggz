#include "util.h"

#include "ggzprofile.h"

#include <kconfiggroup.h>

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
