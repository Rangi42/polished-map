#ifndef MAP_EVENTS_H
#define MAP_EVENTS_H

#include <vector>
#include <unordered_map>

#include "utils.h"
#include "event.h"
#include "deep-tile.h"

#define EVENT_PX_SIZE (TILE_SIZE * 2)

class Map_Events {
public:
	enum class Result { MAP_EVENTS_OK, BAD_MAP_EVENTS_FILE, MAP_EVENTS_NULL };
private:
	char _map_name[FL_PATH_MAX];
	std::vector<Event *> _events;
	std::unordered_map<int, Event *> _warps;
	Result _result;
	bool _loaded, _modified;
	std::string _coda;
	int64_t _mod_time;
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
	inline bool other_modified(const char *f) const { return file_modified(f) > _mod_time; }
	inline std::string coda(void) const { return _coda; }
	inline void coda(const std::string &c) { _coda = c; }
	void clear();
	void resize_events(int x, int y, int s) const;
	void refresh_warps(void);
	Result read_events(const char *f);
public:
	static const char *error_message(Result result);
	bool write_event_script(const char *f);
};

#endif
