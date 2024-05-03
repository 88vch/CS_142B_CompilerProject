CC = g++
CFLAGS = -Wall -Wextra -std=c++20
SRCDIR = src
BINDIR = bin

# List of source files
SOURCES := $(wildcard $(SRCDIR)/*.cpp)
# List of object files
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(BINDIR)/%.o,$(SOURCES))
# Executable name
EXECUTABLE = $(BINDIR)/tiny

.PHONY: all clean

# make bin/tiny
all: $(EXECUTABLE)
################################################
######          TESTING BEGIN            #######
################################################
# make tiny
tiny: $(BINDIR)/main.o
	$(CC) $(CFLAGS) $(BINDIR)/main.o -o $(EXECUTABLE)

$(BINDIR)/main.o: main.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c main.cpp -o $(BINDIR)/main.o
################################################
######           TESTING END             #######
################################################

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@

$(BINDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(BINDIR)

