# ============================================================================
#  Makefile - Assignment 1, System Programming (course 3503820)
#  Yotam Weintraub - ID: 321610859
#
#  Targets:
#    make          / make all   - build the my_cp executable
#    make run                   - build and run with default demo arguments
#    make test                  - build, copy source.txt and verify with diff -s
#    make rebuild               - clean build from scratch
#    make clean                 - remove the executable and generated artifacts
# ============================================================================

CC       = gcc
CFLAGS   = -Wall -Wextra -std=gnu11 -O2
TARGET   = my_cp
SRCS     = my_cp.c

# Default arguments used by the `run` / `test` targets.
# Override from the command line, e.g.:
#   make run SRC_FILE=big.bin DST_FILE=copy.bin GRAN=4096
SRC_FILE ?= source.txt
DST_FILE ?= destination.txt
GRAN     ?= 1024

.PHONY: all run test rebuild clean

# Default target - builds the program.
all: $(TARGET)

# Single translation unit: compile and link in one step, exactly as required
# by the assignment: gcc -Wall -Wextra -std=gnu11 -O2 my_cp.c -o my_cp
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

# Run the program with the default (or overridden) arguments.
run: $(TARGET)
	./$(TARGET) $(SRC_FILE) $(DST_FILE) $(GRAN)

# Same as `run`, plus an explicit external verification of the copy.
test: run
	@diff -s $(SRC_FILE) $(DST_FILE)

# Force a full rebuild.
rebuild: clean all

# Remove build output and the benchmark's temporary files.
clean:
	rm -f $(TARGET) massive_source.bin massive_dest.bin
