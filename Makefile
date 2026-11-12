CC = gcc
CFLAGS = -Wall -Wextra -O2 -g
TARGET = mypreprocessor
OBJS = main.o comment_removal.o file_inclusion.o macro_handler.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c file_inclusion.h
file_inclusion.o: file_inclusion.c file_inclusion.h comment_removal.h macro_handler.h
comment_removal.o: comment_removal.c comment_removal.h
macro_handler.o: macro_handler.c macro_handler.h

test: $(TARGET)
	./$(TARGET) test.c
	@echo "--- Extended source output (test.i) ---"
	@cat test.i

clean:
	rm -f $(OBJS) $(TARGET) *.i

.PHONY: all test clean