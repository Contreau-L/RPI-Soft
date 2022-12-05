OBJ_DIR=objectFiles
INC_DIR=include
SRC_DIR=src
LIB_DIR=lib
BIN_DIR=bin

all : directory

directory :
	mkdir -p $(OBJ_DIR) $(BIN_DIR)
clean :
	rm -f $(OBJ_DIR)/*.o
	rm -f $(BIN_DIR)/*

