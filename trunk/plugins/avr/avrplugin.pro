 ###########################################################################
 #   Copyright (C) 2012   by santiago González                             #
 #   santigoro@gmail.com                                                   #
 #                                                                         #
 #   This program is free software; you can redistribute it and/or modify  #
 #   it under the terms of the GNU General Public License as published by  #
 #   the Free Software Foundation; either version 3 of the License, or     #
 #   (at your option) any later version.                                   #
 #                                                                         #
 #   This program is distributed in the hope that it will be useful,       #
 #   but WITHOUT ANY WARRANTY; without even the implied warranty of        #
 #   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
 #   GNU General Public License for more details.                          #
 #                                                                         #
 #   You should have received a copy of the GNU General Public License     #
 #   along with this program; if not, see <http://www.gnu.org/licenses/>.  #
 #                                                                         #
 ###########################################################################


PLGN_DIR_DATA = $$PWD/resources/data
PLGN_DIR_EXAMPLES = $$PWD/resources/examples

include( ../commonplugin.pro )

TARGET  = $$qtLibraryTarget(avrplugin)

SOURCES =   $$PWD/src/*.cpp \
            $$PWD/src/simavr/sim/*.c \
            $$PWD/src/simavr/cores/*.c \
            $$PWD/avrplugin.cpp \
            
HEADERS =   $$PWD/src/*.h \
            $$PWD/src/simavr/sim/*.h \
            $$PWD/src/simavr/sim/avr/*.h  \
            $$PWD/src/simavr/cores/*.h \
            avrplugin.h

INCLUDEPATH += src \
               src/simavr \
               src/simavr/sim \
               src/simavr/sim/avr \
               src/simavr/cores \
               src/simavr/cores/avr
               
RESOURCES = $$PWD/src/resources.qrc

#LIBS  += ../build_$$_ARCH$$_BITS/lib/libsimavr.a

isEqual( _ARCH,"Lin") {
    QMAKE_LIBS += -lutil
}
isEqual( _ARCH,"Win") {

    INCLUDEPATH += build-$$_ARCH$$_BITS/include
    QMAKE_LFLAGS += "build_$$_ARCH$$_BITS/lib"
    QMAKE_LIBS += -lsimavr -lelf -lws2_32 -lwsock32 -lglibc_win
}

QMAKE_LIBS += -ldl
