PROJECT := container

CC := gcc
CXX := g++
PLATFORM := x64
RELEASE := debug

# ========== PROJECT STRUCT ==========
BASE_DIR := .
SRC_DIR := $(BASE_DIR)/src
INC_DIR := $(BASE_DIR)/include $(BASE_DIR)/lib/google
STAITC_LIB_DIR := $(BASE_DIR)/lib
DYNAMIC_LIB_DIR := 
BUILD_DIR := $(BASE_DIR)/build
OBJ_DIR := $(BUILD_DIR)/$(PLATFORM)/$(RELEASE)/obj
BIN_DIR := $(BUILD_DIR)/$(PLATFORM)/$(RELEASE)/bin

TARGET := $(BIN_DIR)/$(PROJECT)
C_SRC := $(wildcard $(SRC_DIR)/*.c)
CXX_SRC := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*.cc)

C_OBJECTS := $(patsubst %.o, $(OBJ_DIR)/%.o,	\
	$(patsubst %.c, %.o, $(notdir $(C_SRC)))	\
)
CXX_OBJECTS := $(patsubst %.o, $(OBJ_DIR)/%.o,	\
	$(patsubst %.cpp, %.o,						\
		$(patsubst %.cc, %.o, 					\
			$(notdir $(CXX_SRC) )				\
		)										\
	)											\
)
OBJECTS := $(C_OBJECTS) $(CXX_OBJECTS)

PRE_OBJ := $(patsubst %.o, %.i, $(OBJECTS))

# ========== compile option ==========
# --------------- PUBLIC -------------
OPTFLAGS := -O3
INCLUDE := -I $(INC_DIR)
STATIC_LIB := -l $(STAITC_LIB_DIR)
DYNAMIC_LIB := -ld $(DYNAMIC_LIB_DIR)


# ----------------- C ----------------
CFLAGS   := -Wall              # some warnings about bad code
LDFLAGS  := -Llib              # -L is a linker flag
LDLIBS   := -lm				   # Left empty if no libs are needed
C_STANDARD := -std=c2x
C_CPPFLAGS := $(INCLUDE) $(STATIC_LIB) $(DYNAMIC_LIB) $(C_STANDARD) $(OPTFLAGS) -MMD -MP # -I is a preprocessor flag, not a compiler flag


# ----------------- CXX --------------
CXX_STANDARD := -std=c++2a
CXX_CPPFLAGS := $(INCLUDE) $(STATIC_LIB) $(DYNAMIC_LIB) $(CXX_STANDARD) $(OPTFLAGS)


# =============== SCRIPT =============
.PHONY: echo clean all dir

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(C_CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXX_CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

pre: $(PRE_OBJ) | $(OBJ_DIR)
$(OBJ_DIR)/%.i: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(C_CPPFLAGS) -E -C -P $< -o $@

$(OBJ_DIR)/%.i: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXX_CPPFLAGS) -E -C -P $< -o $@

test:
	

clean: $(OBJ_DIR)
	@$(RM) -rv $<

clean_all:
	@$(RM) -rv $(BUILD_DIR)

dir: $(BIN_DIR) $(OBJ_DIR)


echo:
	@echo $(TARGET)
	@echo $(CXX_SRC)
	@echo $(OBJECTS)
	@echo $(PRE_OBJ)
	@echo $(C_CPPFLAGS)
	

