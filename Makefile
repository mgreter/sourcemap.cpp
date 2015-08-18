CC = gcc
CXX = g++
CXXFLAGS = -Wall -fopenmp -O2 -I src -I deps/cencode -I deps/json -I deps/UnitTest++/src

GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always)

# very simple checks for now
ifeq ($(USE_STD_TR1_SHARED_PTR),)
	ifeq ($(USE_STD_SHARED_PTR),)
		CXXFLAGS += -DUSE_STD_TR1_SHARED_PTR
	endif
endif

ifeq ($(OS),Windows_NT)
	MV ?= move
	CP ?= copy /Y
	RM ?= del /Q /F
else
	MV ?= mv -f
	CP ?= cp -f
	RM ?= rm -f
endif

_lib_src = deps/json/json.cpp \
           deps/cencode/cencode.c \
           deps/cencode/cdecode.c \
           src/map_line.cpp \
           src/map_col.cpp \
           src/mappings.cpp \
           src/pos_idx.cpp \
           src/pos_txt.cpp \
           src/format/v3.cpp \
           src/document.cpp

_test_src = deps/UnitTest++/src/AssertException.cpp \
            deps/UnitTest++/src/Test.cpp \
            deps/UnitTest++/src/Checks.cpp \
            deps/UnitTest++/src/TestRunner.cpp \
            deps/UnitTest++/src/TestResults.cpp \
            deps/UnitTest++/src/TestReporter.cpp \
            deps/UnitTest++/src/TestReporterStdout.cpp \
            deps/UnitTest++/src/ReportAssert.cpp \
            deps/UnitTest++/src/TestList.cpp \
            deps/UnitTest++/src/TimeConstraint.cpp \
            deps/UnitTest++/src/TestDetails.cpp \
            deps/UnitTest++/src/MemoryOutStream.cpp \
            deps/UnitTest++/src/DeferredTestReporter.cpp \
            deps/UnitTest++/src/DeferredTestResult.cpp \
            deps/UnitTest++/src/XmlTestReporter.cpp \
            deps/UnitTest++/src/CurrentTest.cpp

_lib_prog = tool/cli.cpp
_test_prog = test/test.cpp

ifeq ($(OS),Windows_NT)
	_test_src += deps/UnitTest++/src/Win32/TimeHelpers.cpp
else
	_test_src += deps/UnitTest++/src/Posix/TimeHelpers.cpp \
	             deps/UnitTest++/src/Posix/SignalTranslator.cpp
endif

tool = sourcemap
test = testsuite
ifeq ($(OS),Windows_NT)
	tool = sourcemap.exe
	test = testsuite.exe
	testsuite = $(test)
	lib_src = $(_lib_src)
	test_src = $(_test_src)
	lib_prog = $(_lib_prog)
	test_prog = $(_test_prog)
else
	testsuite = ./$(test)
	lib_src = $(_lib_src)
	test_src = $(_test_src)
	lib_prog = $(_lib_prog)
	test_prog = $(_test_prog)
endif

lib_objects_x = $(lib_src:.cpp=.o)
lib_objects = $(lib_objects_x:.c=.o)
test_objects_x = $(test_src:.cpp=.o)
test_objects = $(test_objects_x:.c=.o)
testsuite_objects_x = $(test_prog:.cpp=.o)
testsuite_objects = $(testsuite_objects_x:.c=.o)

CLEANUP_FILES = $(tool) $(test) $(lib_objects) $(test_objects) $(testsuite_objects)

all: $(tool) $(test)
	echo $(lib_src)

tool: $(tool)
test: $(test)
	@$(testsuite)

%.o: %.cpp
	@echo compile $<
	$(CXX) -std=c++0x $(CXXFLAGS) $(EXTRA_CFLAGS) -DVERSION="\"$(GIT_VERSION)\"" -c $< -o $@

%.o: %.c
	@echo compile $<
	$(CC) $(CXXFLAGS) $(EXTRA_CFLAGS) -DVERSION="\"$(GIT_VERSION)\"" -c $< -o $@

$(tool): $(lib_prog) $(lib_objects)
	@echo link $@
	$(CXX) -std=c++0x  $(CXXFLAGS) $(EXTRA_CFLAGS) -DVERSION="\"$(GIT_VERSION)\"" $(EXTRA_LDFLAGS) -o $(tool) $(lib_prog) $(lib_objects)

$(test): $(testsuite_objects) $(lib_objects) $(test_objects)
	@echo link $@
	@$(CXX) -std=c++0x  $(CXXFLAGS) $(EXTRA_CFLAGS) -DVERSION="\"$(GIT_VERSION)\"" $(EXTRA_LDFLAGS) -o $(test) $(testsuite_objects) $(lib_objects) $(test_objects)

clean:
	@$(RM) "src\\*.o" "src\\format\\*.o" "deps\\json\\*.o" "deps\\UnitTest++\\src\\*.o" "deps\\cencode\\*.o"  "test\\*.o" "*.exe"
	@$(RM) $(CLEANUP_FILES)

.PHONY: clean test tool all