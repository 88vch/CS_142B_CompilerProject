CC = g++
CFLAGS = -Wall -Wextra -std=c++20
SRCDIR = src
BINDIR = bin

# List of source files
SOURCES := $(wildcard $(SRCDIR)/*.cpp)
HEADERS := $(wildcard $(SRCDIR)/*.hpp)
SOURCES += main.cpp

# Object files
OBJECTS := $(SOURCES:.cpp=.o)

# Executable name
EXECUTABLE = $(BINDIR)/tiny

.PHONY: all clean

all: dir $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

dir:
	@mkdir -p $(BINDIR)

clean:
	@rm -rf $(BINDIR)
	@rm -f src/*.o
	@rm -rf tst/Lexer_results.txt
