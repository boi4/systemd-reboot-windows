CPP = g++
CPPFLAGS = -Wall -Werror



console.exe: console.cc systemdvars-windows.cc systemdvars-linux.cc systemdvars-common.cc util-windows.cc util-linux.cc util-common.cc
	${CPP} ${CPPFLAGS} -o $@ $^
