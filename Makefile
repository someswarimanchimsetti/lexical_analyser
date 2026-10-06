CC = gcc
CFLAGS = -Wall -Wextra -Werror

TARGET = lexical_analyser

SRC = main.c lexical_analyser.c
OBJ = main.o lexical_analyser.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

main.o: main.c lexical_analyser.h
	$(CC) $(CFLAGS) -c main.c

lexical_analyser.o: lexical_analyser.c lexical_analyser.h
	$(CC) $(CFLAGS) -c lexical_analyser.c

clean:
	rm -f $(OBJ) $(TARGET)