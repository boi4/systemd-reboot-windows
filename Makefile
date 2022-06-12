CPP = g++
CPPFLAGS = -Wall -Werror -ggdb


LIB_SRC = $(wildcard src/lib/*.cc)

ifeq ($(OS),Windows_NT)
CONSOLE_TARGET = systemd-reboot-console.exe
else ifeq ($(shell uname -s),Linux)
CONSOLE_TARGET = systemd-reboot-console
endif
CONSOLE_SRC = src/console/console.cc


ifeq ($(OS),Windows_NT)
TRAY_TARGET = systemd-reboot-tray.exe
TRAY_FLAGS = -mwindows
else ifeq ($(shell uname -s),Linux)
TRAY_TARGET = systemd-reboot-tray
TRAY_FLAGS = $(shell pkg-config --cflags --libs appindicator3-0.1)
endif
TRAY_SRC = src/tray/tray.cc


.PHONY: all
all: ${CONSOLE_TARGET} ${TRAY_TARGET}

${CONSOLE_TARGET}: ${CONSOLE_SRC} ${LIB_SRC}
	${CPP} ${CPPFLAGS} -o $@ -I src $^

${TRAY_TARGET}: ${TRAY_SRC} ${LIB_SRC}
	${CPP} ${CPPFLAGS} ${TRAY_FLAGS} -o $@ -I src $^


src/tray/icon.h: src/tray/icon.ico
	echo '#pragma once' > src/tray/icon.h
	xxd -i src/tray/icon.ico > src/tray/icon.h
