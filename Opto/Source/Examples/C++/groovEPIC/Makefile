
TOP_DIR=../../../..
SOURCE_DIR=$(TOP_DIR)/Source
OUTPUT_DIR=$(TOP_DIR)/Output

CXXFLAGS = -I$(SOURCE_DIR) -Wall -D_LINUX -g
LDFLAGS = -pthread

SOURCES=groovEPIC.cpp
OBJECTS=$(patsubst %.cpp,%.o,${SOURCES})

all: $(OUTPUT_DIR)/groovEPIC

$(OUTPUT_DIR)/groovEPIC: $(OUTPUT_DIR)/liboptommp.a ${OBJECTS}
	$(CXX) $(LDFLAGS) -o $@ ${OBJECTS} $(OUTPUT_DIR)/liboptommp.a

clean:
	rm -f *.o $(OUTPUT_DIR)/groovEPIC

