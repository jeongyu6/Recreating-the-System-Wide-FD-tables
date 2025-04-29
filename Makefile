#Reference: Week 5 Notes
CC = gcc
CFLAGS = -Wall -g -std=c99 

SRCS = main.c graph.c pid.c utility.c

OBJS = main.o graph.o pid.o utility.o

TARGET = program

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

utility.o: utility.c utility.h
	$(CC) $(CFLAGS) -c $< -o $@

pid.o: pid.c pid.h utility.h
	$(CC) $(CFLAGS) -c $< -o $@

graph.o: graph.c graph.h utility.h pid.h
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c pid.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all clean
clean:
	rm -f $(OBJS) $(TARGET)


