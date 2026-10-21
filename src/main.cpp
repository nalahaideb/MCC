#include <iostream>
#include "lexer.h"
#include "tests.h"
int main()
{
	//if flag == tokens run lexer, if flag == ast run lexer then parser, if flag == typed run lexer then parser then typer, etc...
	std::string input;
	std::cin >> input;
	
	Lexer lexer;
	// lexer.read_file("part1/part1/tokens.mc");
	lexer.read_file(input);
	std::vector< std::string > output = lexer.lex();
	
	// for (const auto & v : output)
	// {
	// 	std::cout << v << std::endl;
	// }
	return 0;
}
