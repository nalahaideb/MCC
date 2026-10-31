#ifndef PARSER_H
#define PARSER_h
#include <iostream>
#include <unordered_map>
#include <set>
#include "node.h"
#include "production.h"
#include "token.h"
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
  [] add parentheses around return value for type of expression, for example
     "return n > 0;" can be written as "return (n > 0);"
  [] 
  []
  []
*/

//==============================================================================
class Parser
{
 public:
	Parser(const std::vector< token > & input)
		:tape(&input) {}
 private:
	//follow
	std::unordered_map< symbol, std::set< symbol > > generate_follow_set(const std::vector< token > & input);
	
	//first
	std::unordered_map< symbol, std::set< symbol > > generate_first_set(const std::vector< token > & input);
	
	//nullable
	std::unordered_map< symbol, std::set< symbol > > generate_nullable_set(const std::vector< token > & input);

	const std::vector < token > * const tape;
	Node * root;
};
#endif
