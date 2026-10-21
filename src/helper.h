#ifndef HELPER_H
#define HELPER_H

inline bool ISNUM(const char c)
{return (c >= 48 && c <= 57);}

inline bool ISALPHA(const int c)
{return ((c >= 65 && c <= 90) || (c >= 97 && c <= 122));}

inline bool ISUNARY(const int c)
{return c == '-' || c == '!';}

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
