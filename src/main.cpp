#include <iostream>
#include "lexer.h"
#include "parser.cpp"
#include "symtable.cpp"

int main(int argc, char * argv[])
{
	//if flag == tokens -> lexer
	//if flag == ast    -> lexer -> parser
	//if flag == check  -> lexer -> parser -> typer, etc...
	int filenum = -1;
	bool errors = false;
	std::string file = "";
	//switched manually for checking specific error types
	bool debug_prints = false;
	for (int i = 0; i < argc; ++i)
	{
		std::string z = static_cast<std::string>(argv[i]);
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
			    p.print_tree();
				break;
			}
		case 5: //type checker
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
				Symtable symtable;

				std::ofstream out("ast.dot");
				out << dot(p.get_root());
				out.close();

				symtable.check_program(p.get_root());
				std::cout << "OK";
				break;
			}
		default:
			std::cerr << "INVALID OPTION\n";
			break;
		}
	}
	catch (Error & e)
	{
		std::string s = argv[1];
		//get rid of "--" for check on std::cerr printout vs file output
		s.replace(0, 2, "");

		//part 1 requires output to file, part 2 and on require std::cerr output
		if (s == "tokens")
		{
			std::ofstream f(file.substr(0, file.size() - 3) + ".err");
			f << e.message << std::endl;
		}
		else
		{
			//standard error messages are just "error: line x:" so this
			//converts my other error types to that
			if (debug_prints)
			{
				std::cout << e.message << std::endl;
			}
			else
			{
		    unsigned int start = e.message.find("ERROR");
			unsigned int end = e.message.rfind(':');
			std::string s;
			for (unsigned int i = start; i <= end; ++i)
				if (ISALPHA(e.message[i]))
					s += std::tolower(e.message[i]);
				else
					s += e.message[i];
			std::cerr << s << std::endl;
			}
		}
		errors = true;
	}
	
	//needs to return 1 on error 
	return errors;
}
