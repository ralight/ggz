set(kgrubby_SRCS
	main.cpp
	toplevel.cpp
	app.cpp
	plugin.cpp)

kde4_add_executable(kgrubby ${kgrubby_SRCS})

kde4_add_app_icon(kgrubby_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../data/hi*-app-kgrubby.png")

target_link_libraries(kgrubby ${KDE4_KDEUI_LIBS})

install(TARGETS kgrubby ${INSTALL_TARGETS_DEFAULT_ARGS})
