project(ggz-sample-server)

set(ggz-sample-server-c++_SRCS
	kryds.cpp
	tictactoe_server.cpp
	main.cpp
)

kde4_add_executable(ggz-sample-server-c++ ${ggz-sample-server-c++_SRCS})

target_link_libraries(ggz-sample-server-c++ ${KDE4_KDECORE_LIBS} kggzdmod kggznet)

install(TARGETS ggz-sample-server-c++ ${INSTALL_TARGETS_DEFAULT_ARGS})

#install(FILES ggz-sample-server-c++.dsc DESTINATION ${DATA_INSTALL_DIR}/...)
