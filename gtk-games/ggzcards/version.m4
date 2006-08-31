GGZCARDS_VERSION=0.0.7

GGZCARDS_PROTOCOL_ENGINE=GGZCards
GGZCARDS_PROTOCOL_VERSION=16

AC_SUBST(GGZCARDS_VERSION)
AC_SUBST(GGZCARDS_PROTOCOL_ENGINE)
AC_SUBST(GGZCARDS_PROTOCOL_VERSION)

AC_DEFINE_UNQUOTED([GGZCARDS_VERSION], "$GGZCARDS_VERSION", [GGZCards version])
AC_DEFINE_UNQUOTED([GGZCARDS_PROTOCOL_ENGINE], "$GGZCARDS_PROTOCOL_ENGINE",
		   [GGZCards protocol engine])
AC_DEFINE_UNQUOTED([GGZCARDS_PROTOCOL_VERSION], "$GGZCARDS_PROTOCOL_VERSION",
		   [GGZCards protocol version])
