SRC_DIR = src
SOURCES = $${SRC_DIR}/*.cpp
HEADERS = $${SRC_DIR}/*.h

CONFIG += warn_on
CONFIG += c++17

TARGET = pacman

QT += widgets
