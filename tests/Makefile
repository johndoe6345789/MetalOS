# MetalOS Test Suite Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I../tests/include -I../kernel/include -I../bootloader/include
LDFLAGS = 

# Test source files
TEST_SOURCES = $(wildcard unit/*.c)
TEST_BINARIES = $(TEST_SOURCES:.c=)

# Default target
.PHONY: all
all: $(TEST_BINARIES)

# Build each test binary
unit/%: unit/%.c include/test_framework.h
	@echo "Building test: $@"
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Run all tests
.PHONY: test
test: all
	@echo ""
	@echo "╔════════════════════════════════════════════╗"
	@echo "║      Running MetalOS Test Suite           ║"
	@echo "╚════════════════════════════════════════════╝"
	@echo ""
	@for test in $(TEST_BINARIES); do \
		./$$test || exit 1; \
	done
	@echo ""
	@echo "╔════════════════════════════════════════════╗"
	@echo "║      All Test Suites Passed ✓              ║"
	@echo "╚════════════════════════════════════════════╝"
	@echo ""

# Run tests with verbose output
.PHONY: test-verbose
test-verbose: all
	@for test in $(TEST_BINARIES); do \
		echo "Running $$test..."; \
		./$$test -v; \
		echo ""; \
	done

# Clean test binaries
.PHONY: clean
clean:
	@echo "Cleaning test binaries..."
	@rm -f $(TEST_BINARIES)
	@rm -f unit/*.o
	@echo "Clean complete"

# Help target
.PHONY: help
help:
	@echo "MetalOS Test Suite Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all           - Build all test binaries"
	@echo "  test          - Build and run all tests"
	@echo "  test-verbose  - Run tests with verbose output"
	@echo "  clean         - Remove test binaries"
	@echo "  help          - Show this help message"
