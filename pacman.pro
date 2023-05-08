SRC_DIR = src
SOURCES = $${SRC_DIR}/*.cpp
HEADERS = $${SRC_DIR}/*.h

CONFIG += warn_on
CONFIG += c++17
CONFIG += debug

TARGET = pacman

# Cíle specifikované zadáním pro makefile
dox.target = doxygen
dox.commands = doxygen Doxyfile;
run.target = run
run.commands = ./pacman;
pack.target = pack
pack.commands = tar -cvzf xvolfr00-xuherp02.tgz src/* examples/* doc/* sprites/* README.txt Makefile pacman.pro

QMAKE_EXTRA_UNIX_TARGETS += dox run pack

QT += core widgets xml
