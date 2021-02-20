CC = gcc
CFLAGS = -Wall
LFLAGS = -Wall -g
LIBS = 

ROOT = ./../..

SRC_DIR = .
OBJ_DIR = $(ROOT)/build

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))


all: $(OBJ_FILES)
	$(CC) $(LFLAGS) -o $@ $^ $(LIBS)
	
$(OBJ_DIR)/%.o: $(SRC_FILES)
	$(CC) $(CFLAGS) -o $@ $<

