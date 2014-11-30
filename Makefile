CXX = g++
CXXFLAGS = -g -Wall

GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always)

ifeq ($(OS),Windows_NT)
	MV ?= move
	CP ?= copy /Y
	RM ?= del /Q /F
	RM = rm -f
else
	MV ?= mv -f
	CP ?= cp -f
	RM ?= rm -f
endif

_lib_src = json.cpp \
           sourcemap.cpp \

_test_src = UnitTest++/src/AssertException.cpp \
            UnitTest++/src/Test.cpp \
            UnitTest++/src/Checks.cpp \
            UnitTest++/src/TestRunner.cpp \
            UnitTest++/src/TestResults.cpp \
            UnitTest++/src/TestReporter.cpp \
            UnitTest++/src/TestReporterStdout.cpp \
            UnitTest++/src/ReportAssert.cpp \
            UnitTest++/src/TestList.cpp \
            UnitTest++/src/TimeConstraint.cpp \
            UnitTest++/src/TestDetails.cpp \
            UnitTest++/src/MemoryOutStream.cpp \
            UnitTest++/src/DeferredTestReporter.cpp \
            UnitTest++/src/DeferredTestResult.cpp \
            UnitTest++/src/XmlTestReporter.cpp \
            UnitTest++/src/CurrentTest.cpp

_lib_prog = tool/cli.cpp
_test_prog = test/test.cpp

ifeq ($(OS),Windows_NT)
	_test_src += UnitTest++/src/Win32/TimeHelpers.cpp
else
	_test_src += UnitTest++/src/Posix/TimeHelpers.cpp \
	             UnitTest++/src/Posix/SignalTranslator.cpp
endif

tool = sourcemap
test = testsuite
ifeq ($(OS),Windows_NT)
	tool = sourcemap.exe
	test = testsuite.exe
	testsuite = $(test)
	lib_src = $(subst,/,\ $(_lib_src))
	test_src = $(subst,/,\ $(_test_src))
	lib_prog = $(subst,/,\ $(_lib_prog))
	test_prog = $(subst,/,\ $(_test_prog))
else
	testsuite = ./$(test)
	lib_src = $(_lib_src)
	test_src = $(_test_src)
	lib_prog = $(_lib_prog)
	test_prog = $(_test_prog)
endif

lib_objects = $(lib_src:.cpp=.o)
test_objects = $(test_src:.cpp=.o)

all: $(tool) $(test)

tool: $(tool)
test: $(test)
	@$(testsuite)

%.o: %.cpp
	@echo compile $<
	@$(CXX) $(CXXFLAGS) $(EXTRA_CFLAGS) -DVERSION="\"$(GIT_VERSION)\"" -c $< -o $@

$(tool): $(lib_prog) $(lib_objects)
	@echo link $@
	@$(CXX) $(CXXFLAGS) $(EXTRA_CFLAGS) -DVERSION="\"$(GIT_VERSION)\"" $(EXTRA_LDFLAGS) -o $(tool) $(lib_prog) $(lib_objects)

$(test): $(test_prog) $(lib_objects) $(test_objects)
	@echo link $@
	@$(CXX) $(CXXFLAGS) $(EXTRA_CFLAGS) -DVERSION="\"$(GIT_VERSION)\"" $(EXTRA_LDFLAGS) -o $(test) $(test_prog) $(lib_objects) $(test_objects)

clean:
	@$(RM) $(tool)
	@$(RM) $(test)
	@$(RM) $(lib_objects)
	@$(RM) $(test_objects)

.PHONY: clean test tool all