#ifndef PARSER_H
#define PARSER_H
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <stack>
#include "node.h"
#include "token.h"
#include "grammar.h"
#include "error.h"
//==============================================================================
/*
  OVERVIEW:
  given a grammar g and a vector of input tokens, the parser will:
  - generate NULLABLE, FIRST and FOLLOW sets
  - combine the sets into a parse table
  - begin reading tokens from input and to build AST
  - the AST is built by balancing two stacks, a parse stack and an evaluated stack
  - the parse stack only holds nonterminals and node directives
  - a node directive ("#Add", "#Program", "#Num") will take one or more evaluated elements
    and create the corresponding node named after the directive, then add it to the
	evaluated stack
  - the evaluated stack only holds node pointers, pieces the parse stack looks for when
    building another node or to make a match
  - output AST to terminal

  if there are "holes" in the tree that constitutes a parsing error and instead
  of printing out a partially completed AST, the parser then prints out to console
  the offending line / type of error
*/
//==============================================================================

//==============================================================================
/*
  TODO:
  
  [X] generates a parse table from given rules
     [X] generate nullable sets
	 [X] generate first sets
	 [] generate follow sets
     [X] establish precedence rules for forms in the table
	 
  [] implement parser-generation algorithm
     [] cases for terminal, nonterminal and end-of-input marker
	 [] breaks on failed check against parse-table with specific error
	 
  [] constructs syntax tree (via stack?)
     [] parse stack to place nonterminal productions and action flags
	 [] value stack to place terminals and factored expression nodes
	 [] action flags being read pop appropriate num of nodes off value stack
	 [] elements in value stack are added in reverse to respect associativity
	 
  [] output rewritten grammar to "docs/grammar.md"
  (with exception of dangling else, but that could be associated with nearest "if" like c/c++/java)

  [] tree destructor
  
  OPTIMIZATIONS:
  [] add parentheses around return value for type of expression, for example
     "return n > 0;" can be written as "return (n > 0);"
  [] only parse-table needs to persist, so convert return types of set generators
     to update existing parse-table instead
*/

//==============================================================================
class Parser
{
public:
	Parser(const std::vector< token > & input, grammar * gram)
		:tape(&input), g(gram)
	{
		//generate terminal / nonterminal sets
		populate_nont_term();
		//generate nullables
		nullable = generate_nullable_set();
		//generate firsts
	    first = generate_first_set();
		//generate follows
	    follow = generate_follow_set();

		leaf_terminals = { "IDENT", "NUM" };

		//combine into parse table
		populate_parse_table();

		//execute
		std::cout << "begin tree:\n";
		parse();
	}

	//can you guess what this does :^)
	inline void parse();

	//print finished AST 
	inline void print_tree() { std::cout << *root << std::endl; }
private:

	//----------member funcs----------
    inline bool is_action(const symbol & s);
	inline token look(const size_t pos);
	inline symbol terminal_of(const token & t) const;
	//----------generators----------
	//sets up terminal / nonterminal sets
    inline void populate_nont_term();
	//union nullable / first / follow sets in parse_table
	inline void populate_parse_table();
	//generate follow set
	inline std::unordered_map< symbol, std::unordered_set< symbol > > generate_follow_set();	
	//generate first set
	inline std::unordered_map< symbol, std::unordered_set< symbol > > generate_first_set();
	//generate nullable set
	inline std::unordered_set< symbol > generate_nullable_set();
	
	//----------member vars----------
	const std::vector < token > * const tape;
	Node * root;
    grammar * g;
	//----------terminals----------
	std::unordered_set< symbol > terminals;
	std::unordered_set< symbol > leaf_terminals;
	//nonterminals
	std::unordered_set< symbol > nonterminals;
	//nullable
	std::unordered_set< symbol > nullable;
	//first
	std::unordered_map< symbol, std::unordered_set< symbol > > first;
	//follow
	std::unordered_map< symbol, std::unordered_set< symbol > > follow;
	//parse table
	std::unordered_map< symbol, std::unordered_map< symbol, std::vector< symbol > > > parse_table;
	
};



