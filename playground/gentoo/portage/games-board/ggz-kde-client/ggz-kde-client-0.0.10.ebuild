# Copyright 1999-2004 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header$

inherit kde

DESCRIPTION="These are the kde versions of the games made by GGZ Gaming Zone"
HOMEPAGE="http://www.ggzgamingzone.org/"
SRC_URI="http://ftp.ggzgamingzone.org/pub/ggz/${PV}/${P}.tar.gz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~x86 ~ppc ~sparc ~amd64"
IUSE=""

DEPEND="=dev-games/ggz-client-libs-0.0.10
	=dev-games/libggz-0.0.10
	>=kde-base/kdelibs-3.0.0
	>=x11-libs/qt-3.0.0"

need-kde 3

