#ifndef HELPER_H
#define HELPER_H

inline bool ISNUM(const char c)
{return (c >= 48 && c <= 57);}

inline bool ISALPHA(const int c)
{return ((c >= 65 && c <= 90) || (c >= 97 && c <= 122));}

inline bool ISUNARY(const int c)
{return c == '!' || c == '-' || c == '+';}
#endif
