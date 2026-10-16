#include <iostream>
#include "lexer.h"

int main()
{
	std::string test = "/**/";
	Lexer lexer(test);
	int lines = 0;
	lexer.block_comment_lexer(test, lines);
	std::cout << lines << std::endl;
	return 0;
}
