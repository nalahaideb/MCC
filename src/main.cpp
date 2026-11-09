#include <iostream>
#include "lexer.h"
#include "parser.cpp"
int main(int argc, char * argv[])
{
	//if flag == tokens run lexer, if flag == ast run lexer then parser, if flag == typed run lexer then parser then typer, etc...
	int filenum = -1;
	bool errors = false;
	std::string file = "";
	for (int i = 0; i < argc; ++i)
	{
		std::string z = static_cast<std::string>(argv[i]);
		//std::cout << z << std::endl;
		if (z.find(".mc") != std::string::npos) filenum = i;
	}
	if (filenum > 1)
		file = static_cast<std::string>(argv[filenum]);
	else
	{
		errors = true; return errors;
	}	
	try
	{
		switch (static_cast<std::string>(argv[1]).size() - 2)
		{
		case 6: //tokens
			{
				Lexer lexer;
				lexer.read_file(argv[2]);
				std::vector< std::pair< std::string, token > > r = lexer.lex();
				for (const auto & k : r)
					std::cout << k.first << ", " << k.second.printable() << std::endl;
				lexer.to_file();
				break;
			}
		case 3: //ast
			{
				Lexer lexer;
				lexer.read_file(argv[2]);
				std::vector< std::pair< std::string, token > > r = lexer.lex();
				grammar * g = new grammar;
				std::vector< token > list;
				for (const auto & t : r)
					list.push_back(t.second);
				Parser p(list, g);
				delete g;
				break;
			}
		default:
			std::cerr << "INVALID OPTION\n";
			break;
		}
	}
	catch (Error & e)
	{
		std::ofstream f(file.substr(0, file.size() - 3) + ".err");
	    f << e.message << std::endl;
		errors = true;
	}
	//needs to return 1 on error 
	return errors;
}
