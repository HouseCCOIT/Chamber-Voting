# Top-level Makefile for the OptoMMPToolkit along with the example programs
# that can be run on a Posix system.

CXXFLAGS = -I$(SRC_DIR) -Wall -D_LINUX -g

LIB_SOURCES=$(wildcard Source/*.cpp)
LIB_OBJECTS=$(patsubst %.cpp,%.o,${LIB_SOURCES})

all: build Output/liboptommp.a eiocl eiocon digital_bank learningcenter groovEPIC

build:
	@mkdir -p Output

Output/liboptommp.a: ${LIB_OBJECTS}
	ar rcs $@ ${LIB_OBJECTS}
	ranlib $@

clean:
	rm -rf Output ${LIB_OBJECTS}
	${MAKE} -C Source/Examples/C++/eiocl clean
	${MAKE} -C Source/Examples/C++/eiocon clean
	${MAKE} -C Source/Examples/C++/DigitalBankConsole clean
	${MAKE} -C Source/Examples/C++/LearningCenter clean
	${MAKE} -C Source/Examples/C++/groovEPIC clean

eiocl:
	${MAKE} -C Source/Examples/C++/eiocl

eiocon:
	${MAKE} -C Source/Examples/C++/eiocon

digital_bank:
	${MAKE} -C Source/Examples/C++/DigitalBankConsole

learningcenter:
	${MAKE} -C Source/Examples/C++/LearningCenter

groovEPIC:
	${MAKE} -C Source/Examples/C++/groovEPIC

Source/O22SIOMM.o: Source/O22SIOMM.cpp Source/O22SIOMM.h Source/O22SIOUT.h Source/O22STRCT.h

Source/O22SIOST.o: Source/O22SIOST.cpp Source/O22SIOST.h Source/O22SIOUT.h Source/O22STRCT.h

Source/O22SIOUT.o: Source/O22SIOUT.cpp Source/O22SIOUT.h

