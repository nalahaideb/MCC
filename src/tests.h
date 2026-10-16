#ifndef TESTS_H
#define TESTS_H
#include "lexer.h"

void test_ident()
{
	std::vector<std::string> input = {"agaga", "_1234___", "_", "_hello_", "hello_1", "b_1_n6us42__", "__", "_a/23*-_", "42", "14aaaaa__"};
	std::string p = "";
	Lexer lexer(p);
	int lines = 0;
	std::cout << lines << std::endl;
	for (const auto & f : input)
	{
		std::cout << "testing " << f << "... ";
		try
		{
			std::cout << lexer.lex_var(f) << std::endl;
		}
		catch (Error & e)
		{
			std::cout << "FAIL:" << e.message << std::endl;
		}
	}
}

void test_block_comment()
{

}

void test_single_comment()
{

}

void test_numbers()
{

}

void test_bool()
{

}

void test_keyword()
{

}

#endif
