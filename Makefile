
GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always)

ifeq ($(OS),Windows_NT)
	MV ?= move
	CP ?= copy /Y
	RM ?= del /Q /F
	EXESUFFIX ?= .exe
	SUFFIX ?= 2>NULL
else
	MV ?= mv -f
	CP ?= cp -f
	RM ?= rm -rf
	EXESUFFIX ?=
	SUFFIX ?=
endif

all: sourcemap

json.o: json.cpp
	g++ -DVERSION="\"$(GIT_VERSION)\"" -Wall -c json.cpp

sourcemap.o: sourcemap.cpp
	g++ -DVERSION="\"$(GIT_VERSION)\"" -Wall -c sourcemap.cpp

sourcemap: sourcemap.o json.o
	g++ -DVERSION="\"$(GIT_VERSION)\"" -Wall -o sourcemap -I. tool/sourcemap.cpp json.o sourcemap.o

clean:
	ifeq ($(OS),Windows_NT)
		${RM} sourcemap.exe 2>NUL
		${RM} sourcemap.o 2>NUL
		${RM} json.o 2>NUL
	else
		${RM} sourcemap.o
		${RM} sourcemap
		${RM} json.o
	endif
