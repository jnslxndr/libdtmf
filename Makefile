#### Simple Makefile for Simple Projects ####

# Just declare the Project name
PRODUCT = libdtmf.a

SRC=src

BIN_DIR=bin/dtmf
LIB_DIR=$(BIN_DIR)/lib
INCLUDE_DIR=$(BIN_DIR)/include
OUT = $(BIN_DIR)/$(PRODUCT)

REMOVE = rm -rdf

AR=ar rcs

CFLAGS=-Wall -Wstrict-prototypes
# CPPFLAGS=

CPPFILES=$(wildcard $(SRC)/*.cpp)
CCFILES=$(wildcard $(SRC)/*.cc)
CFILES=$(wildcard $(SRC)/*.c)

OBJECTS=$(CFILES:.c=.o) \
	$(CPPFILES:.cpp=.o)\
	$(CCFILES:.cc=.o) \
	$(CCFILES:.m=.o)

.PHONY: clean all

default: all

all: $(PRODUCT)

# test: clean all
# 	./$(BIN_DIR)/$(PRODUCT)
 
clean:
	$(REMOVE) $(BIN_DIR)
	$(REMOVE) $(OBJECTS)

$(PRODUCT): $(OBJECTS)
	mkdir -p $(BIN_DIR)
	mkdir -p $(LIB_DIR)
	mkdir -p $(INCLUDE_DIR)
	$(AR) $(LIB_DIR)/$(PRODUCT) $(OBJECTS)
	cp src/dtmf.h $(INCLUDE_DIR)

#### Generating object files ####
# object from C
.c.o: 
	$(CC) $(CFLAGS) -c $< -o $@

# object from C++ (.cc, .cpp)
.cc.o .cpp.o :
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
