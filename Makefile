
GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always)

ifeq ($(OS),Windows_NT)
	MV ?= move
	CP ?= copy /Y
	RM ?= del /Q /F
	EXESUFFIX = .exe
	SUFFIX = 2>NULL
	RMFIX = 2>NUL
else
	MV ?= mv -f
	CP ?= cp -f
	RM ?= rm -rf
	EXESUFFIX =
	SUFFIX =
endif

all: sourcemap$(EXESUFFIX)

cli.o: tool/cli.cpp
	g++ -DVERSION="\"$(GIT_VERSION)\"" $(EXTRA_CFLAGS) -Wall -c tool/cli.cpp

json.o: json.cpp
	g++ -DVERSION="\"$(GIT_VERSION)\"" $(EXTRA_CFLAGS) -Wall -c json.cpp

sourcemap.o: sourcemap.cpp
	g++ -DVERSION="\"$(GIT_VERSION)\"" $(EXTRA_CFLAGS) -Wall -c sourcemap.cpp

sourcemap$(EXESUFFIX): sourcemap.o json.o cli.o
	g++ -DVERSION="\"$(GIT_VERSION)\"" $(EXTRA_LDFLAGS) -Wall -o sourcemap cli.o json.o sourcemap.o

clean:
	$(RM) sourcemap.exe $(SUFFIX)
	$(RM) sourcemap.o $(SUFFIX)
	$(RM) sourcemap $(SUFFIX)
	$(RM) json.o $(SUFFIX)

.PHONY: clean