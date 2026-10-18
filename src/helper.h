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
#endif
