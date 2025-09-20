SRCS := $(wildcard src/*.c)
BIN := algebra-engine
INCLUDE := -Iinclude

all:
	$(CC) $(SRCS) $(INCLUDE) -o $(BIN) && ./$(BIN)

format:
	@find src include -name "*.c" -o -name "*.h" | \
	awk '{print "Formatting "$$0"..."; system("clang-format -i "$$0)}'

help:
	@echo "Makefile commands:"
	@echo "  make         Compile and run the program"
	@echo "  make format  Format all .c and .h files with clang-format"
	@echo "  make help    Show this help message"
