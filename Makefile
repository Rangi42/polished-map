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

CXXFLAGS = -std=c++11 -I include -isystem /usr/include -I$(SRCDIR) -I$(RESDIR)
LDFLAGS = $(wildcard $(LIBDIR)/*.a) -lm -lpng -lz -lXfixes -lXext -lXft -lfontconfig -lXinerama -lpthread -ldl -lX11 -lXpm -lXrender

RELEASEFLAGS = -DNDEBUG -O3 -flto -march=native
DEBUGFLAGS = -DDEBUG -D_DEBUG -O0 -g -ggdb3 -Wall -Wextra -pedantic -Wno-unknown-pragmas -Wno-reorder -Wno-unused-parameter -Wno-sign-compare

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
	cp $(TARGET) /usr/local/bin/$(BINNAME)
	cp $(RESDIR)/app.xpm /usr/share/pixmaps/polishedmap48.xpm
	cp $(RESDIR)/app-icon.xpm /usr/share/pixmaps/polishedmap16.xpm
