SOURCE_DIR        = ./src
BUILD_DIR         = ./build
INSTALL_DIR       = ~/.bin

NAME               = hashtest
TARGET             = $(BUILD_DIR)/$(NAME)

CC                 = mpicc
CFLAGS             = -std=c99 -Wall -Wextra -pedantic -g2 -ldl -fpic -O2


# {{{ Build variables
date   := $(shell date "+%Y-%m-%d-%H:%M:%S")
host   := $(shell uname -n)
user   := $(shell whoami)
arch   := $(shell uname -m)
endian := $(shell echo -n I | od -to2 | head -n1 | cut -f2 "-d " | cut -c6)
# }}}

# {{{ Build cflags
CFLAGS += -D DATE=$(date)
CFLAGS += -D HOST=$(host)
CFLAGS += -D USER=$(user)
CFLAGS += -D ARCH=$(arch)
CFLAGS += -D ENDIAN=$(endian)
# }}}


all: build modules

build: $(TARGET)

modules: $(BUILD_DIR)/modules/md5.so $(BUILD_DIR)/modules/sha256.so

$(TARGET): $(BUILD_DIR)/main.o $(BUILD_DIR)/passgen.o $(BUILD_DIR)/hashgen.o
	@echo 'Building target:'
	$(CC) $(CFLAGS) $^ -o $@
	@echo -e $(SUCCESS_MSG)

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $^ -o $@

$(BUILD_DIR)/modules/%.so: ./src/modules/%.c
	@mkdir -p $(BUILD_DIR)/modules
	$(CC) $(CFLAGS) -lssl -shared $^ -o $@

.PHONY: rebuild
rebuild: clean build

.PHONY: install
install: build
	@echo 'Installing target:'
	cp -f $(TARGET) $(INSTALL_DIR)/$(NAME)

.PHONY: uninstall
uninstall:
	@echo 'Uninstalling target:'
	rm -f $(INSTALL_DIR)/$(NAME)

.PHONY: clean
clean:
	@echo 'Cleaning workspace:'
	rm -rf $(BUILD_DIR)/
