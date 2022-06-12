TARGET = h3 
CC = gcc
SRC = $(shell find -name '*.c')
LIB = -lwiringPi -lwiringPiDev -lpthread

objects = $(SRC:.c=.o)

$(TARGET):$(objects)
	$(CC) $^ -g -o $@ $(LIB)  

$(object):$(SRC)

.PHONY:clean
clean:
	-rm -f $(objects) $(TARGET)
