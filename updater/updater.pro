#  updater/updater.pro
#  Copyright (C) 2008 John Eric Martin <john.eric.martin@gmail.com>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2 as
#  published by the Free Software Foundation.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the
#  Free Software Foundation, Inc.,
#  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

QT          += network webkit xml
TEMPLATE     = app
TARGET       = megatendb_updater
INCLUDEPATH += src

OBJECTS_DIR = .objs
MOC_DIR     = .moc
RCC_DIR     = .rcc
UI_DIR      = .ui

RESOURCES    = \#updater.rc \
               updater.qrc
FORMS       += ui/PaletteEditor.ui \
               ui/Updater.ui
HEADERS     += src/HttpTransfer.h \
               src/PaletteEditor.h \
               src/Updater.h
SOURCES     += src/main.cpp \
               src/sha1.c \
               src/HttpTransfer.cpp \
               src/PaletteEditor.cpp \
               src/Updater.cpp
