DESTDIR =
PREFIX = /usr/local

polishedmap = polishedmap-plusplus
polishedmapd = polishedmap-plusplusd

CXX ?= g++
LD = $(CXX)
RM = rm -rf

srcdir = src
resdir = res
tmpdir = tmp
debugdir = tmp/debug
bindir = bin

fltk-config = $(bindir)/fltk-config

CXXFLAGS := -std=c++17 -I$(srcdir) -I$(resdir) $(shell $(fltk-config) --use-images --cxxflags) $(CXXFLAGS)
LDFLAGS := $(shell $(fltk-config) --use-images --ldstaticflags) $(shell pkg-config --libs xpm) $(LDFLAGS)

RELEASEFLAGS = -DNDEBUG -O3 -flto
DEBUGFLAGS = -DDEBUG -D_DEBUG -O0 -g -ggdb3 -Wall -Wextra -pedantic -Wno-unknown-pragmas -Wno-sign-compare -Wno-unused-parameter

COMMON = $(wildcard $(srcdir)/*.h) $(wildcard $(resdir)/*.xpm) $(resdir)/help.html
SOURCES = $(wildcard $(srcdir)/*.cpp)
OBJECTS = $(SOURCES:$(srcdir)/%.cpp=$(tmpdir)/%.o)
DEBUGOBJECTS = $(SOURCES:$(srcdir)/%.cpp=$(debugdir)/%.o)
TARGET = $(bindir)/$(polishedmap)
DEBUGTARGET = $(bindir)/$(polishedmapd)
DESKTOP = "$(DESTDIR)$(PREFIX)/share/applications/Polished Map++.desktop"

.PHONY: all $(polishedmap) $(polishedmapd) release debug clean install uninstall

.SUFFIXES: .o .cpp

all: $(polishedmap)

$(polishedmap): release
$(polishedmapd): debug

release: CXXFLAGS := $(RELEASEFLAGS) $(CXXFLAGS)
release: $(TARGET)

debug: CXXFLAGS := $(DEBUGFLAGS) $(CXXFLAGS)
debug: $(DEBUGTARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(LD) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

$(DEBUGTARGET): $(DEBUGOBJECTS)
	@mkdir -p $(@D)
	$(LD) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

$(tmpdir)/%.o: $(srcdir)/%.cpp $(COMMON)
	@mkdir -p $(@D)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(debugdir)/%.o: $(srcdir)/%.cpp $(COMMON)
	@mkdir -p $(@D)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

clean:
	$(RM) $(TARGET) $(DEBUGTARGET) $(OBJECTS) $(DEBUGOBJECTS)

install: release
	mkdir -p "$(DESTDIR)$(PREFIX)/bin"
	cp $(TARGET) "$(DESTDIR)$(PREFIX)/bin/$(polishedmap)"
	mkdir -p "$(DESTDIR)$(PREFIX)/share/pixmaps"
	cp $(resdir)/app.xpm "$(DESTDIR)$(PREFIX)/share/pixmaps/polishedmap++48.xpm"
	cp $(resdir)/app-icon.xpm "$(DESTDIR)$(PREFIX)/share/pixmaps/polishedmap++16.xpm"
	mkdir -p "$(DESTDIR)$(PREFIX)/share/applications"
	echo "[Desktop Entry]" > "$(DESKTOP)"
	echo "Name=Polished Map++" >> "$(DESKTOP)"
	echo "Comment=Edit pokecrystal maps and tilesets" >> "$(DESKTOP)"
	echo "Icon=$(PREFIX)/share/pixmaps/polishedmap++48.xpm" >> "$(DESKTOP)"
	echo "Exec=$(PREFIX)/bin/$(polishedmap)" >> "$(DESKTOP)"
	echo "Type=Application" >> "$(DESKTOP)"
	echo "Terminal=false" >> "$(DESKTOP)"

uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/bin/$(polishedmap)"
	rm -f "$(DESTDIR)$(PREFIX)/share/pixmaps/polishedmap++48.xpm"
	rm -f "$(DESTDIR)$(PREFIX)/share/pixmaps/polishedmap++16.xpm"
	rm -f "$(DESKTOP)"
