#include "constants.h"
#include <vector>

/*
  should return a vector of vectors with tokens, each subvector denoting a line of formatted input 
*/
class Lexer
{

 public:
 Lexer(const std::string & input)
	 :tape(input)
	{}
	//std::vector< std::vector > type(const std::string & input);   //determine type 
	std::vector< std::string > strip_line(const std::string & input);
	std::vector< std::vector< std::string > > lex();
    void comment_lexer(std::string & input)
 private:
	//ptr to type_map in constants.h
    constexpr static const auto * tokens = &type_map;	
	std::string tape;
};

//parse as many comments as possible. ill-formatted multi-line comments will trigger an error
//when the parser detects mismatched or nested pairs
void Lexer::comment_lexer(std::string & input)
{
	if (input.empty()) return;
	size_t k = input.find("/*");
	
	//pairs of comment blocks and their respective types. should always follow BCOM -> ECOM, no nesting allowed
	std::vector< std::pair<size_t, int > comment_blocks = {k, BCOM};
	
	while (k != std::string::npos)
	{
		k = input.find("*/");
		std::pair<size_t, int> entry = {};
		if (k == std::string::npos) comment_blocks = {};
		else
		
		
		k = input.find("/*")
			
	}
	
	//if line starts with SCOM, ignore rest of line
	/* int start = 0; */
	/* for (const auto & k : input) */
	/* 	if (k == '\n') break; */
	/* 	else ++start; */
	/* input = input.substr(start, input.size() - 1);	 */
}

/* std::string Lexer::type(const std::string & input) */
/* { */
	
/* } */

std::vector< std::string > Lexer::strip_line(const std::string & input)
{
	bool comment_flag = false;
	if (tokens->find(input[0])->second == SCOM)
		return {""};
	else if (tokens->find(input[0])->second == BCOM)
		comment_flag = true;
	else
		"";
	std::vector< std::string > ret;
	std::string word;
	char k = input[0];
	while (input[k] != '\0')
	{
		if (input[k] == ' ')
		{
			ret.push_back(word);
			word = "";
		}
		//need some way to detect addl comments, run thru and check if comments total > 2 for BCOM/ECOM, anything but newline is allowed for SCOM
		else
			word += input[k];
		++k;
	}
	return ret;
}
