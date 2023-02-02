OBJ_DIR=objectFiles
INC_DIR=include
SRC_DIR=src
LIB_DIR=lib
BIN_DIR=bin

all : directory libSocketHandler stateMachine

libSocketHandler.o : $(LIB_DIR)/libSocketHandler.c
	gcc -c $(LIB_DIR)/libSocketHandler.c -o $(OBJ_DIR)/libSocketHandler.o
libSocketHandler : libSocketHandler.o
	ar rcs $(BIN_DIR)/libSocketHandler.a $(OBJ_DIR)/libSocketHandler.o
stateMachine.o : $(SRC_DIR)/stateMachine.c
	gcc -c $(SRC_DIR)/stateMachine.c -o $(OBJ_DIR)/stateMachine.o
stateMachine : stateMachine.o
	gcc $(OBJ_DIR)/stateMachine.o -o $(BIN_DIR)/stateMachine -L$(BIN_DIR) -lSocketHandler
directory :
	mkdir -p $(OBJ_DIR) $(BIN_DIR)
clean :
	rm -f $(OBJ_DIR)/*.o
	rm -f $(BIN_DIR)/*

