set(dot_LIB_SRCS
	dots.cpp
	vdots.cpp
	qdots.cpp
)

kde4_add_library(dot STATIC ${dot_LIB_SRCS})

target_link_libraries(dot ${KDE4_KDECORE_LIBS} ${QT_QTCORE_LIBRARY})

