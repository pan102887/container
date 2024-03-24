project := container

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin


EXE := ${BIN_DIR}/${project}
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)%.c, $(OBJ_DIR)%.o, $(SRC))

CC := gcc
CPPFLAGS := -Iinclude -MMD -MP # -I is a preprocessor flag, not a compiler flag
CFLAGS   := -Wall              # some warnings about bad code
LDFLAGS  := -Llib              # -L is a linker flag
LDLIBS   := -lm				   # Left empty if no libs are needed
OPTFLAGS := -O3
CSTANDARD := -std=c2x


.PHONY: all clean print test

all: $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(OPTFLAGS) ${CSTANDARD} $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

 

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

print:
	echo ${EXE} ${SRC} ${OBJ}