std::unordered_map< symbol, std::unordered_set< symbol > > Parser::generate_follow_set()
{
	//FOLLOW set (return)
	std::unordered_map< symbol, std::unordered_set< symbol > > follow;

	//initialize every nonterminal in FOLLOW to epsilon
	for (const auto & v : nonterminals)
		follow[v] = {};

	//seed FOLLOW set with artificial start state + EOF marker
	follow[g->start_] = {g->EOF_};

	bool changed = true;
	while (changed)
	{
		changed = false;

		for (const auto & p : g->productions) //p = production <symbol, vector>
		{
			const auto & rhs = p.produced; //rhs = symbol vector

			for (size_t i = 0; i < rhs.size(); ++i)
			{
				//FOLLOW is only defined for nonterminals
				if (nonterminals.count(rhs[i]))
				{
					auto & followB = follow[rhs[i]];

					//collect first of production until a non-nullable symbol
					bool suffix_nullable = true;
					for (size_t j = i + 1; j < rhs.size(); ++j)
					{
						//FIRST(rhs[j]) to FOLLOW(B)
						for (const auto & s : first[rhs[j]])
							if (followB.insert(s).second)
								changed = true;

						//break on non-nullable
						if (!nullable.count(rhs[j]))
						{
							suffix_nullable = false;
							break;
						}
					}
					//suffix empty or entirely nullable, FOLLOW(A) -> FOLLOW(B)
					if (suffix_nullable && p.prod_symbol != rhs[i])  //skip A == B
					{
						for (const auto & s : follow[p.prod_symbol])
							if (followB.insert(s).second)
								changed = true;
					}
				}
			}
		}
	}
	return follow;
}

std::unordered_map< symbol, std::unordered_set< symbol > > Parser::generate_first_set()
{
	//FIRST set (return)
	std::unordered_map< symbol, std::unordered_set< symbol > > first;

	//populate FIRST set terminals
	for (const auto & t : terminals)
		first[t] = {t};
	
	//populate FIRST set nonterminals
	for (const auto & p : nonterminals)
		first[p] = {};

	//build FIRST set
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
					//detects if an insertion was successful, dont break 
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
	
	//get all NULLABLE nonterminals first
	for (const auto & p : g->productions)
	{
		if (p.produced.empty())
			nullable.insert(p.prod_symbol);
		for (const auto & s : p.produced)
			if (is_action(s))
				nullable.insert(s);
	}
	
	bool changed = true;
	while (changed)
	{
		size_t nullable_size = nullable.size();
		for (const auto & p : g->productions)
		{
			//check all elements in the production of nonterminal p against
			//existing NULLABLE set
			bool is_nullable = true;
			for (const auto & t : p.produced)
			{
				//all productions of p must already be in NULLABLE set
				if (nullable.find(t) == nullable.end())
				{
					is_nullable = false;
					break;
				}
			}
			//add valid NULLABLE nonterminal
			if (is_nullable)
			{
				if (!nullable.count(p.prod_symbol))
					//std::cout << "new nont " << p.prod_symbol << std::endl;
				nullable.insert(p.prod_symbol);
			}
		}		
		//break on no new additions
		if (nullable.size() == nullable_size) changed = false;
	}

	return nullable;
}

bool Parser::is_action(const symbol & s)
{ return !s.empty() && s[0] == '#'; }

void Parser::populate_nont_term()
{
	//add all left-hand production symbols to nonterminals
	for (const auto & p : g->productions)
		nonterminals.insert(p.prod_symbol);

	//add elements in right-hand that are NOT in nonterminals set to terminals
	for (const auto & p : g->productions)
		for (const auto & v : p.produced)
			if (!nonterminals.count(v) && !is_action(v))
				terminals.insert(v);
}

template <typename T, typename Container>
	std::ostream& operator<<(std::ostream& os, const std::stack<T, Container>& s)
{
    std::stack<T, Container> temp = s;

    os << "[ ";

    while (!temp.empty())
    {
        os << temp.top();
        temp.pop();

        if (!temp.empty())
            os << ", ";
    }

    os << " ]";
    return os;
}

#endif
