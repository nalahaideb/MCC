#include <iostream>
#include "lexer.h"
#include "tests.h"
int main()
{

	std::string input = "/* //x=42\n*/";
	Lexer lexer(input);
	std::vector< std::vector< std::string > > output = lexer.lex();
	
	for (const auto & k : output)
	{
		for (const auto & v : k)
		{
			std::cout <<"token = " << v << ", ";
		}
		std::cout << std::endl;
	}
	std::cout << "rest of input: " << input << std::endl;
	return 0;
}
