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

inline bool ISALNUM(const char c)
{return (ISALPHA(c) || ISNUM(c));}

//strips dir names and extensions from given path, for testing
inline std::string stem(const std::string & path)
{
	size_t slash = path.find_last_of('/');
	std::string name = (slash == std::string::npos) ? path : path.substr(slash + 1);
	size_t dot = name.rfind('.');
	return (dot == std::string::npos || dot == 0) ? name : name.substr(0, dot);
}

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
#endif
