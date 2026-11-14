#ifndef TOKEN_H
#define TOKEN_H
#include <string>

//todo: [] fix semi printing an extra space on .out file (here or in lexer)
struct token
{
	token(const std::string r, const std::string v, const std::string l = "-1")
		:type(r), value(v), line(l) {}
	
	std::string printable() const
	{
		if (value == "IDENT")
			return type + " " + value;
		else
			return type;
	}
	bool empty()
	{ return type.empty() && value.empty(); }
	std::string type;
	std::string value;
	std::string line;
};

#endif
