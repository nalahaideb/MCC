#ifndef PRODUCTION_H
#define PRODUCTION_H
#include <vector>
#include <string>
#include <iostream>

typedef std::string symbol;

struct production
{
    symbol prod_symbol;
	std::vector< symbol > produced; //an empty vector = epsilon
production(const symbol n, const std::vector< symbol > & p)
	:prod_symbol(n), produced(p) {}
};

production test = {"hello", {"test", "test1"}};

#endif
