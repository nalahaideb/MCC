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
	//std::vector< std::vector > type(const std::vector< std::string > & input);   //determine type 
	std::vector< std::string > strip_line(const std::string & input);
	std::vector< token > type_lexer(const std::string & input);
	std::string block_comment_lexer(std::string & input);
 private:
	//ptr to type_map in constants.h
    constexpr static const auto * tokens = &type_map;	
	std::string tape;
};

//called when a beginning block-comment is detected (/*). scans until it reaches a (*/), returning nonempty string with error msg
//"input" should be a substring
std::string Lexer::block_comment_lexer(std::string & input)
{
	if (input.empty()) return "error line insert_line_here: no end comment";
	size_t k = input.rfind("*/");
    size_t bcom_nest = input.find("/*", 1);
	size_t ecom_nest = input.find("/*");
	
	//k == ecom_nest means no other nested end-block-comment was found,
	//bcom starts +1 of input so no nested begin-block-comment was found
	//returns input - block comment if success, error string if not
	if (k == ecom_nest && bcom_nest == std::string::npos)
		return input.substr(ecom_nest, input.size() - ecom_nest);
	else
	{
		if (bcom_nest != 0) return "error line insert_line_here: nested block comment begin";
		else                return "error line insert_line_here: nested block comment end";
	}
}

std::vector< token > type_lexer(const std::string & input)
{
	
}

//increment line ct by 
std::vector< std::string > Lexer::strip_line(const std::string & input)
{
	size_t line_ct = 0;
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
		else if (input[k] == '\n') //increment line counter
			++line_ct;
		else
			word += input[k];
		++k;
	}
	return ret;
}
