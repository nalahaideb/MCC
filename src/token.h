#ifndef TOKEN_H
#define TOKEN_H
#include <string>

struct token
{
	token(const std::string r, const std::string v)
		:type(r), value(v) {}
	
	std::string printable() const
	{ return "<token:" + type + ", " + value + ">"; }
	bool empty()
	{ return type.empty() && value.empty(); }
	std::string type;
	std::string value;
};
#endif
