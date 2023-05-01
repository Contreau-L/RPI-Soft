

OBJ_DIR=objectFiles
INC_DIR=include
SRC_DIR=src
LIB_DIR=lib
BIN_DIR=bin
CURRENT_DIRECTORY=$(shell pwd)

all: directory $(BIN_DIR)/contreaul

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.c $(INC_DIR)/%.h
	gcc -fPIC -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/%.h
	gcc -c $< -o $@

$(BIN_DIR)/lib%.so: $(OBJ_DIR)/lib%.o 
	gcc -shared -o $@ $<

$(BIN_DIR)/contreaul: $(OBJ_DIR)/contreaul.o $(OBJ_DIR)/stateMachine.o $(OBJ_DIR)/sensorManager.o $(OBJ_DIR)/actuatorManager.o \
                     $(BIN_DIR)/libUtils.so $(BIN_DIR)/libSharedMemory.so $(BIN_DIR)/libSocketHandler.so $(BIN_DIR)/libSensorManager.so $(BIN_DIR)/libI2C.so \
					 $(BIN_DIR)/libActuatorManager.so $(BIN_DIR)/libStateMachine.so
	gcc $^ -o $@ -L$(BIN_DIR) -lUtils -lSharedMemory -lSocketHandler -lpthread -lcjson -lSensorManager -lI2C -lwiringPi -lActuatorManager -lStateMachine -Wl,-rpath,$(CURRENT_DIRECTORY)/$(BIN_DIR)

directory:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(BIN_DIR)/*