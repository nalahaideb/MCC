#include <iostream>
#include "lexer.h"
#include "tests.h"
int main(int argc, char * argv[])
{
	//if flag == tokens run lexer, if flag == ast run lexer then parser, if flag == typed run lexer then parser then typer, etc...
	std::string input;
	Lexer lexer;

	//switch to handle different stages of compiler
	int filenum = -1;
	for (int i = 0; i < argc; ++i)
	{
		std::string z = static_cast<std::string>(argv[i]);
		if (z.find(".mc") != std::string::npos) filenum = i;
	}
	std::string flag = argv[3];
	
	//read file arg
	if (filenum >= 0)
	{
		lexer.read_file(argv[2]);
		std::vector< std::string > output = lexer.lex();
	}

	switch (flag.size())
	{
	case 3: //parsing
		//Parser parser;
		break;
	case 5: //basic lexer
		lexer.read_file(argv[2]);
		std::vector< std::string > output = lexer.lex();
		break;
	case:
		break;
	case:
		break;
	}
	//needs to return 1 on error 
	return 0;
}
