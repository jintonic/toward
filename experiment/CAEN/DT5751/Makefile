UNIC = $(HOME)
GEE = $(HOME)

OPT2       = -O2

ROOTCONFIG = root-config

ARCH      := $(shell $(ROOTCONFIG) --arch)
ALTCXX    := $(shell $(ROOTCONFIG) --cxx)
ROOTCFLAGS:= $(shell $(ROOTCONFIG) --cflags)
ROOTLIBS  := $(shell $(ROOTCONFIG) --libs)

ARCHOK     = no

ifeq ($(ARCH),linux)
  CXX      = g++
  CXXFLAGS = $(OPT2) -Wall -fPIC
  ARCHOK   = yes
endif

ifeq ($(ARCH),linuxx8664gcc)
  CXX      = g++
  CXXFLAGS = $(OPT2) -Wall -fPIC
  ARCHOK   = yes
endif

# If arch != the above options, a error message is given
ifeq ($(ARCHOK),no)
  $(error $(ARCH) invalid architecture)
endif

# In case that ALTCXX = 0, g++ will be used as CXX
ifneq ($(ALTCXX),)
  CXX = $(ALTCXX)
endif

# Finally, define CXXFLAGS & LIBS
CXXFLAGS+= $(ROOTCFLAGS) -I$(UNIC)/include/UNIC -I$(GEE)/include/GEE
LIBS     = $(ROOTLIBS)

# Define SOURCES, HEADERS & OBJECTS 
# ==========================================

SOURCES = $(wildcard *.cc)
HEADERS = $(SOURCES:.cc=.h)
OBJECTS = $(SOURCES:.cc=.o)
DEPFILE = $(SOURCES:.cc=.d)

SRCS = $(wildcard *.C)
EXES = $(SRCS:.C=.exe)

OBJS = $(wildcard *.c)

# Action starts
# =============

all: $(EXES) daq
	@echo
	@echo "* Done!"
	@echo 

daq:$(OBJS)
	@echo
	@echo "* Creating C executable:"
	gcc -I. -o $@.exe $(OBJS) -lCAENDigitizer

# include *.d files, which are makefiles defining dependencies between files
ifeq ($(filter info clean tags,$(MAKECMDGOALS)),)
  -include $(DEPFILE)
endif

# rules to create *.d files
%.d:%.cc
	@echo creating $@
	@set -e; rm -f $@; \
	  $(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	  sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	  rm -f $@.$$$$ 

# An xxx.o file depends on xxx.cc. It is created with the command:
# 	g++ -c xxx.cc -o xxx.o
# Since this is obvious, "make" automatically does it. 
# There is no need to explicitly write down the rules to do it.

$(EXES):%.exe:%.C $(OBJECTS)
	@echo
	@echo "* Creating C++ executables:"
	$(CXX) $< $(OBJECTS) $(CXXFLAGS) -L$(GEE)/lib -lGEE $(LIBS) -o $@

clean:
	$(RM) *.exe *.o *.d *.d.* *~

.PHONY: all clean

