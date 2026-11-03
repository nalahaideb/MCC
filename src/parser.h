#ifndef PARSER_H
#define PARSER_H
#include <iostream>
#include <unordered_map>
#include <set>
#include "node.h"
#include "grammar.h"
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
	 [] tree derivations can be replicated consistently
	 [] checks tree against derivation table to ensure no parsing error
  [] output rewritten grammar to "docs/grammar.md"
  (with exception of dangling else, but that could be associated with nearest "if" like c/c++/java)
  
  OPTIMIZATIONS:
  [] add parentheses around return value for type of expression, for example
     "return n > 0;" can be written as "return (n > 0);"
  [] 
*/

//==============================================================================
class Parser
{
public:
	Parser(const std::vector< token > & input, grammar * gram)
		:tape(&input), g(gram)//, nullable(generate_nullable_set()),
		 //first(generate_first_set()), follow(generate_follow_set())
	{
		nullable = generate_nullable_set();
	    first = generate_first_set();
	    follow = generate_follow_set();
	}
	//private:
	//follow
	std::unordered_map< symbol, std::unordered_set< symbol > > generate_follow_set();	
	//first
	std::unordered_map< symbol, std::unordered_set< symbol > > generate_first_set();
	//nullable
	std::unordered_set< symbol > generate_nullable_set();
	
	const std::vector < token > * const tape;
	std::unordered_set< symbol > nullable;
	std::unordered_map< symbol, std::unordered_set< symbol > > first;
	std::unordered_map< symbol, std::unordered_set< symbol > > follow;
	Node * root;
    grammar * g;
};

std::unordered_map< symbol, std::unordered_set< symbol > > Parser::generate_follow_set()
{
	std::unordered_map< symbol, std::unordered_set< symbol > > follow;

	return follow;
}

std::unordered_map< symbol, std::unordered_set< symbol > > Parser::generate_first_set()
{
	//first set (return)
	std::unordered_map< symbol, std::unordered_set< symbol > > first;

	//programmatically determine terminals / nonterminals
	//the creation of these sets should be moved to a fn
	std::unordered_set< symbol > terminals;
	std::unordered_set< symbol > nonterminals;

	//nullables
	std::unordered_set< symbol > nullable = generate_nullable_set();
	
	//add all left-hand production symbols to nonterminals
	for (const auto & p : g->productions)
		nonterminals.insert(p.prod_symbol);

	//add elements in right-hand that are NOT in nonterminals set to terminals
	for (const auto & p : g->productions)
		for (const auto & v : p.produced)
			if (!nonterminals.count(v))
				terminals.insert(v);

	//populate first set terminals
	for (const auto & t : terminals)
		first[t] = {t};
	
	//populate first set nonterminals
	for (const auto & p : nonterminals)
		first[p] = {};

	//build first set
	bool changed = true;
    while (changed)
    {
        changed = false;
        for (const auto & p : g->productions) //p = production <symbol, vector <symbol> >
        {
            auto & N = first[p.prod_symbol]; //N = p -> vector<symbol>

            for (const auto & v : p.produced) //v = symbol from p
            {
				for (const auto & s : first[v]) //s = set from first
					//detects if an insertion was successful
					if (N.insert(s).second) 
						changed = true;
				if (!nullable.count(v))
					break;
			}
		}
	}
	return first;
}

std::unordered_set< symbol > Parser::generate_nullable_set()
{
	std::unordered_set< symbol > nullable;
	
	//get all nullable nonterminals first
	for (const auto & p : g->productions)
		if (p.produced.empty())
			nullable.insert(p.prod_symbol);

	bool changed = true;
	while (changed)
	{
		size_t nullable_size = nullable.size();
		for (const auto & p : g->productions)
		{
			//check all elements in the production of nonterminal p against
			//existing nullable set
			bool is_nullable = true;
			for (const auto & t : p.produced)
			{
				//all productions of p must already be in nullable set
				if (nullable.find(t) == nullable.end())
				{
					is_nullable = false;
					break;
				}
			}
			//add valid nullable nonterminal
			if (is_nullable)
			{
				if (!nullable.count(p.prod_symbol))
					std::cout << "new nont " << p.prod_symbol << std::endl;
				nullable.insert(p.prod_symbol);
			}
		}
		
		//break on no new additions
		if (nullable.size() == nullable_size) changed = false;
	}

	return nullable;
}

#endif
