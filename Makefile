CC=gcc
CFLAGS=-o build/handmade.exe -mwindows -Wall -g
LDFLAGS=-lole32 -luuid -lcomdlg32 -lwinmm
SRC_DIR=src
BUILD_DIR=build

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

all: $(BUILD_DIR)/handmade.exe

$(BUILD_DIR)/handmade.exe: $(OBJS)
	$(CC) $^ $(CFLAGS) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -g -c $< -o $@

clean:
	rm $(BUILD_DIR)/*.o $(BUILD_DIR)/handmade.exe
