CXX=g++
CXXFLAGS=-O2 -std=gnu++0x -Wall -Wextra

EXECUTABLE_NAME=bin/qpdf_transform

SOURCE_FILES=$(shell find src -name '*.cpp')

LIBS=-lqpdf -lz -lpcre
ADDITIONAL_LIBS=$(shell find lib -name '*.so')

INCLUDES=\
    -I/usr/local/include/qpdf/ \
    -I/usr/share/include/qpdf/ \
    -Iinclude

LIB_DIRS=\
    -L/usr/lib64/ \
    -L/usr/lib/ \
    -L/usr/local/lib/ \
    -L/usr/share/lib

all:
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIB_DIRS) $(SOURCE_FILES) $(LIBS) $(ADDITIONAL_LIBS) -o $(EXECUTABLE_NAME)
