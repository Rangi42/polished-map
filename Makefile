PROGNAME = Polished Map
BINNAME = polishedmap
DEBUGBINNAME = polishedmapd

CXX = g++
LD = $(CXX)
RM = rm -rf

OSDIRNAME = linux
SRCDIR = src
RESDIR = res
OBJDIR = tmp/$(OSDIRNAME)
DEBUGOBJDIR = tmp/$(OSDIRNAME)/debug
LIBDIR = lib/$(OSDIRNAME)
BINDIR = bin/$(OSDIRNAME)

CXXFLAGS = -std=c++11 -isystem include -isystem /usr/include -I$(SRCDIR) -I$(RESDIR)
LDFLAGS = $(wildcard $(LIBDIR)/*.a) -lm -lpng -lz -lXfixes -lXext -lXft -lfontconfig -lXinerama -lpthread -ldl -lX11 -lXpm -lXrender

RELEASEFLAGS = -DNDEBUG -O2 -flto -march=native
DEBUGFLAGS = -DDEBUG -D_DEBUG -O0 -g -ggdb3 -Wall -Wextra -pedantic -Wsign-conversion

COMMON = $(wildcard $(SRCDIR)/*.h) $(wildcard $(RESDIR)/*.xpm)
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DEBUGOBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(DEBUGOBJDIR)/%.o)
TARGET = $(BINDIR)/$(BINNAME)
DEBUGTARGET = $(BINDIR)/$(DEBUGBINNAME)

.PHONY: all $(DEBUGBINNAME) $(BINNAME) debug release clean install

.SUFFIXES: .o .cpp

all: $(BINNAME)

$(DEBUGBINNAME): debug
$(BINNAME): release

debug: CXXFLAGS += $(DEBUGFLAGS)
debug: $(DEBUGTARGET)

release: CXXFLAGS += $(RELEASEFLAGS)
release: $(TARGET)

$(DEBUGTARGET): $(DEBUGOBJECTS)
	@ mkdir -p $(BINDIR)
	@ echo Linking $@ ...
	@ $(LD) -o $@ $^ $(LDFLAGS)
	@ echo Done building $(PROGNAME)

$(TARGET): $(OBJECTS)
	@ echo Building $(PROGNAME) for release ...
	@ mkdir -p $(BINDIR)
	@ echo Linking $@ ...
	@ $(LD) -o $@ $(OPTIMIZELDFLAGS) $^ $(LDFLAGS)
	@ echo Done building $(PROGNAME)

$(DEBUGOBJDIR)/%.o: $(SRCDIR)/%.cpp $(COMMON)
	@ mkdir -p $(DEBUGOBJDIR)
	@ echo Compiling $@ ...
	@ $(CXX) -c $(CXXFLAGS) -o $@ $<

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(COMMON)
	@ mkdir -p $(OBJDIR)
	@ echo Compiling $@ ...
	@ $(CXX) -c $(CXXFLAGS) -o $@ $<

clean:
	@ echo Cleaning ...
	-@ $(RM) $(TARGET) $(DEBUGTARGET) $(OBJECTS) $(DEBUGOBJECTS)
	@ echo Done cleaning

install: release
	cp $(TARGET) /usr/local/bin/$(BINNAME)
	cp $(RESDIR)/app.xpm /usr/share/pixmaps/polishedmap48.xpm
	cp $(RESDIR)/app-icon.xpm /usr/share/pixmaps/polishedmap16.xpm
