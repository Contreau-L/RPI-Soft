OBJ_DIR=objectFiles
INC_DIR=include
SRC_DIR=src
LIB_DIR=lib
BIN_DIR=bin

all : directory libSocketHandler libUtils libSensorManager libActuatorManager libSharedMemory contreaul
libSocketHandler.o : $(LIB_DIR)/libSocketHandler.c
	gcc -c $(LIB_DIR)/libSocketHandler.c -o $(OBJ_DIR)/libSocketHandler.o
libSocketHandler : libSocketHandler.o
	ar rcs $(BIN_DIR)/libSocketHandler.a $(OBJ_DIR)/libSocketHandler.o
libUtils.o : $(LIB_DIR)/libUtils.c
	gcc -c $(LIB_DIR)/libUtils.c -o $(OBJ_DIR)/libUtils.o
libUtils : libUtils.o
	ar rcs $(BIN_DIR)/libUtils.a $(OBJ_DIR)/libUtils.o
libSensorManager.o : $(LIB_DIR)/libSensorManager.c
	gcc -c $(LIB_DIR)/libSensorManager.c -o $(OBJ_DIR)/libSensorManager.o
libSensorManager : libSensorManager.o
	ar rcs $(BIN_DIR)/libSensorManager.a $(OBJ_DIR)/libSensorManager.o
libActuatorManager.o : $(LIB_DIR)/libActuatorManager.c
	gcc -c $(LIB_DIR)/libActuatorManager.c -o $(OBJ_DIR)/libActuatorManager.o
libActuatorManager : libActuatorManager.o
	ar rcs $(BIN_DIR)/libActuatorManager.a $(OBJ_DIR)/libActuatorManager.o
libSharedMemory.o : $(LIB_DIR)/libSharedMemory.c
	gcc -c $(LIB_DIR)/libSharedMemory.c -o $(OBJ_DIR)/libSharedMemory.o
libSharedMemory : libSharedMemory.o
	ar rcs $(BIN_DIR)/libSharedMemory.a $(OBJ_DIR)/libSharedMemory.o
contreaul.o : $(SRC_DIR)/contreaul.c
	gcc -c $(SRC_DIR)/contreaul.c -o $(OBJ_DIR)/contreaul.o
contreaul : contreaul.o
	gcc $(OBJ_DIR)/contreaul.o -o $(BIN_DIR)/contreaul -L$(BIN_DIR) -lUtils -lSocketHandler  -lSensorManager -lActuatorManager -lSharedMemory -lpthread
directory :
	mkdir -p $(OBJ_DIR) $(BIN_DIR)
clean :
	rm -f $(OBJ_DIR)/*.o
	rm -f $(BIN_DIR)/*

