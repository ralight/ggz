project(ggz-sample-client)

set(ggz-sample-client-c++_SRCS
	gamewin.cpp
	main.cpp
	net.cpp
)

kde4_add_executable(ggz-sample-client-c++ ${ggz-sample-client-c++_SRCS})

target_link_libraries(ggz-sample-client-c++ ${KDE4_KDEUI_LIBS} kggzcore kggzmod kggzlib kggzgames)

install(TARGETS ggz-sample-client-c++ ${INSTALL_TARGETS_DEFAULT_ARGS})

register_ggz_module(module.dsc)

