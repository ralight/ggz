project(kryds)

set(kryds_SRCS
	kryds.cpp
	tictactoe_server.cpp
	main.cpp
)

kde4_add_executable(kryds ${kryds_SRCS})

target_link_libraries(kryds ${KDE4_KDECORE_LIBS} -L../lib -lkggzdmod kggznet)

install(TARGETS kryds ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES kryds.dsc DESTINATION ${GGZ_GAMEDESC_INSTALL_DIR})
install(FILES kryds.room DESTINATION ${GGZ_ROOMDESC_INSTALL_DIR})
