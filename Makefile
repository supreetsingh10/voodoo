CXX=g++

SRC_DIR := ./src
INC_DIR := ./include
BUILD_DIR := ./build
BINARY := $(BUILD_DIR)/voodoo


SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
INCLUDE := $(shell find $(INC_DIR) -name '*.hpp')
OBJS := $(patsubst %.c, %.o, $(SRCS))

#CXX_FLAGS := -Wall -Wextra -g $(foreach D, $(INCLUDE), -I$(wildcard $(INC_D)/*.hpp))
CXX_FLAGS := -Wall -Wextra -g 

all: $(BINARY)
	echo $(BINARY)

$(BINARY): $(OBJS)
	$(CXX) $(CXX_FLAGS) -o $@ $^

clean: 
	rm -rf $(BUILD_DIR)/*

run: $(BINARY)
	./$(BINARY)

debug:
	gdb ./$(BINARY)
