# Copyright 1999-2004 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header$

DESCRIPTION="These are the gtk versions of the games made by GGZ Gaming Zone"
HOMEPAGE="http://www.ggzgamingzone.org/"
SRC_URI="http://ftp.ggzgamingzone.org/pub/ggz/${PV}/${P}.tar.gz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~x86 ~ppc ~amd64 ~sparc"
IUSE="gtk2"

inherit ggz

DEPEND="=games-board/ggz-gtk-client-0.0.10
	gtk2? ( =x11-libs/gtk+-2* )
	!gtk2? ( =x11-libs/gtk+-1* )"

pkg_setup() {
	check_ggz_modules
}

src_compile() {
	fix_ggz_module_makefiles "chess chinese-checkers combat dots ggzcards hastings reversi spades tictactoe"
	myconf="--enable-gtk2"
	use gtk2 || myconf="--enable-gtk"

	econf ${myconf} || die
	emake || die
}

src_install() {
	make DESTDIR=${D} install || die
	dodoc AUTHORS ChangeLog INSTALL NEWS QuickStart.GGZ README* TODO

	install_ggz_modules "chess chinese-checkers combat dots ggzcards hastings reversi spades tictactoe"
}

pkg_prerm() {
	remove_ggz_modules "chess chinese-checkers combat dots ggzcards hastings reversi spades tictactoe"
}
