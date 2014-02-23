SHARED_SOURCES= \
							 src/shared/network/socket.cpp \
							 src/shared/network/address.cpp
SERVER_SOURCES= \
							 src/server/main.cpp
CLIENT_SOURCES= \
							 src/client/main.cpp

OBJ_DIR=obj/
CLIENT_SOURCES:=$(CLIENT_SOURCES) $(SHARED_SOURCES)
CLIENT_CPP_OBJECTS=$(CLIENT_SOURCES:.cpp=.cpp.o)
CLIENT_OBJECTS=$(CLIENT_CPP_OBJECTS)
CLIENT_BIN_OBJECTS=$(foreach obj,$(CLIENT_OBJECTS), $(addprefix $(OBJ_DIR), $(obj)))

SERVER_SOURCES:=$(SERVER_SOURCES) $(SHARED_SOURCES)
SERVER_CPP_OBJECTS=$(SERVER_SOURCES:.cpp=.cpp.o)
SERVER_OBJECTS=$(SERVER_CPP_OBJECTS)
SERVER_BIN_OBJECTS=$(foreach obj,$(SERVER_OBJECTS), $(addprefix $(OBJ_DIR), $(obj)))

CLIENT_BIN=bin/client
SERVER_BIN=bin/server

CXX=g++
CXX_WARN=-Wall -Wextra -pedantic
CXX_NOWARN=
CXX_OPT=-ggdb
CXX_INCLUDE=-I src
CXX_FLAGS=-std=c++11 $(CXX_WARN) $(CXX_NOWARN) $(CXX_OPT) $(CXX_INCLUDE) -c

LD_LIBS=-lstdc++ -lrt -lpthread -lm -lc -lgcc

COLOR_OFF=$(shell tput sgr0)
COLOR_GREEN=$(shell tput setaf 2)
PREFIX=$(COLOR_GREEN)»»»$(COLOR_OFF)

.PHONY: all setup clean \
	client client_setup client_link \
	server server_setup server_link

.SILENT:

all: setup clean client server
	echo "$(PREFIX) Success"

setup:
	echo "$(PREFIX) Preparing"
	mkdir -p bin obj

clean: setup
	rm -f bin/client bin/server
	rm -rf $(OBJ_DIR)
	echo "$(PREFIX) Cleaned"

client: client_setup client_link
	echo "$(PREFIX) Done"

client_setup: setup
	echo "$(PREFIX) Building client"

client_link: setup $(CLIENT_OBJECTS)
	echo "  $(PREFIX) Linking"
	$(CXX) -o $(CLIENT_BIN) $(CLIENT_BIN_OBJECTS) $(LD_LIBS)

server: server_setup server_link
	echo "$(PREFIX) Done"

server_setup: setup
	echo "$(PREFIX) Building server"

server_link: setup $(SERVER_OBJECTS)
	echo "  $(PREFIX) Linking"
	$(CXX) -o $(SERVER_BIN) $(SERVER_BIN_OBJECTS) $(LD_LIBS)

%.cpp.o: %.cpp setup
	echo "  $(PREFIX) Compiling $<"
	mkdir -p $(addprefix $(OBJ_DIR),$(dir $<))
	$(CXX) $(CXX_FLAGS) $< -o $(addsuffix .o,$(addprefix $(OBJ_DIR), $<))
