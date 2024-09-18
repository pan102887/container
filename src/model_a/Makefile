PROJECT := libmodel_a.so

CC := gcc
CXX := g++
PLATFORM := x64
RELEASE := debug

# ========== PROJECT STRUCT ==========
BASE_DIR := ../..
SRC_DIR := $(BASE_DIR)/src
INC_DIR := $(BASE_DIR)/include $(BASE_DIR)/src/model_a						# 包含(头文件)目录
LIB_DIR := 																	# 库目录
LIB :=																		# 引入的库
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
OPTFLAGS := -O2
INCLUDE := $(addprefix -I,$(INC_DIR))
CFLAGS   := -Wall -D__TEST__            		# some warnings about bad code

# ----------------- C ----------------
C_STANDARD := -std=c2x
C_CPPFLAGS := $(INCLUDE) $(CFLAGS) $(C_STANDARD) $(OPTFLAGS) -MMD -MP # -I is a preprocessor flag, not a compiler flag

# ----------------- CXX --------------
CXX_STANDARD := -std=c++2a
CXX_CPPFLAGS := $(INCLUDE) $(CFLAGS) $(CXX_STANDARD) $(OPTFLAGS)

# ============= LINK OPTION ==========
LDFLAGS  := $(addprefix -L,$(LIB_DIR))              			# -Ldir 动态库搜索路径中增加dir
LDLIBS   := $(addprefix -l,$(LIB))				   				# 连接静态库libname.a 或动态库libname.so的库文件


# =============== SCRIPT =============
.PHONY: echo clean all dir

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) -shared $^ $(LDFLAGS) $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(C_CPPFLAGS) -fPIC -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXX_CPPFLAGS) -fPIC -c $< -o $@

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
	@echo $(INCLUDE)
	

