#ifndef SIGNATURE_H
#define SIGNATURE_H
#include <vector>

typedef std::string name;
typedef std::string type;

struct signature
{
	type ret;
	std::vector< type > params;      // parameter types, in order
	std::string line;
};

#endif
