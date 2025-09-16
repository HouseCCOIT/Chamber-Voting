
TOP_DIR=../../../..
SOURCE_DIR=$(TOP_DIR)/Source
OUTPUT_DIR=$(TOP_DIR)/Output

CXXFLAGS = -I$(SOURCE_DIR) -Wall -D_LINUX -g

SOURCES=$(wildcard *.cpp)
OBJECTS=$(patsubst %.cpp,%.o,${SOURCES})

all: $(OUTPUT_DIR)/eiocl

$(OUTPUT_DIR)/eiocl: $(OUTPUT_DIR)/liboptommp.a ${OBJECTS}
	$(CXX) -o $@ ${OBJECTS} $(OUTPUT_DIR)/liboptommp.a

clean:
	rm -f *.o $(OUTPUT_DIR)/eiocl

