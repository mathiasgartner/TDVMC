# TDVMC
TDVMC - Time Dependent Variational Monte Carlo

## resources
use json parser from https://github.com/open-source-parsers/jsoncpp

## Makefile for zusie server
CXX      := mpicxx
CXXFLAGS := -O3 -std=c++11
LDFLAGS  := -lmpi -L./libs -llapack -lrefblas -lgfortran -lpthread
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)
TARGET   := TDVMC
INCLUDE  :=
SRC      := $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(wildcard resources/*.cpp)

OBJECTS := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
        @mkdir -p $(@D)
        $(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

$(APP_DIR)/$(TARGET): $(OBJECTS)
        @mkdir -p $(@D)
        $(CXX) $(CXXFLAGS) $(OBJECTS) -o $(APP_DIR)/$(TARGET) $(INCLUDE) $(LDFLAGS)

.PHONY: all build clean

build:
        @mkdir -p $(APP_DIR)
        @mkdir -p $(OBJ_DIR)

clean:
        -@rm -rf $(OBJ_DIR)/*
        -@rm -rf $(APP_DIR)/*
