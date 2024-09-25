PROJECT := container

CC := gcc
CXX := g++
PLATFORM := x64
RELEASE := Debug

# ========== PROJECT STRUCT ==========
BASE_DIR	:=	.
SRC_DIR		:=	$(BASE_DIR)/src
INC_DIR		:=	$(BASE_DIR)/include
LIB_DIR		:=	$(BASE_DIR)/lib
LIB			:=
BUILD_DIR	:=	$(BASE_DIR)/build
OBJ_DIR		:=	$(BUILD_DIR)/$(PLATFORM)/$(RELEASE)/obj
BIN_DIR		:=	$(BUILD_DIR)/$(PLATFORM)/$(RELEASE)/bin

TARGET	:= 	$(BIN_DIR)/$(PROJECT)
C_SRC 	:= 	$(wildcard $(SRC_DIR)/*.c)
CPP_SRC	:= 	$(wildcard $(SRC_DIR)/*.cpp)
CC_SRC 	:= 	$(wildcard $(SRC_DIR)/*.cc)

C_OBJECTS	:= 	$(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(C_SRC)))
CXX_OBJECTS	:= 	$(patsubst %.cpp, $(OBJ_DIR)/%.o,$(notdir $(CPP_SRC)))\
				$(patsubst %.cc, $(OBJ_DIR)/%.o,$(notdir $(CC_SRC)))

OBJECTS := $(C_OBJECTS) $(CXX_OBJECTS)
PRE_OBJ := $(patsubst %.o, %.i, $(OBJECTS))

# ========== compile option ==========
OPTFLAGS	:= -O2
INCLUDE		:= $(addprefix -I,$(INC_DIR))
CFLAGS		:= -Wall -D__TEST__
# ----------------- C ----------------
C_STANDARD := -std=c2x
C_CPPFLAGS := $(INCLUDE) $(CFLAGS) $(C_STANDARD) $(OPTFLAGS) -MMD -MP

# ----------------- CXX --------------
CXX_STANDARD := -std=c++2a
CXX_CPPFLAGS := $(INCLUDE) $(CFLAGS) $(CXX_STANDARD) $(OPTFLAGS)

# ============= LINK OPTION ==========
LDFLAGS  := $(addprefix -L,$(LIB_DIR))
LDLIBS   := $(addprefix -l,$(LIB))


# =============== SCRIPT =============
.PHONY: echo clean all dir

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(C_CPPFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXX_CPPFLAGS) -c $< -o $@

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
	@echo $(OBJECTS)
	@echo $(TEST)
	

