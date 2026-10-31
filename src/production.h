#ifndef PRODUCTION_H
#define PRODUCTION_H
#include <vector>
#include <string>
#include <iostream>

typedef std::string symbol;

struct production
{
    symbol eval;
	std::vector< symbol > produced; //an empty vector = epsilon
production(const symbol n, const std::vector< symbol > & p)
	:eval(n), produced(p) {}
};

production test = {"hello", {"test", "test1"}};

//std::cout << test.eval << std::endl;

#endif
