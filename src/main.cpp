#include <iostream>
#include "lexer.h"

int main()
{
	std::string test = "agaga;/**/bbb; /*tetetete*/ hello world;";
	Lexer lexer(test);
	int lines = 0;
	lexer.block_comment_lexer(test, lines);
	std::cout << lines << std::endl;
	return 0;
}
