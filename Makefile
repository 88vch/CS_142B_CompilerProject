CC = g++
CFLAGS = -g -Wall -Wextra -std=c++20
SRCDIR = src
BINDIR = bin
RESDIR = res

# List of source files
SOURCES := $(wildcard $(SRCDIR)/*.cpp)
HEADERS := $(wildcard $(SRCDIR)/*.hpp)
SOURCES += main.cpp

# Object files
OBJECTS := $(SOURCES:.cpp=.o)

# Executable name
EXECUTABLE = $(BINDIR)/tiny

.PHONY: all clean

all: dir $(EXECUTABLE) mv_dir

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

dir:
	@mkdir -p $(BINDIR)
	@mkdir -p $(RESDIR)

mv_dir:
	@mv src/*.o $(BINDIR)
	@mv main.o $(BINDIR)

clean:
	@rm -rf $(BINDIR)
	@rm -rf $(RESDIR)
	@rm -f src/*.o
	@rm -rf tst/Lexer_results.txt
