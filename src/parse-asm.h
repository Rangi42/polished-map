#ifndef PARSE_ASM_H
#define PARSE_ASM_H

#include <vector>

class Parsed_Asm {
public:
	enum class Result { ASM_OK, ASM_BAD_FILE, ASM_OVERFLOW, ASM_NULL };
private:
	std::vector<uint8_t> _data;
	Result _result;
public:
	Parsed_Asm(const char *f);
	inline ~Parsed_Asm() {}
	inline size_t size(void) const { return _data.size(); }
	inline uint8_t get(size_t i) const { return _data.at(i); }
	inline Result result(void) const { return _result; }
private:
	Result parse_asm(const char *f);
};

#endif
