#ifndef TOKEN_H
#define TOKEN_H
#include <string>

struct token
{
	token(const std::string r, const std::string v)
		:raw(r), value(v) {}
	
	std::string printable() const
	{ return value + " " + raw; }
	bool empty()
	{ return raw.empty() && value.empty(); }
	std::string raw;
	std::string value;
};
#endif
