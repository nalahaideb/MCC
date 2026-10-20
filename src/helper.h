#ifndef HELPER_H
#define HELPER_H

inline bool ISNUM(const char c)
{return (c >= 48 && c <= 57);}

inline bool ISALPHA(const int c)
{return ((c >= 65 && c <= 90) || (c >= 97 && c <= 122));}

inline int STR_INT(const std::string & s)
{
	int pow = 1, k = 0;	    
	int size = s.size() - 1;
	for (int v = 0; v < size; ++v)
		pow *= 10;
	for (const auto & d : s)
	{
		k += pow * (d-48);
		pow /= 10;
	}
	return k;
}

inline std::string INT_STR(int k)
{
	std::string ret;
	while (k > 0)
	{
		ret += char(k % 10);
		k /= 10;
	}
	return ret;
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
