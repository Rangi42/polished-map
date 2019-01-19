DESTDIR =
PREFIX = /usr/local

polishedmap = polishedmap
polishedmapd = polishedmapd

CXX ?= g++
LD = $(CXX)
RM = rm -rf

srcdir = src
resdir = res
tmpdir = tmp
debugdir = tmp/debug
bindir = bin

CXXFLAGS = -std=c++11 -I$(srcdir) -I$(resdir) $(shell fltk-config --use-images --cxxflags)
LDFLAGS = $(shell fltk-config --use-images --ldflags) $(shell pkg-config --libs libpng xpm)

RELEASEFLAGS = -DNDEBUG -O3 -flto -march=native
DEBUGFLAGS = -DDEBUG -D_DEBUG -O0 -g -ggdb3 -Wall -Wextra -pedantic -Wno-unknown-pragmas -Wno-sign-compare -Wno-unused-parameter

COMMON = $(wildcard $(srcdir)/*.h) $(wildcard $(resdir)/*.xpm)
SOURCES = $(wildcard $(srcdir)/*.cpp)
OBJECTS = $(SOURCES:$(srcdir)/%.cpp=$(tmpdir)/%.o)
DEBUGOBJECTS = $(SOURCES:$(srcdir)/%.cpp=$(debugdir)/%.o)
TARGET = $(bindir)/$(polishedmap)
DEBUGTARGET = $(bindir)/$(polishedmapd)
DESKTOP = "$(DESTDIR)$(PREFIX)/share/applications/Polished Map.desktop"

.PHONY: all $(polishedmap) $(polishedmapd) release debug clean install uninstall

.SUFFIXES: .o .cpp

all: $(polishedmap)

$(polishedmap): release
$(polishedmapd): debug

release: CXXFLAGS += $(RELEASEFLAGS)
release: $(TARGET)

debug: CXXFLAGS += $(DEBUGFLAGS)
debug: $(DEBUGTARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(LD) -o $@ $^ $(LDFLAGS)

$(DEBUGTARGET): $(DEBUGOBJECTS)
	@mkdir -p $(@D)
	$(LD) -o $@ $^ $(LDFLAGS)

$(tmpdir)/%.o: $(srcdir)/%.cpp $(COMMON)
	@mkdir -p $(@D)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(debugdir)/%.o: $(srcdir)/%.cpp $(COMMON)
	@mkdir -p $(@D)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

clean:
	$(RM) $(TARGET) $(DEBUGTARGET) $(OBJECTS) $(DEBUGOBJECTS)

install: release
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(polishedmap)
	mkdir -p $(DESTDIR)$(PREFIX)/share/pixmaps
	cp $(resdir)/app.xpm $(DESTDIR)$(PREFIX)/share/pixmaps/polishedmap48.xpm
	cp $(resdir)/app-icon.xpm $(DESTDIR)$(PREFIX)/share/pixmaps/polishedmap16.xpm
	mkdir -p $(DESTDIR)$(PREFIX)/share/applications
	echo "[Desktop Entry]" > $(DESKTOP)
	echo "Name=Polished Map" >> $(DESKTOP)
	echo "Comment=Edit pokecrystal maps and tilesets" >> $(DESKTOP)
	echo "Icon=$(DESTDIR)$(PREFIX)/share/pixmaps/polishedmap48.xpm" >> $(DESKTOP)
	echo "Exec=$(DESTDIR)$(PREFIX)/bin/$(polishedmap)" >> $(DESKTOP)
	echo "Type=Application" >> $(DESKTOP)
	echo "Terminal=false" >> $(DESKTOP)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(polishedmap)
	rm -f $(DESTDIR)$(PREFIX)/share/pixmaps/polishedmap48.xpm
	rm -f $(DESTDIR)$(PREFIX)/share/pixmaps/polishedmap16.xpm
	rm -f $(DESKTOP)
