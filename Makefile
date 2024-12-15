CC = gcc
CFLAGS = -Wall -Wextra -g

SRCS = main.c memory.c parser.c scheduler.c 
OBJS = main.o memory.o parser.o scheduler.o 
TARGET = memory_simulator

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Target for compiling only the target
memory: memory.o
	$(CC) $(CFLAGS) -o memory memory.o

format:
	find . -maxdepth 1 -name "*.c" -o -name "*.h" | xargs clang-format -i --style="{BasedOnStyle: Google, ColumnLimit: 80}"

clean:
	rm -f $(OBJS) $(TARGET) parser parser.o
