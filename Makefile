EMXX := em++
CXX := g++
CXX_FLAGS := -w -std=c++17

SRCDIR := src
OBJDIR := bin
EM_OBJDIR := embin
INC := -I include
LIB := -L lib
EM_LIB := -L emlib

LIBRARIES := -l glpk
TARGET := main

SOURCES := $(shell find $(SRCDIR) -type f -name '*.cpp' -o -name '*.c')
# to escape $, use $$
#SOURCES := $(shell find $(SRCDIR) -type f -regex ".*\.\(cpp\|c\)$$")
OBJECTS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(addsuffix .o,$(SOURCES)))
EM_OBJECTS := $(patsubst $(SRCDIR)/%,$(EM_OBJDIR)/%,$(addsuffix .o,$(SOURCES)))

$(TARGET): $(filter-out bin/main.cpp.o, $(OBJECTS))
	$(CXX) $(LIB) $^ -o $@ $(LIBRARIES)

$(OBJDIR)/%.o: $(SRCDIR)/%
	$(CXX) $(CXX_FLAGS) $(INC) -c -o $@ $<

$(EM_OBJDIR)/%.o: $(SRCDIR)/%
	$(EMXX) $(CXX_FLAGS) $(INC) -c -o $@ $<

$(OBJECTS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(EM_OBJECTS): | $(EM_OBJDIR)

$(EM_OBJDIR):
	mkdir -p $(EM_OBJDIR)

run: $(TARGET)
	mkdir -p output
	clear
	@LD_LIBRARY_PATH=${PWD}/lib:${LD_LIBRARY_PATH} ./$(TARGET)

clean:
	-rm $(OBJDIR)/* $(TARGET)
	-rm $(EM_OBJDIR)/* ./web.*


web: react-app/public/web.js

react-app/public/web.js: $(filter-out embin/main.cpp.o, $(EM_OBJECTS))
	$(EMXX) --bind $(EM_LIB) $^ -o $@ $(LIBRARIES)
	# -s ENVIRONMENT=node
	# -s MODULARIZE=1 -s EXPORT_ES6=1
	# -s ALLOW_MEMORY_GROWTH=1
