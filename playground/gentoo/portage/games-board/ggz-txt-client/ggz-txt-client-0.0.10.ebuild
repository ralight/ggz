# Copyright 1999-2004 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header$

DESCRIPTION="The textbased client for GGZ Gaming Zone"
HOMEPAGE="http://www.ggzgamingzone.org/"
SRC_URI="http://ftp.ggzgamingzone.org/pub/ggz/${PV}/${P}.tar.gz"
LICENSE="GPL-2"
SLOT="0"
KEYWORDS="x86 ~ppc ~sparc ~amd64"
IUSE=""

inherit ggz

DEPEND="=dev-games/ggz-client-libs-0.0.10
	sys-libs/ncurses
	sys-libs/readline"

src_compile() {
	fix_ggz_module_makefiles "tttxt"
	econf || die
	emake || die
}

src_install() {
	make DESTDIR=${D} install || die
	dodoc AUTHORS ChangeLog INSTALL NEWS QuickStart.GGZ README* TODO

	install_ggz_modules "tttxt"
}

pkg_prerm() {
	remove_ggz_modules "tttxt"
}

