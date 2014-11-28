CXX = g++.exe
CXXFLAGS ?= -g -Wall

GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always)

ifeq ($(OS),Windows_NT)
	MV ?= move
	CP ?= copy /Y
	RM ?= del /Q /F
else
	MV ?= mv -f
	CP ?= cp -f
	RM ?= rm -rf
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
	lib_src = $(subst,/,\ $(_lib_src))
	test_src = $(subst,/,\ $(_test_src))
	lib_prog = $(subst,/,\ $(_lib_prog))
	test_prog = $(subst,/,\ $(_test_prog))
else
	lib_src = _lib_src
	test_src = _test_src
	lib_prog = _lib_prog
	test_prog = _test_prog
endif

lib_objects = $(subst,.cpp,.o $(lib_src))
test_objects = $(subst,.cpp,.o $(test_src))

all: $(tool) $(test)

tool: $(tool)
test: $(test)
	ifeq ($(OS),Windows_NT)
		$(test)
	else
		./$(test)
	endif

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
	@$(RM) $(foreach,f,$(tool) "$f")
	@$(RM) $(foreach,f,$(test) "$f")
	@$(RM) $(foreach,f,$(lib_objects) "$f")
	@$(RM) $(foreach,f,$(test_objects) "$f")

.PHONY: clean test tool all