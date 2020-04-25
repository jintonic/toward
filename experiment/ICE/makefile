# Setup flags used by the implicit make rule: %:%.cc
CXXFLAGS=$(shell root-config --cflags)
LDLIBS=$(shell root-config --libs)

SRC=$(wildcard *.cc) # a list of all C++ source files
EXE=$(SRC:.cc=) # a list of all executables

.PHONY: all clean

all: $(EXE)
clean:
	$(RM) -r `cat .gitignore`
