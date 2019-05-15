#ifndef MAP_EVENTS_H
#define MAP_EVENTS_H

#include <vector>

#include "utils.h"
#include "event.h"
#include "tile.h"

#define EVENT_PX_SIZE (TILE_SIZE * 2)

class Map_Events {
public:
	enum Result { MAP_EVENTS_OK, BAD_MAP_EVENTS_FILE, MAP_EVENTS_NULL };
private:
	std::vector<Event *> _events;
	Result _result;
	bool _loaded, _modified;
	std::string _coda;
public:
	Map_Events();
	~Map_Events();
	inline size_t size(void) const { return _events.size(); }
	inline Event *event(size_t i) const { return _events[i]; }
	inline void event(size_t i, Event *e) { _events[i] = e; }
	inline Result result(void) const { return _result; }
	inline bool loaded(void) const { return _loaded; }
	inline void loaded(bool l) { _loaded = l; }
	inline bool modified(void) const { return _modified; }
	inline void modified(bool m) { _modified = m; }
	inline std::string coda(void) const { return _coda; }
	inline void coda(std::string c) { _coda = c; }
	void clear();
	void resize_events(int x, int y, int s);
	Result read_events(const char *f, bool allow_warp);
public:
	static const char *error_message(Result result);
	bool write_event_script(const char *f);
};

#endif
