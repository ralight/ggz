# Copyright 1999-2004 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header$

inherit kde
inherit ggz

DESCRIPTION="These are the kde versions of the games made by GGZ Gaming Zone"
HOMEPAGE="http://www.ggzgamingzone.org/"
SRC_URI="http://ftp.belnet.be/packages/ggzgamingzone/ggz/${PV}/${P}.tar.gz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~x86 ~ppc ~sparc ~amd64"
IUSE=""

DEPEND="=games-board/ggz-kde-client-0.0.11"

need-kde 3

pkg_setup() {
	check_ggz_modules
}

src_compile() {
	fix_ggz_module_makefiles "fyrdman kcc kdots keepalive koenig krosswater ktictactux muehle"
	econf || die
	emake || die
}

src_install() {
	make DESTDIR=${D} install || die
	dodoc AUTHORS ChangeLog INSTALL NEWS QuickStart.GGZ README* TODO

	install_ggz_modules "fyrdman kcc kdots keepalive koenig krosswater ktictactux muehle"
}

pkg_prerm() {
	remove_ggz_modules "fyrdman kcc kdots keepalive koenig krosswater ktictactux muehle"
}

