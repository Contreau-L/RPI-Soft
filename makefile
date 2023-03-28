OBJ_DIR=objectFiles
INC_DIR=include
SRC_DIR=src
LIB_DIR=lib
BIN_DIR=bin
CURRENT_DIRECTORY=$(shell pwd)
all : directory libSocketHandler libUtils libSensorManager libActuatorManager libSharedMemory sensorManager.o actuatorManager.o stateMachine.o contreaul
libSocketHandler.o : $(LIB_DIR)/libSocketHandler.c
	gcc -fPIC -c $(LIB_DIR)/libSocketHandler.c -o $(OBJ_DIR)/libSocketHandler.o
libSocketHandler : libSocketHandler.o
	gcc -shared -o $(BIN_DIR)/libSocketHandler.so $(OBJ_DIR)/libSocketHandler.o
libUtils.o : $(LIB_DIR)/libUtils.c
	gcc -fPIC -c $(LIB_DIR)/libUtils.c -o $(OBJ_DIR)/libUtils.o
libUtils : libUtils.o
	gcc -shared -o $(BIN_DIR)/libUtils.so $(OBJ_DIR)/libUtils.o
libSensorManager.o : $(LIB_DIR)/libSensorManager.c
	gcc -fPIC -c $(LIB_DIR)/libSensorManager.c -o $(OBJ_DIR)/libSensorManager.o
libSensorManager : libSensorManager.o
	gcc -shared -o $(BIN_DIR)/libSensorManager.so $(OBJ_DIR)/libSensorManager.o
libActuatorManager.o : $(LIB_DIR)/libActuatorManager.c
	gcc -fPIC -c $(LIB_DIR)/libActuatorManager.c -o $(OBJ_DIR)/libActuatorManager.o
libActuatorManager : libActuatorManager.o
	gcc -shared -o $(BIN_DIR)/libActuatorManager.so $(OBJ_DIR)/libActuatorManager.o
libSharedMemory.o : $(LIB_DIR)/libSharedMemory.c
	gcc -fPIC -c $(LIB_DIR)/libSharedMemory.c -o $(OBJ_DIR)/libSharedMemory.o
libSharedMemory : libSharedMemory.o
	gcc -shared -o $(BIN_DIR)/libSharedMemory.so $(OBJ_DIR)/libSharedMemory.o
sensorManager.o : $(SRC_DIR)/sensorManager.c
	gcc -c $(SRC_DIR)/sensorManager.c -o $(OBJ_DIR)/sensorManager.o
actuatorManager.o : $(SRC_DIR)/actuatorManager.c
	gcc -c $(SRC_DIR)/actuatorManager.c -o $(OBJ_DIR)/actuatorManager.o
stateMachine.o : $(SRC_DIR)/stateMachine.c
	gcc -c $(SRC_DIR)/stateMachine.c -o $(OBJ_DIR)/stateMachine.o
contreaul.o : $(SRC_DIR)/contreaul.c
	gcc -c $(SRC_DIR)/contreaul.c -o $(OBJ_DIR)/contreaul.o
contreaul : contreaul.o
	gcc $(OBJ_DIR)/contreaul.o $(OBJ_DIR)/stateMachine.o $(OBJ_DIR)/sensorManager.o $(OBJ_DIR)/actuatorManager.o -o $(BIN_DIR)/contreaul -L$(BIN_DIR) -lUtils -lSharedMemory -lSocketHandler -lpthread -Wl,-rpath,$(CURRENT_DIRECTORY)/$(BIN_DIR)
directory :
	mkdir -p $(OBJ_DIR) $(BIN_DIR)
clean :
	rm -f $(OBJ_DIR)/*.o
	rm -f $(BIN_DIR)/*

