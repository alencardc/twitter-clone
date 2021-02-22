CC = g++
CPPFLAGS = -Wall
LFLAGS = -Wall -g

SRC_DIR = ./src
OBJ_DIR = ./build
BIN_DIR = ./bin

SERVER_SRC := $(wildcard $(SRC_DIR)/server/*.cpp)
SERVER_OBJ := $(patsubst $(SRC_DIR)/server/%.cpp, $(OBJ_DIR)/server/%.o, $(SERVER_SRC))
CLIENT_SRC := $(wildcard $(SRC_DIR)/client/*.cpp)
CLIENT_OBJ := $(patsubst $(SRC_DIR)/client/%.cpp, $(OBJ_DIR)/client/%.o, $(CLIENT_SRC))


all: server client

server: $(SERVER_OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) $(LFLAGS) -o $(BIN_DIR)/app_server $^

$(SERVER_OBJ): $(SERVER_SRC)#
	mkdir -p $(OBJ_DIR)/server
	$(CC) $(CFLAGS) -o $@ -c $<

client: $(CLIENT_OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) $(LFLAGS) -o $(BIN_DIR)/app_client $^

$(CLIENT_OBJ): $(CLIENT_SRC)
	mkdir -p $(OBJ_DIR)/client
	$(CC) $(CFLAGS) -o $@ -c $<
	
clean:
	rm -rf *.o $(OBJ_DIR) $(BIN_DIR)
