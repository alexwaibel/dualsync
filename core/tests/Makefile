CC ?= cc

SOURCE_ROOT := ../../../core
TEST_BINARY := test_version
CFLAGS := -std=c11 -Wall -Wextra -Werror -pedantic \
	-I$(SOURCE_ROOT)/include

.PHONY: all test

all: test

test: $(TEST_BINARY)
	./$(TEST_BINARY)

$(TEST_BINARY): $(SOURCE_ROOT)/tests/test_version.c \
		$(SOURCE_ROOT)/source/version.c
	$(CC) $(CFLAGS) -o $@ $^
