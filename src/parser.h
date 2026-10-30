#ifndef PARSER_H
#define PARSER_h
#include <iostream>
#include <sstream>
#include <map>
#include <unordered_map>
#include "error.h"

//==============================================================================
/*
  OVERVIEW:

*/
//==============================================================================

//==============================================================================
/*
  TODO:
  
  [] generates a derivation table from given rules
     [] generate nullable sets
	 [] generate first sets
	 [] generate follow sets
     [] establish precedence rules for forms in the table
	 [] implement parser-generation algorithm
	 [] verify 
  [] constructs syntax tree
     [] traversal and creation of tree respects precedence rules
	 [] similar tree derivations can be replicated
	 [] checks tree against derivation table to ensure no parsing error
  [] output rewritten grammar to "docs/grammar.md"
  (with exception of dangling else, but that could be associated with nearest "if" like c/c++/java)

  
  OPTIMIZATIONS:
  [] add parentheses around return value for type of expression, "return n > 0;" can be written as "return (n>0);"
  [] 
  []
  []
*/

//==============================================================================
class Parser
{
 public:

 private:
	//follow
	
	//first
	
	//nullable
	std::unordered_map< 
};
#endif
