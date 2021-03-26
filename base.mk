# Variables that will be set by Makefile
TARGET_EXEC = app 
BUILD_DIR = ./build
SRC_DIRS = ./src
EXTRA_LIBS = 
########################################

SRCS := $(shell find $(SRC_DIRS) -name *.cpp)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

#INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_DIRS := ./src
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Compiler
CXX = g++
LDFLAGS = -pthread $(EXTRA_LIBS)
CPPFLAGS = $(INC_FLAGS) -MMD -MP
CXXFLAGS = -Wall

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p