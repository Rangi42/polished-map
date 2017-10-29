PROGNAME = Polished Map
BINNAME = polishedmap
DEBUGBINNAME = polishedmapd

DESTDIR =
TARGET_PREFIX = /usr/local
TARGET_BINDIR = $(TARGET_PREFIX)/bin
TARGET_DATADIR = $(TARGET_PREFIX)/share

CXX = g++
LD = $(CXX)
RM = rm -rf

SRCDIR = src
RESDIR = res
OBJDIR = tmp
DEBUGOBJDIR = tmp/debug
LIBDIR = lib
BINDIR = bin

CXXFLAGS = -std=c++11 -I$(SRCDIR) -I$(RESDIR) $(shell fltk-config --use-images --cxxflags)
LDFLAGS = $(shell fltk-config --use-images --ldflags) $(shell pkg-config --libs libpng16 xpm)

RELEASEFLAGS = -DNDEBUG -O3 -flto -march=native
DEBUGFLAGS = -DDEBUG -D_DEBUG -O0 -g -ggdb3 -Wall -Wextra -pedantic -Wno-unknown-pragmas -Wno-sign-compare

COMMON = $(wildcard $(SRCDIR)/*.h) $(wildcard $(RESDIR)/*.xpm)
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DEBUGOBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(DEBUGOBJDIR)/%.o)
TARGET = $(BINDIR)/$(BINNAME)
DEBUGTARGET = $(BINDIR)/$(DEBUGBINNAME)

.PHONY: all $(BINNAME) $(DEBUGBINNAME) release debug clean install

.SUFFIXES: .o .cpp

all: $(BINNAME)

$(BINNAME): release
$(DEBUGBINNAME): debug

release: CXXFLAGS += $(RELEASEFLAGS)
release: $(TARGET)

debug: CXXFLAGS += $(DEBUGFLAGS)
debug: $(DEBUGTARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(LD) -o $@ $(OPTIMIZELDFLAGS) $^ $(LDFLAGS)

$(DEBUGTARGET): $(DEBUGOBJECTS)
	@mkdir -p $(@D)
	$(LD) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(COMMON)
	@mkdir -p $(@D)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(DEBUGOBJDIR)/%.o: $(SRCDIR)/%.cpp $(COMMON)
	@mkdir -p $(@D)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

clean:
	$(RM) $(TARGET) $(DEBUGTARGET) $(OBJECTS) $(DEBUGOBJECTS)

install: release
	cp $(TARGET) $(DESTDIR)$(TARGET_BINDIR)/$(BINNAME)
	cp $(RESDIR)/app.xpm $(DESTDIR)$(TARGET_DATADIR)/pixmaps/polishedmap48.xpm
	cp $(RESDIR)/app-icon.xpm $(DESTDIR)$(TARGET_DATADIR)/pixmaps/polishedmap16.xpm
