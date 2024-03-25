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


# ======== test ========
TEST_DIR := test
TEST_PROJECT := container_test
TEST_OBJ_DIR := ${TEST_DIR}/obj
TEST_BIN_DIR := ${TEST_DIR}/bin

TEST_EXE := ${TEST_BIN_DIR}/${TEST_PROJECT}
TEST_OBJ := $(patsubst ${SRC_DIR}/%.c, $(TEST_OBJ_DIR)/%.o, $(SRC))
TEST_FLAGES := -D__TEST__ -g


.PHONY: all test clean print 

all: $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(OPTFLAGS) ${CSTANDARD} $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

test: $(TEST_EXE)

$(TEST_EXE): $(TEST_OBJ) | $(TEST_BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
$(TEST_OBJ_DIR)/%.o: $(SRC_DIR)/%.c | ${TEST_OBJ_DIR}
	${CC} ${TEST_FLAGES} $(OPTFLAGS) ${CSTANDARD} $(CPPFLAGS) $(CFLAGS) -c $< -o $@


${TEST_DIR} $(TEST_BIN_DIR) ${TEST_OBJ_DIR} :
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR) ${TEST_OBJ_DIR} ${TEST_BIN_DIR}

print:
	echo ${EXE} ${SRC} ${OBJ}