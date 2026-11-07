#ifndef HELPER_H
#define HELPER_H
#include <cctype> //for std::tolower
#include "constants.h"
typedef std::string symbol;

inline bool ISNUM(const char c)
{return (c >= 48 && c <= 57);}

inline bool ISALPHA(const int c)
{return ((c >= 65 && c <= 90) || (c >= 97 && c <= 122));}

inline bool ISUNARY(const int c)
{return c == '-' || c == '!';}

static std::string lowercase(std::string s)
{
	for (char & c : s) c = std::tolower(static_cast< unsigned char >(c));
	return s;
}

//reverse map for leaf nodes in AST
//uses existing maps operators / keyword_map
//this is really wasteful for every single terminal, gotta change it or rework maps
static std::string readout(const symbol & terminal)
{
	static const std::unordered_map< symbol, std::string > text = []
	{
		std::unordered_map< symbol, std::string > m;
		for (const auto & o : operators)
			m[o.second] = "'" + o.first + "'";
		for (const auto & k : keyword_map)
			m[k.second] = "'" + k.first + "'";
		m["IDENT"] = "identifier";
		m["NUM"]   = "number";
		m["TYPE"]  = "type";
		m["EOF"]   = "end of file";
		return m;
	}();
	const auto it = text.find(terminal);
	return it != text.end() ? it->second : terminal;
}

//block statements or groupers should start and end with the same thing
//BEGIN = begin symbol, END = end symbol
//if nestable, will avoid nested checks, i.e. "(())" != "/* /* */ */"
/* inline bool IS_VALID_BLOCK(std::string & s, std::string BEGIN, std::string END, bool nestable) */
/* { */
/*     size_t start = s.find(BEGIN), stop = s.find(END); */
/* 	if (start == std::string::npos || stop == std::string::npos) */
/* 		return false; */
/* 	else */
/* 	{ */
/* 		if (!nestable) */
/* 	} */
/* } */
#endif
