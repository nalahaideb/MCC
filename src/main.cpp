#include <iostream>
#include "lexer.h"
#include "tests.h"
int main()
{

	std::string input = "x = 42\n";
	Lexer lexer(input);
	const auto output = lexer.lex();

	// for (const auto & k : output)
	// {
	// 	for (const auto & v : k)
	// 	{
	// 		std::cout << v << ", ";
	// 	}
	// 	std::cout << std::endl;
	// }
	return 0;
}
