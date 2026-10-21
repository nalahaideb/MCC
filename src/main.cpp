#include <iostream>
#include "lexer.h"
#include "tests.h"
int main()
{

	std::string input;
	std::string line;
	std::fstream f("part1/part1/tokens.mc");
	while (std::getline(f, line))
	{
		input += line + " \n ";
		//std::cout << line << std::endl;
	}
	std::cout << input << std::endl;
	Lexer lexer(input);
	
	std::vector< std::string > output = lexer.lex();
	
	for (const auto & v : output)
	{
		std::cout << v << std::endl;
	}
	
	lexer.to_file("test.mc");
	return 0;
}
