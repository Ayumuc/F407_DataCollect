INC = -I${HOME}/inc/*.h
BIN = ${HOME}/bin/radio
SRC = ${HOME}/src/*cpp

build:aarch64-linux-gnu-g++ ${SRC} ${INC} -pthread -o ${BIN}
