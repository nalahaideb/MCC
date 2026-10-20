#ifndef TESTS_H
#define TESTS_H
#include "lexer.h"

void test_ident(Lexer & lexer)
{
	std::vector<std::string> input = {"agAga", "_1234___", "_", "_hello_", "hello_1_WORLD", "b_1_n6Us42__", "__", "_a/23*-_", "42", "14aaaaa__"};
	std::string p = "";
	int lines = 0;
	std::cout << lines << std::endl;
	for (auto & f : input)
	{
		std::cout << "testing " << f << "... ";
		try
		{
			std::cout << "PASS:" << lexer.lex_var(f) << std::endl;
		}
		catch (Error & e)
		{
			std::cout << "FAIL:" << e.message << std::endl;
		}
	}
}

//add float functionality in the future
void test_numbers(Lexer & lexer)
{
	std::vector< std::string > input = {"123", "0", "4444444", "9487219", "abcd", "1b2a", "a1234", "1.42", "8000000000000", "11111_", "1+2", "-34", "0+-0", "00000-", "000012"};
    for (auto & s : input)
	{
		std::cout << "testing: " << s << "... ";
		try
		{
			std::cout << "PASS: " << lexer.lex_num(s) << std::endl;
				}
		catch (Error & e) {std::cout << "FAIL: " << e.message << std::endl;}
	}
}

void test_operator(Lexer & lexer)
{
	//auto temp = operators;
	//cant be bothered to make the beauty of OOP work right now
    std::unordered_map< std::string, std::string > temp =
		{
		{"<", "LT"}, {">", "GT"}, {"==","EQ"},{"<=","LE"},
		{"!=", "NEQ"},
		{">=","GE"}, {"!","NOT"}, {"||","OR"}, {"&&","AND"},
		{"(","LPAREN"},
		{")","RPAREN"}, {"{","LBRACE"}, {"}","RBRACE"},
		{"[", "LBRACK"},
		{"", "RBRACK"}, {"+","PLUS"}, {"-","MINUS"}, {"/","DIV"},
		{"*", "MULT"},
		{"%","MOD"}, {".","DECIMAL"}, {"=","ASSIGN"},
		{"++", "SINC"}, {"--", "SDEC"}, {"+=", "CADD"}, {"-=", "CMIN"},
		{"/=", "CDIV"}, {"%=", "CMOD"}, {"*=", "CMULT"}
	};
;
	//std::vector< std::string > input = {};
    for (auto & [k,v] : temp)
	{
		std::cout << "testing: " << v << "... ";
		try
		{
			auto f = k;
			std::cout << "PASS:" << lexer.lex_opr(f) << std::endl;
		}
		catch (Error & e)
		{std::cout << "FAIL: " << e.message << std::endl;}
	}
	//should fail all of these
	temp = { {"0+02", ""}, {"(++)", ""},{"(%+)", ""},{"//", ""},{"=-", ""},{"!-", ""},{"*+", ""},{"+1", ""},{":", ""},{"<>", ""},{"---", ""},};
    for (auto & [k,v] : temp)
	{
		std::cout << "testing: " << v << "... ";
		try
		{
			auto f = k;
			std::cout << "PASS:" << lexer.lex_opr(f) <<std::endl;
		}
		catch (Error & e)
		{std::cout << "FAIL: " << e.message << std::endl;}
	}	
}

void test_single_comment(Lexer & lexer)
{
	std::vector< std::string > input = {"//\n", "///////\n", "/!!!!", "//test1234\n", "/*1234////*/", "///*!@#$%^&*)(*&^%$#*/\n"};
    for (auto & s : input)
	{
		std::cout << "testing: " << s << "... ";
		try
		{
			int k = 0;
			lexer.single_comment_lexer(s, k);
			std::cout << "PASS: "
					  << s << std::endl;
		}
		catch (Error & e) {std::cout << "FAIL: " << e.message << std::endl;}
	}
}

