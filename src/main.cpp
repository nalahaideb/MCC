#include <iostream>
#include "lexer.h"
#include "parser.h"
#include "tests.h"
int main(int argc, char * argv[])
{
	//if flag == tokens run lexer, if flag == ast run lexer then parser, if flag == typed run lexer then parser then typer, etc...
	int filenum = -1;
	bool errors = false;
	std::string file = "";
	for (int i = 0; i < argc; ++i)
	{
		std::string z = static_cast<std::string>(argv[i]);
		std::cout << z << std::endl;
		if (z.find(".mc") != std::string::npos) filenum = i;
	}
	if (filenum > 1)
		file = static_cast<std::string>(argv[filenum]);
	else
	{
		errors = 1; return errors;
	}	
	try
	{
		// std::cout << static_cast<std::string>(argv[1]).size() - 2 << std::endl;
		switch (static_cast<std::string>(argv[1]).size() - 2)
		{
		case 3: //ast
			{
				grammar * g = new grammar;
				Parser p({}, g);
				delete g;
				break;
			}
		case 5: //tokens
			{
				Lexer lexer;
				lexer.read_file(argv[2]);
				std::vector< std::pair< std::string, token > > r = lexer.lex();
				break;
			}
		default:
			std::cout << "INVALID OPTION\n";
			break;
		}
	}
	catch (...)
	{
		errors = true;
	}
	//needs to return 1 on error 
	return errors;
}
