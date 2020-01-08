ifeq ($(prefix),)
  prefix=$(HOME)
endif

LIBS = $(shell root-config --libs)
CXXFLAGS = -O2 -Wall -fPIC $(shell root-config --cflags)

LIBRARY = libTOWARD.so
ROOTMAP = $(LIBRARY:.so=.rootmap)

DICTSRC = Dictionary.cc
DICTHDR = $(DICTSRC:.cc=.h)
DICTOBJ = $(DICTSRC:.cc=.o)

SOURCES = $(filter-out $(DICTSRC), $(wildcard *.cc))
HEADERS = $(filter-out $(DICTHDR), $(wildcard *.h))
OBJECTS = $(SOURCES:.cc=.o)
DEPFILE = $(SOURCES:.cc=.d)

# The first target is the default target, it depends on $(ROOTMAP).
# Before it, make will include other makefiles specified by the include command
install: $(ROOTMAP)
	mkdir -p $(prefix)/lib/
	cp libTOWARD.* $(prefix)/lib/
	mkdir -p $(prefix)/include/TOWARD/
	cp *.h $(prefix)/include/TOWARD/
	@echo "done."

# Include *.d files, which are makefiles defining dependencies between files
ifeq ($(filter info clean tags,$(MAKECMDGOALS)),)
  -include $(DEPFILE) # - tells make to continue without $(DEPFILE)
endif

# rules to create *.d files
%.d:%.cc
	@echo creating $@
	@set -e; rm -f $@; \
	  $(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	  sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	  rm -f $@.$$$$ 

# libTOWARD.rootmap can only be created after the creation of libTOWARD.so. It
# tells ROOT the dependence among libraries. Putting it along with the
# corresponding library allows one to use in CLING the functions defined in the
# library without calling gSystem->Load("libXXX.so")
$(ROOTMAP): $(LIBRARY)
	@echo
	@echo "* Creating rootmap file:"
	rootcling -f $(DICTSRC) -rmf $@ -rml $^ $(HEADERS)

# libTOWARD.so depends on all *.o files.
#  The flag "-shared" is used to create shared libs
#  $@ represents the target, that is, libTOWARD.so
#  $^ represents all the prerequisites, i.e., all *.o files
$(LIBRARY): $(DICTOBJ) $(OBJECTS)
	@echo
	@echo "* Creating shared library:"
	$(CXX) $^ $(CXXFLAGS) -shared -o $@ $(LIBS)

# An xxx.o file depends on xxx.cc. It is created with the command:
# 	g++ -c xxx.cc -o xxx.o
# Since this is obvious, "make" automatically does it. 
# There is no need to explicitly write down the rules to do it.

# To use classes & functions directly in ROOT. One has to do the following
$(DICTSRC): $(HEADERS)
	@echo 
	@echo "* Generate dictionary:" 
	@rm -f Dictionary.*
	rootcling -f $(DICTSRC) $(HEADERS)
	@echo 
	@echo "* Creating object files:" 

info: 
	@echo
	@echo "target:   $(LIBRARY) $(ROOTMAP)"
	@echo "sources:  $(SOURCES)"
	@echo "headers:  $(HEADERS)"
	@echo "objects:  $(OBJECTS)"
	@echo 
	@echo "compiler: $(CXX)"
	@echo "flags:    $(CXXFLAGS)"
	@echo "libs:     $(LIBS)"
	@echo

clean:
	$(RM) `cat .gitignore`

tags:
	ctags --c-kinds=+p $(HEADERS) $(SOURCES)

uninstall:
	$(RM) -r $(prefix)/include/TOWARD
	$(RM) -r $(prefix)/lib/libTOWARD.so

.PHONY: info tags clean install uninstall
