CC = g++
CFLAGS = -Wall -Wextra -std=c++20 -g
SRCDIR = src
BINDIR = bin

# List of source files
SOURCES := $(wildcard $(SRCDIR)/*.cpp)
SOURCES += main.cpp

# Executable name
EXECUTABLE = $(BINDIR)/tiny

.PHONY: all clean

all: tiny

tiny: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE)

clean:
	@echo @rm -rf $(BINDIR)
