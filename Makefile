CC = g++
CFLAGS = -Wall -Wextra -std=c++20
SRCDIR = src
BINDIR = bin

# List of source files
SOURCES := $(wildcard $(SRCDIR)/*.cpp)
SOURCES += main.cpp

# Executable name
EXECUTABLE = $(BINDIR)/tiny

.PHONY: all clean

all: dir tiny

tiny: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE)

dir:
	@mkdir -p $(BINDIR)
	@echo @mkdir $(BINDIR)

clean:
	@rm -rf $(BINDIR)
	@echo @rm -rf $(BINDIR)
