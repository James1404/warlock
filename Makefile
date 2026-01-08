CC = cc
CPP = c++
DBG = lldb

EXE = warlock

SRC_DIR = src
BUILD_DIR = build
CACHE_DIR = $(BUILD_DIR)/cache
DEPS_DIR = deps

TEST_DIR = test
STD_DIR = lib

INCLUDE = include\

INCLUDE_DIRS := $(foreach dir,$(INCLUDE),-I$(dir))

DEFINES = -D EXE_NAME=\"$(EXE)\"

CCFLAGS = -Wall -Werror -pedantic -fsanitize=integer -fsanitize-trap=all -g -glldb -Og -std=c99 $(DEFINES)\
		  $(INCLUDE_DIRS)\
	      `llvm-config --cflags`\
		  `pkg-config --cflags libeditline`

CPPFLAGS = -Wall -Werror -pedantic -fsanitize=address -fsanitize-trap=all -fsanitize=undefined -g -glldb -Og -std=c++20 $(DEFINES) $(INCLUDE_DIRS)

LDFLAGS = -fsanitize=address -fsanitize-trap=all -static-libsan \
	      `llvm-config --libs`\
		  `pkg-config --libs libeditline`

SRC := $(shell find $(SRC_DIR)/ -type f \( -iname \*.c -o -iname \*.cpp \))
OBJ := $(addprefix $(CACHE_DIR)/, $(addsuffix .o,$(basename $(notdir $(SRC)))))

DEPS := $(OBJ:.o=.d)

$(BUILD_DIR)/$(EXE): $(OBJ) | $(BUILD_DIR) $(CACHE_DIR) $(BUILD_DIR)/$(TEST_DIR) $(BUILD_DIR)/$(STD_DIR)
	@echo Linking: $@
	@$(CPP) -o $@ $^ $(LDFLAGS)

run: $(BUILD_DIR)/$(EXE)
	@(cd $(BUILD_DIR) && ./$(EXE))

repl: $(BUILD_DIR)/$(EXE)
	@(cd $(BUILD_DIR) && ./$(EXE) repl)

tests: $(BUILD_DIR)/$(EXE)
	@(cd $(BUILD_DIR) && ./$(EXE) test)

help: $(BUILD_DIR)/$(EXE)
	@(cd $(BUILD_DIR) && ./$(EXE) help)

version: $(BUILD_DIR)/$(EXE)
	@(cd $(BUILD_DIR) && ./$(EXE) version)

debug: $(BUILD_DIR)/$(EXE)
	@$(DBG) $(BUILD_DIR)/$(EXE)

-include $(DEPS)

$(CACHE_DIR)/%.o: $(SRC_DIR)/%.c | $(CACHE_DIR)
	@echo Building: $(notdir $<)
	@$(CC) $(CCFLAGS) -MMD -MP -c $< -o $@

$(CACHE_DIR)/%.o: $(SRC_DIR)/%.cpp | $(CACHE_DIR)
	@echo Building: $(notdir $<)
	@$(CPP) $(CPPFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	@echo Making \'$@\' directory
	@mkdir $@

$(CACHE_DIR): | $(BUILD_DIR)
	@echo Making $@ directory
	@mkdir -p $@

$(BUILD_DIR)/$(TEST_DIR): | $(TEST_DIR) $(BUILD_DIR)
	@echo Creating Symbolic Link: $(TEST_DIR) to $(BUILD_DIR)/ output folder
	@ln -sf $(realpath $(TEST_DIR)) $(abspath $(BUILD_DIR))

$(BUILD_DIR)/$(STD_DIR): | $(STD_DIR) $(BUILD_DIR)
	@echo Creating Symbolic Link: $(STD_DIR) to $(BUILD_DIR)/ output folder
	@ln -sf $(realpath $(STD_DIR)) $(abspath $(BUILD_DIR))

clean:
	@echo Deleting: $(BUILD_DIR)/
	@unlink $(BUILD_DIR)/$(TEST_DIR)
	@unlink $(BUILD_DIR)/$(STD_DIR)
	@rm -rf $(BUILD_DIR)/

.PHONY: run repl tests help version debug
