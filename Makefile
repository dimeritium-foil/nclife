CC = gcc
CFLAGS = -Wall 
LDFLAGS = -lncursesw 

SRC_DIR = src
VPATH = src

OBJFILES = $(patsubst $(SRC_DIR)/%.c, $(SRC_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))
TARGET = nclife

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c $^

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJFILES)
