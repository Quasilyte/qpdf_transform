CXX=g++
CXXFLAGS=-O2 -std=gnu++0x -Wall -Wextra

EXECUTABLE_NAME=bin/qpdf_transform

INCLUDES=\
    -I/usr/local/include/qpdf/ \
    -I/usr/share/include/qpdf/

SOURCE_FILES=\
    src/page_number_stream.cpp \
    src/rotation.cpp \
    src/utils.cpp \
    src/qpdf_transform.cpp \
    src/rotation_arg.cpp

LIBS=-lqpdf -lz -lpcre

all:
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCE_FILES) $(LIBS) -o $(EXECUTABLE_NAME)
