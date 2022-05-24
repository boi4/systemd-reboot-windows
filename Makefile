CPP = g++
CPPFLAGS = -Wall -Werror

console.exe: console.cc systemdvars.cc
	${CPP} ${CPPFLAGS} -o $@ $^