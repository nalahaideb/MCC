#include "constants.h"
#include <vector>
/*
  TODO:
  [] make it lex with no spaces
  [] strip comments
  [] lex tokens into vectors based on line
  [] block comments keep track of line count
*/

/*
  should return a vector of vectors with tokens, each subvector denoting a line of formatted input 
*/
class Lexer
{

 public:
 Lexer(const std::string & input)
	 :tape(input)
	{}
	//std::vector< std::vector > type(const std::vector< std::string > & input); //determine type 
	std::vector< std::string > strip_line(const std::string & input);
	std::vector< token > type_lexer(std::string & input);
    void block_comment_lexer(std::string & input, int & line_ct);
	std::string lex_var(std::string & input);
	std::string lex_num(std::string & input);
 private:
	//ptr to type_map in constants.h
    constexpr static const auto * tok_type = &type_map;
    constexpr static const auto * tok_op = &operators;
    /* constexpr static const auto * tok_num = &type_map;	 */
    /* constexpr static const auto * tok_char = &type_map;	 */
	std::string tape;
};

//preprocessing, removes all block comments, throws error otherwise
void Lexer::block_comment_lexer(std::string & input, int & line_ct)
{
	//bool nest_flag = false;
	size_t k = input.find("/*");
	//no block comment, block comment ends will throw errors during lexing
	if (k == std::string::npos) return;

	//acts as a stack, has at most two elements before throwing error
	std::vector< size_t > comment_stk = {k};
	while (input[k] != '\0' && k != std::string::npos)
	{
		if (k < input.size() - 1 && input[k] == '*' && input[k+1] == '/')
		{
			std::cout << input.size() - k << ", " << k << std::endl;
			input = input.substr(k, input.size() - k);
			comment_stk = {};
		    k = input.find("/*");
		}
		else if (comment_stk.size() != 1 && k < input.size() - 1 && input[k] == '/' && input[k+1] == '*')
		{//above case already handles beginning comments, should throw error
			std::cout << "INVALID SYNTAX, NESTING BLOCK COMMENT\n";
			throw -1;
		}
		else if (k == '\n')
		{
			++line_ct, ++k;
		}
		else
			++k;
	}
	std::cout << "input = " << input << std::endl;
}


/* std::vector< std::string > Lexer::strip_line(const std::string & input) */
/* { */
/* 	size_t line_ct = 0; */
/* 	std::vector< std::string > ret; */
/* 	std::string word; */
/* 	char k = input[0]; */
/* 	while (input[k] != '\0') */
/* 	{ */
/* 		if (input[k] == ' ') ++k; //skip spaces */
/* 		else if (input[k]) //character match */
		    
/* 	} */
/* 	return ret; */
/* } */
