CONFIG += link_pkgconfig
PKGCONFIG += xcomposite xdamage
LIBS += -lx11mirror -L../

SOURCES = main.cpp mirror.cpp
HEADERS = mirror.hpp
