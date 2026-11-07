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
  - begin reading tokens from input and to build CST
  - recursive calls on terminal / nonterminal elements use table lookups to create nodes with or without children (dependent on "Kind")
  - nodes are evaluated LL(1) through full construction of leaf
    nodes before full construction of their parents
  - nodes record line via consume() which passes token line field
    as a parameter
  - nodes created depth first, Node * root is top of AST
  - "XChain" functions serve as {} or [] of the original grammar,
    allowing multiple different productions with no ambiguity
  - output AST as S-expression to terminal

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
	 [X] generate follow sets
     [X] establish precedence rules for forms in the table
	 
  [X] implement parser-generation algorithm
     [X] separate nontemrinal functions
     [X] cases for terminal, nonterminal and end-of-input marker
	 [X] breaks on failed check against parse-table with specific error
	 [X] test multiple functions
	 [X] test mutually recursive functions
	 
  [X] constructs syntax tree
     [X] nonterminal evaluation function recursively evaluates all
	    elements in its production vector, creating nodes for each
	 [X] terminal nodes have no children, return a leaf node
  	 
  [] output rewritten grammar + generated sets to "docs/grammar.md"

  OPTIMIZATIONS:
  [] add parentheses around return value for type of expression, for example
     "return n > 0;" can be written as "return (n > 0);"
  [] only parse-table needs to persist, so convert return types of set generators
     to update existing parse-table instead
	 i swear to god if this doesnt work ill blow my brains out
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

		//combine into parse table
		populate_parse_table();

		//execute
		parse();
	}

	//can you guess what this does :^)
	inline void parse();

	//----------print finished AST---------- 
	inline void print_tree() { std::cout << *root << std::endl; }
private:

	//----------member funcs----------
    inline token look(const size_t pos) const;
	inline symbol terminal_of(const token & t) const;	

	//----------table lookups----------
	inline const std::vector< symbol > * predict(const symbol & A) const;
	inline const std::vector< symbol > & select(const symbol & A) const;
	inline bool is_epsilon(const symbol & A) const;

	typedef Node * (Parser::*ParseFn)();

	//----------called by relevant nonterminals----------
	inline Node * seq(Node * acc, Node * next);
	inline Node * repeat(const symbol & A, Node * acc, ParseFn item);
	inline Node * chain(const symbol & A, Node * left, ParseFn operand);
	inline Node * call(const symbol & A, Node * name);

	//---------- NONTERMINAL FUNCTIONS ----------
	inline Node * Start();
	inline Node * Program();
	inline Node * ProgramChain(Node * acc);
	inline Node * FunDecl();
	inline Node * FunDeclChain(Node * head);
	inline Node * Params();
	inline Node * ParamsChain(Node * acc);
	inline Node * Param();
	inline Node * Type();
	inline Node * Block();
	inline Node * BlockItems();
	inline Node * Item();
	inline Node * Decl();
	inline Node * Stmt();
	inline Node * ElsePart(Node * then);
	inline Node * StmtIDChain(Node * name);
	inline Node * StmtCallChain(Node * name);
	inline Node * ReturnChain(const std::string & ln);
	inline Node * Exp();
	inline Node * ExpChain(Node * left);
	inline Node * AndExp();
	inline Node * AndExpChain(Node * left);
	inline Node * EqExp();
	inline Node * EqExpChain(Node * left);
	inline Node * RelExp();
	inline Node * RelExpChain(Node * left);
	inline Node * AddExp();
	inline Node * AddExpChain(Node * left);
	inline Node * MulExp();
	inline Node * MulExpChain(Node * left);
	inline Node * Unary();
	inline Node * Primary();
	inline Node * PrimaryChain(Node * name);
	inline Node * PrimaryCallChain(Node * name);
	inline Node * Args();
	inline Node * ArgsChain(Node * acc);
	
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

	//----------token access and errors----------
	//identify terminal of current symbol
	inline symbol peek() const
	{ return terminal_of(look(pos)); }
	//return current token line
	inline std::string here() const
	{ return look(pos).line; }
	//create a return token based off symbol
	inline token consume(const symbol & terminal);
	//create leaf from symbol / kind
	inline Node * leaf(Kind k, const symbol & terminal);
	inline void fail(const std::string & expected) const;
	inline std::string expected_in(const symbol & A) const;
	inline std::string found(const token & t) const;

	//----------member vars----------
	const std::vector < token > * const tape;
	//AST root
	Node * root;
	//grammar
    grammar * g;	
	//read head
	size_t pos = 0; 
	
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
	
	typedef Node * (Parser:: * Rule)();
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
		// for (const auto & s : p.produced)
		// 	nullable.insert(s);
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

void Parser::populate_nont_term()
{
	//add all left-hand production symbols to nonterminals
	for (const auto & p : g->productions)
		nonterminals.insert(p.prod_symbol);
	
	//add elements in right-hand that are NOT in nonterminals set to terminals
	for (const auto & p : g->productions)
		for (const auto & v : p.produced)
			if (!nonterminals.count(v))
				terminals.insert(v);
}

#endif
