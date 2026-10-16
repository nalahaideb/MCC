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
	if (input.empty()) return;
	
	std::vector< std::pair< int, int > > comment_blocks;
	size_t k = 0;
	//find beginning / end delimiters for block comments
	while (input[k] != '\0')
	{
		if (k < input.size() - 1 && input[k] == '/' && input[k+1] == '*')
			comment_blocks.push_back({BCOM, k});
		else if (k < input.size() - 1 && input[k] == '*' && input[k+1] == '/')
			comment_blocks.push_back({ECOM, k});//have to return substring of ECOM + 1
		else if (input[k] == '\n')
			++line_ct;
	}
	//should be 2n comment delimiters per n comment section blocks
	if (comment_blocks.size() % 2) throw -1;
	std::string replacement;
	int k = 0;
	for (int c = 1; c < comment_blocks.size(); c += 2)
	{
		const auto & begin = comment_blocks[c - 1];
		const auto & end = comment_blocks[c];
		
		//check for sequential comment block pairs
		if (begin.first != BCOM || end.first != ECOM)
		{
			throw -1;
		}
		else //ignore chars found between the positions of the above comment blocks
		{
			replacement += input.substr(input, begin.second) + ' ';
		}
	}
	//string without (correctly formatted) block comments 
	input = replacement;
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