//some of these "pass" but are really supposed to fail in a later iteration 
void test_block_comment(Lexer & lexer)
{
	std::vector< std::string > input = {"/**/", "/*!@#!$#!%!#%!#^*/", "/*             */", "*/ /*", "/* */ */", "/*////////*/", "/*" };
    for (auto & s : input)
	{
		std::cout << "testing: " << s << "... ";
		try
		{
			int k = 0;
			lexer.block_comment_lexer(s, k);
			std::cout << "PASS: "
					  << s << std::endl;
		}
		catch (Error & e) {std::cout << "FAIL: " << e.message << std::endl;}
	}
}

/* void test_bool(Lexer & lexer) */
/* { */
	/* std::vector< std::string > input = {""}; */
    /* for (auto & s : input) */
	/* { */
	/* 	std::cout << "testing: " << s << "... "; */
	/* 	try */
	/* 	{ */
	/* 		std::cout << "PASS:"; */
	/* 	} */
	/* 	catch (Error & e) {std::cout << "FAIL: " << e.message << std::endl;} */
	/* } */
/* } */

/* void test_keyword(Lexer & lexer) */
/* { */
	/* std::vector< std::string > input = {""}; */
    /* for (auto & s : input) */
	/* { */
	/* 	std::cout << "testing: " << s << "... "; */
	/* 	try */
	/* 	{ */
	/* 		std::cout << "PASS:"; */
	/* 	} */
	/* 	catch (Error & e) {std::cout << "FAIL: " << e.message << std::endl;} */
	/* } */
/* } */

/* void test_block(Lexer & lexer) */
/* { */
	/* std::vector< std::string > input = {""}; */
    /* for (auto & s : input) */
	/* { */
	/* 	std::cout << "testing: " << s << "... "; */
	/* 	try */
	/* 	{ */
	/* 		std::cout << "PASS:"; */
	/* 	} */
	/* 	catch (Error & e) {std::cout << "FAIL: " << e.message << std::endl;} */
	/* } */
/* } */

/* void test_expression(Lexer & lexer) */
/* { */
	/* std::vector< std::string > input = {""}; */
    /* for (auto & s : input) */
	/* { */
	/* 	std::cout << "testing: " << s << "... "; */
	/* 	try */
	/* 	{ */
	/* 		std::cout << "PASS:"; */
	/* 	} */
	/* 	catch (Error & e) {std::cout << "FAIL: " << e.message << std::endl;} */
	/* } */
/* } */

/* void test_assignment(Lexer & lexer) */
/* { */
	/* std::vector< std::string > input = {""}; */
    /* for (auto & s : input) */
	/* { */
	/* 	std::cout << "testing: " << s << "... "; */
	/* 	try */
	/* 	{ */
	/* 		std::cout << "PASS:"; */
	/* 	} */
	/* 	catch (Error & e) {std::cout << "FAIL: " << e.message << std::endl;} */
	/* } */
/* } */

/* void test_statement(Lexer & lexer) */
/* { */
	/* std::vector< std::string > input = {""}; */
    /* for (auto & s : input) */
	/* { */
	/* 	std::cout << "testing: " << s << "... "; */
	/* 	try */
	/* 	{ */
	/* 		std::cout << "PASS:"; */
	/* 	} */
	/* 	catch (Error & e) {std::cout << "FAIL: " << e.message << std::endl;} */
	/* } */
/* } */

/* void test_template(Lexer & lexer) */
/* { */
/* std::vector< std::string > input = {""}; */
/* for (auto & s : input) */
/* { */
/* 	std::cout << "testing: " << s << "... "; */
/* 	try */
/* 	{ */
/* 		std::cout << "PASS:"; */
/* 	} */
/* 	catch (Error & e) {std::cout << "FAIL: " << e.message << std::endl;} */
/* } */
/* } */

void test_all()
{
	Lexer lexer("");
	std::cout << "============================TEST NUMS============================" << std::endl;
	test_numbers(lexer);
	std::cout << "============================TEST VARS============================" << std::endl;
	test_ident(lexer);
	std::cout << "============================TEST OPRS============================" << std::endl;
	test_operator(lexer);
	std::cout << "============================TEST SINGLE COMMENT============================" << std::endl;
	test_single_comment(lexer);
	std::cout << "============================TEST BLOCK COMMENT============================" << std::endl;
	test_block_comment(lexer);
}
#endif
