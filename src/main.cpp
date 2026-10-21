#include <iostream>
#include "lexer.h"
#include "tests.h"
int main(int argc, char * argv[])
{
	//if flag == tokens run lexer, if flag == ast run lexer then parser, if flag == typed run lexer then parser then typer, etc...
	std::string input;
	Lexer lexer;

	int filenum = -1;
	for (int i =0; i < argc; ++i)
	{
		std::string z = static_cast<std::string>(argv[i]);
		if (z.find(".mc") != std::string::npos) filenum = i;
	}
	//read file arg
	if (filenum >= 0)
	{
		lexer.read_file(argv[2]);
		std::vector< std::string > output = lexer.lex();
	}	
	// for (const auto & v : output)
	// {
	// 	std::cout << v << std::endl;
	// }
	return 0;
}
