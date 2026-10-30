#ifndef TOKEN_H
#define TOKEN_H
#include <string>

struct token
{
	std::string value;
	std::string raw;
token(const std::string v, const std::string r)
	: value(v), raw(r) {}
};
#endif
