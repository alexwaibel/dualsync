CC ?= cc

SOURCE_ROOT := ../../../core
TEST_BINARY := test_version
WARNING_FLAGS := -Wall -Wconversion -Werror -Wextra -Wpedantic -Wshadow \
	-Wstrict-prototypes
SANITIZER_FLAGS := -fsanitize=address,undefined -fno-omit-frame-pointer
CFLAGS := -std=c11 $(WARNING_FLAGS) $(SANITIZER_FLAGS) \
	-I$(SOURCE_ROOT)/include
LDFLAGS := $(SANITIZER_FLAGS)

.PHONY: all test

all: test

test: $(TEST_BINARY)
	./$(TEST_BINARY)

$(TEST_BINARY): $(SOURCE_ROOT)/tests/test_version.c \
		$(SOURCE_ROOT)/source/version.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
