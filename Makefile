PROJECT := cpp-demo

CC := gcc
CXX := g++
# --------- C ----------------
C_CPPFLAGS := -Iinclude -MMD -MP # -I is a preprocessor flag, not a compiler flag
CFLAGS   := -Wall              # some warnings about bad code
LDFLAGS  := -Llib              # -L is a linker flag
LDLIBS   := -lm				   # Left empty if no libs are needed
C_STANDARD := -std=c2x


# --------- CXX ----------------
CXX_STANDARD := -std=c++2a


# --------- PUBLIC -------------
OPTFLAGS := -O3

# --------- PROJECT ------------
BASE_DIR := .
BUILD_DIR := $(BASE_DIR)/build
PLATFORM := x64
RELEASE := debug


SRC_DIR := $(BASE_DIR)/src
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





.PHONY: echo clean all dir

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(C_CPPFLAGS) $(OPTFLAGS) $(C_STANDARD) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX)  $(OPTFLAGS) $(CXX_STANDARD) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@


pre: $(PRE_OBJ) | $(OBJ_DIR)

$(OBJ_DIR)/%.i: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -E -C -P $< -o $@

$(OBJ_DIR)/%.i: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) -E -C -P $< -o $@

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
	

