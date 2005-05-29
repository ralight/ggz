# Copyright 1999-2004 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header$

DESCRIPTION="These are the gtk versions of the games made by GGZ Gaming Zone"
HOMEPAGE="http://www.ggzgamingzone.org/"
SRC_URI="http://ftp.belnet.be/packages/ggzgamingzone/ggz/${PV}/${P}.tar.gz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~x86 ~ppc ~sparc ~amd64"
IUSE=""

inherit ggz

DEPEND="=dev-games/libggz-0.0.11
	=dev-games/ggz-client-libs-0.0.11
	>=libsdl-1.2.0
	>=sdl-image-1.2.0
	>=sdl-ttf-1.2.0"

pkg_setup() {
	check_ggz_modules
}

src_compile() {
	fix_ggz_module_makefiles "geekgame ttt3d"
	econf || die
	emake || die
}

src_install() {
	make DESTDIR=${D} install || die
	dodoc AUTHORS COPYING ChangeLog INSTALL NEWS \
		QuickStart.GGZ README* TODO
	
	install_ggz_modules "geekgame ttt3d"
}

pkg_prerm() {
	remove_ggz_modules "geekgame ttt3d"
}

