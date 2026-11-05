#ifndef TOKEN_H
#define TOKEN_H
#include <string>

//todo: [] fix semi printing an extra space on .out file (here or in lexer)
struct token
{
	token(const std::string r, const std::string v)
		:type(r), value(v) {}
	
	std::string printable() const
	{
		if (value != "EOF" && value != "SEMI")
			return value + " " + type;
		else
			return value;
	}
	bool empty()
	{ return type.empty() && value.empty(); }
	std::string type;
	std::string value;
};

#endif
