TOPNAME  = driver
NXDC_FILES = constr/top.nxdc
VER_FILE = $(shell find $(abspath ./vsrc) -name "*.v")
CPP_FILE = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")

BUILD_DIR = ./build
AUTO_BIND_FILE = $(abspath .)/build/auto_bind.cpp
SRC_FILE = $(VER_FILE) $(CPP_FILE) $(AUTO_BIND_FILE) $(NVBOARD_ARCHIVE)

BIN = $(abspath ./build)/$(TOPNAME)

include $(NVBOARD_HOME)/scripts/nvboard.mk

COMPILER = verilator

COMPILER_FLAGS=-MMD --build -cc -O3 --x-assign fast --x-initial fast --noassert --exe --trace
$(AUTO_BIND_FILE):$(NXDC_FILES)
	-rm -r build
	mkdir -p build
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $(NXDC_FILES) $(AUTO_BIND_FILE)

bind:$(AUTO_BIND_FILE)


$(BIN):$(SRC_FILE)
	$(COMPILER) $(COMPILER_FLAGS) $(SRC_FILE) -o $(BIN)  --top-module $(TOPNAME) -CFLAGS -g -CFLAGS -I/home/talps/gitrepo/ysyx-workbench/nvboard/include -LDFLAGS -lSDL2 -LDFLAGS -lSDL2_image --Mdir $(BUILD_DIR)/obj_dir /home/talps/gitrepo/ysyx-workbench/nvboard/build/nvboard.a

build:$(BIN)

.PHONY:clean build bind 
clean:
	-rm -r build
