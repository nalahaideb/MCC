#include <iostream>
#include "lexer.h"
#include "tests.h"
int main()
{
	Lexer lexer("");
	std::cout << "============================TEST NUMS============================" << std::endl;
	test_numbers(lexer);
	std::cout << "============================TEST VARS============================" << std::endl;
	test_ident(lexer);
	std::cout << "============================TEST OPRS============================" << std::endl;
	test_operator(lexer);
	std::cout << "============================TEST COMPOUND============================" << std::endl;
	
	return 0;
}
