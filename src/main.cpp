#include <iostream>
#include "lexer.h"
#include "tests.h"
//lexer doesnt actually consume the given input. has caused me some confusion when i came back to this. maybe lexer.tape should be a reference / ptr instead so it actually consumes the string input.
int main()
{

	std::string input;
	input += "/* Part 1 sample: tricky tokens.\nThis comment spans three lines,\nso the next token is on line 4. */\n";
    input += "int ifx = 007;        // 'ifx' is an identifier, not 'if'\n";
	std::string line;
	std::cout << input.size() << std::endl;
	// std::fstream f("part1/part1/tokens.mc");
	// while (std::getline(f, line))
	// {
	// 	input += line;
	// 	std::cout << (line.find('\n') == std::string::npos ? "newline" : "none") << std::endl;
	// }
	Lexer lexer(input);
	std::vector< std::vector< std::string > > output = lexer.lex();
	
	for (const auto & k : output)
	{
		for (const auto & v : k)
		{
			std::cout <<"token: " << v << ", ";
		}
		std::cout << std::endl;
	}
	//std::cout << "rest of input: " << input << std::endl;
	return 0;
}
