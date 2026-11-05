#include "parser.h"
#include "helper.h"

//looks at next token, returns EOF if pos past tape size
token Parser::look(const size_t pos)
{
	token eof_ = {g->EOF_, ""};
	return pos < tape->size() ? (*tape)[pos] : eof_;
};

//used to get actual terminal value instead of file.out piped value
symbol Parser::terminal_of(const token & t) const
{
	if (terminals.count(t.type))  return t.type; //ID, NUM, $
	if (terminals.count(t.value)) return t.value; //"int", "(", "else", "=="
	return t.type; //unknown type/terminal, will fail with a readable error
}

//CHECK ME !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
void Parser::populate_parse_table()
{
	for (const auto & p : g->productions)
	{
		const symbol & A = p.prod_symbol;
		const std::vector< symbol > & alpha = p.produced;

		//cells this production belongs in
		std::unordered_set< symbol > select;

		//FIRST(alpha), walk until a non-nullable symbol
		bool alpha_nullable = true;
		for (const auto & s : alpha)
		{
			const auto f = first.find(s);
			if (f != first.end())
				select.insert(f->second.begin(), f->second.end());

			if (!nullable.count(s))
			{
				alpha_nullable = false;
				break;
			}
		}

		//A may be skipped when the next token can follow A
		if (alpha_nullable)
		{
			const auto f = follow.find(A);
			if (f != follow.end())
				select.insert(f->second.begin(), f->second.end());
		}

		auto & row = parse_table[A];
		for (const auto & t : select)
		{
			const auto ins = row.emplace(t, alpha);
			//cell already holds a different production -> not LL(1)
			if (!ins.second && ins.first->second != alpha)
			{
				//prevent throw on ElsePart
				if (A == "ElsePart" && t == "ELSE") continue;
				Error e("[populate_parse_table] ERROR: GRAMMAR NOT LL(1): conflict at M[" + A + ", " + t + "]");
				throw e;
			}
		}
	}
}

void Parser::parse()
{
	//nonterminals and actions 
	std::stack< symbol > parse_stack;
	
	//finished subtrees
	std::stack< Node * > eval_stack;   

	//read head
	size_t pos = 0;                     
	
	parse_stack.push(g->start_);

	while (!parse_stack.empty())
	{
		const symbol top = parse_stack.top();
		const token & current = look(pos);
		//lookahead as a grammar terminal
		const symbol raw = terminal_of(current);
		symbol la = raw;
		//tokens are capitalized so this is just casting them to lowercase
		//
		if (!terminals.count(la))
		{
			if (terminals.count(lower(raw)))
				la = lower(raw);
			else if (terminals.count(lower(current.value)))   //TYPE with value INT -> int
				la = lower(current.value);
		}

		std::cout << "top: " << top
				  << ", current: " << current.printable()
				  << ", la: " << la
				  << ", raw: " << raw << std::endl;
		std::cout << "\n======BEGIN PARSE STACK======\n";
		std::cout << parse_stack << std::endl;
		std::cout << "======END PARSE STACK======\n\n";
		std::cout << "======BEGIN VALUE STACK======\n";
		std::cout << eval_stack << std::endl;
		std::cout << "======END VALUE STACK======\n\n";
		
		//action flag, get two latest added nodes
		if (is_action(top))
		{
			parse_stack.pop();

			//mismatch on stack count, action flags need at least one node
			if (eval_stack.size() < 2)
			{
				Error e("[parse] ERROR: action " + top + " needs two operands on the value stack");
				throw e;
			}
			//nodes should have been pushed in reverse order
			Node * right = eval_stack.top(); eval_stack.pop();   
			Node * left  = eval_stack.top(); eval_stack.pop();
			eval_stack.push(new Node(top.substr(1), left, right));
		}
		
		//nonterminal, look at table, replace it with its production
		else if (nonterminals.count(top))
		{
			const auto row = parse_table.find(top);
			if (row == parse_table.end())
			{
				Error e("[parse]: ERROR: no table row for " + top);
			}
			const auto cell = row->second.find(la);
			if (cell == row->second.end())
			{
				std::string expected;
				for (const auto & e : row->second)
					expected += " " + e.first;
			    Error e("[parse] ERROR: syntax error at "
						+ current.printable() + ": while parsing " + top
						+ ", expected one of:" + expected);
				throw e;
			}
			
			parse_stack.pop();
			const std::vector< symbol > & rhs = cell->second;
			
			//reverse and push productions from production vector 
			for (auto it = rhs.rbegin(); it != rhs.rend(); ++it)   
				parse_stack.push(*it); //epsilon pushes nothing (if epsilon)
		}
		else //terminal, must equal the current token
		{
			if (top != la)
			{
				Error e("[parse] ERROR: syntax error at "
						+ current.printable() + ": expected " + top);
				throw e;
			}

			if (leaf_terminals.count(top))
				eval_stack.push(new Node(current.value, NULL, NULL));

			parse_stack.pop();
			++pos;
		}
	}

	//throw on leftover elements not used 
	if (eval_stack.size() != 1)
	{
		Error e("[parse] ERROR: parse finished with " + std::to_string(eval_stack.size()) + " nodes on the value stack, expected 1");
		throw e;
	}
	
	//assign root top of completed tree
	root = eval_stack.top();
}
