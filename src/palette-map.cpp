#include <string>
#include <fstream>
#include <sstream>
#include <array>

#pragma warning(push, 0)
#include <FL/fl_utf8.h>
#pragma warning(pop)

#include "utils.h"
#include "config.h"
#include "palette-map.h"

Palette_Map::Palette_Map() : _palette(), _palette_size(0), _result(Result::PALETTE_NULL) {
	clear();
}

void Palette_Map::clear() {
	std::fill_n(_palette, MAX_NUM_TILES, Palette::UNDEFINED);
	_palette_size = 0;
	_result = Result::PALETTE_NULL;
}

Palette_Map::Result Palette_Map::read_from(const char *f) {
	clear();

	std::ifstream ifs(f);
	if (!ifs.good()) {
		Config::monochrome(true);
	}

	if (Config::monochrome()) {
		_palette_size = Config::allow_256_tiles() ? MAX_NUM_TILES : 0x60;
		std::fill_n(_palette, _palette_size, Palette::MONOCHROME);
		return (_result = Result::PALETTE_OK);
	}
	std::string prefix(Config::palette_macro());
	while (ifs.good()) {
		std::string line;
		std::getline(ifs, line);
		if (!starts_with(line, prefix)) { continue; }
		remove_comment(line);
		std::istringstream lss(line);
		std::string token;
		std::getline(lss, token, ','); // skip bank ID
		while (std::getline(lss, token, ',')) {
			if (_palette_size == MAX_NUM_TILES) { return (_result = Result::PALETTE_TOO_LONG); }
			trim(token);
			if      (token == "GRAY")            { _palette[_palette_size++] = Palette::GRAY; }
			else if (token == "RED")             { _palette[_palette_size++] = Palette::RED; }
			else if (token == "GREEN")           { _palette[_palette_size++] = Palette::GREEN; }
			else if (token == "WATER")           { _palette[_palette_size++] = Palette::WATER; }
			else if (token == "YELLOW")          { _palette[_palette_size++] = Palette::YELLOW; }
			else if (token == "BROWN")           { _palette[_palette_size++] = Palette::BROWN; }
			else if (token == "ROOF")            { _palette[_palette_size++] = Palette::ROOF; }
			else if (token == "TEXT")            { _palette[_palette_size++] = Palette::TEXT; }
			else if (token == "PRIORITY_GRAY")   { _palette[_palette_size++] = Palette::PRIORITY_GRAY; }
			else if (token == "PRIORITY_RED")    { _palette[_palette_size++] = Palette::PRIORITY_RED; }
			else if (token == "PRIORITY_GREEN")  { _palette[_palette_size++] = Palette::PRIORITY_GREEN; }
			else if (token == "PRIORITY_WATER")  { _palette[_palette_size++] = Palette::PRIORITY_WATER; }
			else if (token == "PRIORITY_YELLOW") { _palette[_palette_size++] = Palette::PRIORITY_YELLOW; }
			else if (token == "PRIORITY_BROWN")  { _palette[_palette_size++] = Palette::PRIORITY_BROWN; }
			else if (token == "PRIORITY_ROOF")   { _palette[_palette_size++] = Palette::PRIORITY_ROOF; }
			else if (token == "PRIORITY_TEXT")   { _palette[_palette_size++] = Palette::PRIORITY_TEXT; }
			else                                 { return (_result = Result::BAD_PALETTE_NAME); }
			if (_palette[_palette_size-1] >= Palette::PRIORITY_GRAY) {
				Config::allow_priority(true);
			}
		}
	}
	return (_result = Result::PALETTE_OK);
}

const char *Palette_Map::error_message(Result result) {
	switch (result) {
	case Result::PALETTE_OK:
		return "OK.";
	case Result::BAD_PALETTE_FILE:
		return "Cannot open file.";
	case Result::BAD_PALETTE_NAME:
		return "Invalid color name.";
	case Result::PALETTE_TOO_LONG:
		return "More than 256 colors defined.";
	case Result::PALETTE_NULL:
		return "No palette file chosen.";
	default:
		return "Unspecified error.";
	}
}

bool Palette_Map::write_palette_map(const char *f) const {
	FILE *file = fl_fopen(f, "wb");
	if (!file) { return false; }
	size_t n = MAX_NUM_TILES;
	while (_palette[n-1] == Palette::UNDEFINED) { n--; }
	const char *prefix = Config::palette_macro();
	for (size_t i = 0; i < n; i++) {
		if (!Config::allow_256_tiles() && (i == 0x60 || i == 0xe0)) {
			fputs(Config::allow_priority()
				? "\nrept 32\n\tdb $ff\nendr\n\n"
				: "\nrept 16\n\tdb $ff\nendr\n\n", file);
			i += 0x1f;
			continue;
		}
		if (i % PALETTES_PER_LINE == 0) {
			fputs(prefix, file);
			fputc(' ', file);
			fputc(i < 0x80 ? '0' : '1', file);
		}
		fputs(", ", file);
		switch (_palette[i]) {
		case Palette::GRAY:            fputs("GRAY", file); break;
		case Palette::RED:             fputs("RED", file); break;
		case Palette::GREEN:           fputs("GREEN", file); break;
		case Palette::WATER:           fputs("WATER", file); break;
		case Palette::YELLOW:          fputs("YELLOW", file); break;
		case Palette::BROWN:           fputs("BROWN", file); break;
		case Palette::ROOF:            fputs("ROOF", file); break;
		case Palette::TEXT: default:   fputs("TEXT", file); break;
		case Palette::PRIORITY_GRAY:   fputs("PRIORITY_GRAY", file); break;
		case Palette::PRIORITY_RED:    fputs("PRIORITY_RED", file); break;
		case Palette::PRIORITY_GREEN:  fputs("PRIORITY_GREEN", file); break;
		case Palette::PRIORITY_WATER:  fputs("PRIORITY_WATER", file); break;
		case Palette::PRIORITY_YELLOW: fputs("PRIORITY_YELLOW", file); break;
		case Palette::PRIORITY_BROWN:  fputs("PRIORITY_BROWN", file); break;
		case Palette::PRIORITY_ROOF:   fputs("PRIORITY_ROOF", file); break;
		case Palette::PRIORITY_TEXT:   fputs("PRIORITY_TEXT", file); break;
		}
		if (i % PALETTES_PER_LINE == PALETTES_PER_LINE - 1) {
			fputc('\n', file);
		}
	}
	if (!Config::allow_priority()) {
		if (n % 2) {
			fputs(", TEXT\n", file);
		}
		else if (n % PALETTES_PER_LINE) {
			fputc('\n', file);
		}
	}
	fclose(file);
	return true;
}
