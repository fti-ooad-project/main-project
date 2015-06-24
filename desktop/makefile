include ../build-info.mk

C = gcc
CXX = g++
LINKFLAGS = $(_DESKTOP_LIBS)

OBJDIR = obj
DEPDIR = dep

CFLAGS = -I./source $(_CFLAGS) $(_DESKTOP_CFLAGS)
CXXFLAGS = -I./source $(_CXXFLAGS) $(_DESKTOP_CXXFLAGS)

HEADERS = $(_HEADERS)
SOURCES = $(_DESKTOP_SOURCES) $(_SOURCES)
OBJ = $(SOURCES:%=$(OBJDIR)/%.o)
DEP = $(SOURCES:%=$(DEPDIR)/%.d)
BIN = run

all: $(DEP) $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(LINKFLAGS) $^ -o $@

include deprules.mk

-include $(DEP)
