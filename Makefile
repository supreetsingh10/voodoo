CXX=g++

SRC_DIR := ./src
INC_DIR := ./include
BUILD_DIR := ./build
BINARY := $(BUILD_DIR)/voodoo


SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
INCLUDE := $(shell find $(INC_DIR) -name '*.hpp')
OBJS := $(patsubst %.c, %.o, $(SRCS))

CXX_FLAGS := -Wall -Wextra -g $(foreach D, $(INCLUDE), -I$(wildcard $(D)/*.hpp))

all: $(BINARY)
	echo $(BINARY)

$(BINARY): $(OBJS)
	$(CXX) -o $@ $^

clean: 
	rm -rf $(BUILD_DIR)/*

run: $(BINARY)
	./$(BINARY)
