TAR = ./bin/Data_collect
OBJ = ./src/*.cpp
INC = ./inc/*.h
CC = g++

all:
	$(CC) $(OBJ) $(INC) -pthread -o $(TAR)
clean:
	rm -rf $(TAR)