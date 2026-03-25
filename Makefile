CC = cl65
LD = ld65
CFLAGS = -Os -c -t nes
LDFLAGS = -C nes/nes.cfg

SRC_DIR = src
ASM_DIR = nes
OBJ_DIR = obj
BIN_DIR = bin

TARGET = 65FC02.nes

SOURCES  = $(wildcard $(SRC_DIR)/*.c)
ASSEMBLY = $(wildcard $(ASM_DIR)/*.s)
OBJECTS  = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES)) $(patsubst $(ASM_DIR)/%.s, $(OBJ_DIR)/%.o, $(ASSEMBLY)) 

.PHONY:	all run info clean

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) nes.lib

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: $(ASM_DIR)/%.s | $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $@ $<

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

run: $(BIN_DIR)/$(TARGET)
	mesen $<

info: $(BIN_DIR)/$(TARGET)
	fcinfo -s $<

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)