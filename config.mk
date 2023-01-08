# JPC note!
# Libs needed for Ubuntu synaptic / apt:
#
#  * libx11-dev
#  * libxft-dev
#  * libfonconfig1-dev                        --> really needed?
#  * libfreetype6-dev   (which is freetype2)  --> really needed?

# dwm version
VERSION = 6.1

# Customize below to fit your system

# paths
PREFIX = ${HOME}/install-prefix
MANPREFIX = ${PREFIX}/share/man

X11INC = /usr/include
X11LIB = /usr/X11R6/lib

# Xinerama, comment if you don't want it
#XINERAMALIBS  = -lXinerama
#XINERAMAFLAGS = -DXINERAMA

# freetype
FREETYPELIBS = -lfontconfig -lXft
#FREETYPEINC = /tool/pandora64/.package/freetype-2.4.4/include/freetype2
# OpenBSD (uncomment)
FREETYPEINC = ${X11INC}/freetype2

# includes and libs
INCS = -I${X11INC} -I${FREETYPEINC}
LIBS = -L${X11LIB} -lX11 ${XINERAMALIBS} ${FREETYPELIBS}

# flags
CPPFLAGS = -D_BSD_SOURCE -D_POSIX_C_SOURCE=2 -DVERSION=\"${VERSION}\" ${XINERAMAFLAGS}
#CFLAGS   = -g -std=c99 -pedantic -Wall -O0 ${INCS} ${CPPFLAGS}
CFLAGS   = -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Os ${INCS} ${CPPFLAGS}
LDFLAGS  = -s ${LIBS}

# Solaris
#CFLAGS = -fast ${INCS} -DVERSION=\"${VERSION}\"
#LDFLAGS = ${LIBS}

# compiler and linker
CC = cc
CXX = g++
