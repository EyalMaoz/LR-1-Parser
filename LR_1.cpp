// Eyal Maoz - 205911936
///////////////////////////// IMPORTANT ////////////////////////////////////////////////////////////
/* I didnt inherit from "LR(0)" but i used its functions.										  */
/* The original class had lots of exception therefor i started over and copied					  */
/* the common functions. I also used "Rule" struct,"Item_lr_0" in "Item_lr_1" and etc...	      */
/* Please make sure you are compiling using C++ language standart: ISO C++17 Standard (/std:c++17)*/
/* Take notice - epsilon is "e", do not use "e" as a terminal									  */
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  LR_1.cpp
//  LR1_Parser implementation
//	I used the function,types and attributes from LR(0) that are the same and created 
//  the missing for LR(1), i took function "first" from Dan's Lamberg LL Parser
//  Created by Eyal Maoz, using Dan Lamberg class's.
//  Copyright © 2019 Dan Lemberg. All rights reserved.
//
#include "LR_1.h"


//////////////////////////////////////////////////////////////////////////////////
//////////////////// Items are different from LR(0) - I added "symbol" ///////////
//////////////////////////////////////////////////////////////////////////////////
//Constructors
Item_lr_1::Item_lr_1() {}
Item_lr_1::Item_lr_1(string left_, vector<string> right_, size_t separator_) : Item_lr_0(left_, right_, separator_), symbol("$") {}
Item_lr_1::Item_lr_1(string str_rule, size_t separator_) : Item_lr_0(str_rule, separator_), symbol("$") {}
Item_lr_1::Item_lr_1(Rule& rule, size_t separator_) : Item_lr_0(rule, separator_), symbol("$") {}

bool Item_lr_1::operator < (const Item_lr_1& x) const 
{
	if (left < x.left) return true;
	if (left > x.left) return false;
	if (right < x.right) return true;
	if (right > x.right) return false;
	if (symbol < x.symbol) return true;
	if (symbol > x.symbol) return false;// My add

	return separator < x.separator;
}

bool Item_lr_1::operator == (const Item_lr_1& x) const
{
																					// My add
	return (left == x.left) && (right == x.right) && (separator == x.separator) && (symbol == x.symbol);
}

void Item_lr_1::printWithSym() const
{
	cout << left << " -> ";
	for (size_t i = 0; i < separator; i++)
		cout << right[i] << " ";
	cout << ". ";
	for (size_t i = separator; i < right.size(); i++)
		cout << right[i] << " ";
	cout << ", " << symbol;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////// LR(1) Functions ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


//A constructor that inputs a grammar as a vector of strings and initial non-terminal.
LR_1::LR_1(vector<string>& grammar, string start_non_terminal) : start(start_non_terminal) 
{
	get_grammar(grammar); //parse a grammar
	noIssue = true;
	for (auto& r : rules) //collecting all terminals and non-terminals
	{
		non_terminals.insert(r.left);
		for (auto& x : r.right)
			if (x[0] == '<' && x[x.size() - 1] == '>' && x.size() > 2)
				non_terminals.insert(x);
			else if (x != ""&& x != "e")
				terminals.insert(x);
	}
	non_terminals.insert(start);
	terminals.insert("$"); //add $ to terminals

	//augumenting a grammar by new initial non-terminal and new start rule
	non_terminals.insert("<--->");
	string st = "<---> : " + start;
	//st += " $";
	start_rule = Rule(st);
	start_rule.set_number(rules.size());
	rules.push_back(start_rule);

	//creating a start_item and a end_item
	start_item = Item_lr_1(start_rule, 0);
	start_item.symbol = "$";
	end_item = Item_lr_1(start_rule, start_rule.right.size());
	end_item.set_reduct(true);
	end_item.symbol = "$";

	set_automateForLR1();
	set_tablesForLR1();   //creating a tables GOTO and ACTION

	if (noIssue)
	{
		cout << "Parser Createrd succesfully" << endl;
	}

}

bool LR_1::is_terminalForLR1(string x) { return terminals.count(x); }
bool LR_1::is_non_terminalForLR1(string x) { return non_terminals.count(x); }

//The function inputs set of lr1-items and returns a closure of them.
set<Item_lr_1> LR_1::closureForLR1(set<Item_lr_1>& item_set)
{
	set<Item_lr_1> cl_set = item_set;
	size_t sz_old = 0, sz_new = cl_set.size();

	while (sz_old != sz_new)
	{
		sz_old = sz_new;
		for (auto& item : cl_set)// For each item in the closure group
		{
			string nt = item.get_next();// look at what is next the dot
			if (is_non_terminalForLR1(nt))// and if its not a termianl - add each row from it
			{
				string betaT = "";
				for (int i = item.separator + 1; i < item.right.size(); i++)
					betaT.append(item.right.at(i));
				betaT.append(item.symbol);

				set<string> setOfFirsts = First(betaT);
				for (auto& r : rules)
				{
					if (r.left == nt)
					{
						Item_lr_1 tmp(r, 0);

						for (string x : setOfFirsts)
						{
							//Item_lr_1 newItem(r, 0);
							tmp.separator = 0;
							tmp.symbol = x;
							//newItem.symbol = beta;
							cl_set.insert(tmp);
						}
					}
				}
			}
		}
		sz_new = cl_set.size();
	}
	return cl_set;
}

set<string> LR_1::First(string temp)
{
	map<string, set<string>> F = first_map_creater();
	set<string> EPS = eps_set_creater();
	string str = "";
	str.append(1, temp.at(0));
	if (temp.at(0) == '<')
	{
		str.append(1, temp.at(1));
		str.append(1, temp.at(2));
	}
	set<string> FST = F[str];
	for (int i = 1; i < str.size() && EPS.count(str); i++)
	{
		FST.insert(F[str].begin(), F[str].end());
	}
	return FST;
}

set<string> LR_1::eps_set_creater()
{
	set<string> A;
	size_t sz = 0;
	for (Rule rule : rules)
		if (rule.right.at(0) == "e") A.insert(rule.left);

	while (A.size() != sz) {
		sz = A.size();
		for (Rule rule : rules)
		{
			auto tmp = set<string>(rule.right.begin(), rule.right.end());
			if (includes(tmp.begin(), tmp.end(), A.begin(), A.end()))
				A.insert(rule.left);
		}
	}
	return A;
}

map<string, set<string>> LR_1::first_map_creater()
{
	map<string, set<string>> F;
	set<string> eps_set = eps_set_creater();
	size_t sz = 0;
	bool flag = true;

	for (string x : terminals) F[x] = { x };
	for (string x : non_terminals) F[x] = {};

	while (flag)
	{
		for (auto& rule : rules)
		{
			auto& X = F[rule.left], & Y = F[rule.right.at(0)];
			sz = X.size();
			X.insert(Y.begin(), Y.end());
			flag = (X.size() != sz);

			for (int i = 4; i < rule.right.size() && eps_set.count(rule.right.at(i - 1)); i++)
			{
				auto& X = F[rule.left], & Y = F[rule.right.at(i)];
				sz = X.size();
				X.insert(Y.begin(), Y.end());
				flag = flag || (X.size() != sz);
			}
		}
	}
	return F;
}

//Transition function
set<Item_lr_1> LR_1::deltaForLR1(const set<Item_lr_1>& item_set, string x)
{
	set<Item_lr_1> ret;
	for (auto& item : item_set)
	{
		if (item.get_next() == x)
		{
			auto tmp = item;
			tmp.separator++;
			if (tmp.separator == tmp.right.size())
				tmp.set_reduct(true);
			ret.insert(tmp);
		}
	}
	return closureForLR1(ret);
}

bool operator==(const set<Item_lr_1>::value_type& lhs, vector<Rule>::const_reference rhs)
{
	return (lhs.left == rhs.left) && (lhs.right == rhs.right);
}

//Function that create an automate
void LR_1::set_automateForLR1()
{
	set<Item_lr_1> start_set = { start_item };
	set<set<Item_lr_1>> set_of_states = { closureForLR1(start_set) };
	size_t sz_old = 0, sz_new = set_of_states.size();

	while (sz_old != sz_new)
	{
		sz_old = sz_new;

		for (auto& state : set_of_states)
			for (auto& item : state)
				if (string str = item.get_next(); str != ""&& str != "e")
					set_of_states.insert(deltaForLR1(state, str));

		sz_new = set_of_states.size();
	}

	size_t i = 0;
	for (auto& state : set_of_states) {
		states.push_back(state);
		i++;
	}

	i = 0;
	for (auto& state : states) 
	{
		for (auto& item : state)
		{
			if (string str = item.get_next(); str != "" && str != "e")
			{
				automate[i].insert({ distance(set_of_states.begin(),
					set_of_states.find(deltaForLR1(state, str))), str });
			}
		}
		i++;
	}
}

//Function that create a tables ACTION adn GOTO
void LR_1::set_tablesForLR1()
{
	GOTO = vector<vector<size_t>>(states.size(), vector<size_t>(non_terminals.size(), 0));
	ACTION = vector<vector<pair<char, size_t>>>(states.size(), vector<pair<char, size_t>>(terminals.size(), { ' ', 0 }));
	// GOTO:
	size_t j = 0;
	for (auto& nt : non_terminals)
	{
		for (auto& [state, val] : automate)
			for (auto& [i, str] : val)
				if (nt == str) 
					GOTO[state][j] = i;
		j++;
	}
	// ACTION:
	// acc and reduce actions:
	j = 0;
	for (string term : terminals)
	{
		size_t i = 0;
		for (auto& state : states)
		{
			for (auto& item : state)
			{
				if (item == end_item && term == "$")
				{
					ACTION[i][j] = { 'a', 0 };
					break;
				}
				else if ((item.reduct || item.right[0]=="e" )&& term == item.symbol)
				{
					for (int k = 0; k < this->rules.size(); k++)
					{
						if (item == rules.at(k)) 
						{
							if (ACTION[i][j].first == ' ' && ACTION[i][j].second == 0)
								ACTION[i][j] = { 'r', k };
							else
							{
								noIssue = false;
							}
							break;
						}
					}
				}
			}
			i++;
		}
		j++;
	}
	// shift action:
	j = 0;
	for (auto& nt : terminals)
	{
		for (auto& [state, val] : automate)
		{
			for (auto& [i, str] : val)
			{
				if (nt == str)
				{
					if (ACTION[state][j].first == ' ' && ACTION[state][j].second == 0)
					{
						ACTION[state][j] = { 's', i };
					}
					else
					{
						noIssue = false;
					}
				}
			}
		}
		j++;
	}
	if (!noIssue)
	{
		cout << "Encountered conflict building tables (Shift Reduce, etc...)" << endl;
	}
}

//The function inputs grammar as a vector of strings and creates rules.
void LR_1::get_grammar(vector<string>& grammar)
{
	for (string& str : grammar)
	{
		size_t pos = str.find(':');
		string first = str.substr(0, pos + 1);
		string seq = str.substr(pos + 1);

		std::size_t current, previous = 0;

		current = seq.find('|', previous);
		while (current != string::npos) {
			rules.push_back(Rule(first + seq.substr(previous, current - previous)));
			previous = current + 1;
			current = seq.find('|', previous);
		}
		rules.push_back(Rule(first + seq.substr(previous, current - previous)));
	}

	size_t i = 0;
	for (auto& r : rules)
		r.set_number(i++);
}

//Parser
vector<size_t> LR_1::parse(vector<string> input) 
{
	
	vector<size_t> ret;
	size_t pos = 0, i, ix;
	string x;
	stack<pair<string, size_t>> st;
	if (!noIssue)
	{
		cout << "Grammer incorrect." << endl;
		cout << "Cannot parse with LR(1)." << endl;
		return ret;
	}

	st.push({ "", 0 });

	if (input.at(input.size()-1) != "$")
	{
		input.insert(input.end(), "$");
	}

	while (true) 
	{
		i = st.top().second;
		x = input[pos];
		if (!terminals.count(x))
		{
			cout << "There is no '"<<x<<"' in terminals." << endl<< "Cannot parse."<<endl;
			return ret;
		}
		ix = distance(terminals.begin(), terminals.find(x));

		if ((pos + 1) == input.size() && ACTION[i][0].first == 'a') 
		{
			for (int i = 0; i < input.size(); ++i)
				std::cout << input[i];
			cout << "  Is ok! Parsed successfully." << endl;
			return ret;
		}
		
		auto [act, n] = ACTION[i][ix];

		if (act == 's') 
		{
			shift(st, x, n);
			pos++;
		}
		else if (act == 'r') 
		{
			ret.push_back(n);
			reduce(st, n);
		}
		else 
		{
			for (int i = 0; i < input.size(); ++i)
				std::cout << input[i];
			cout << "  Is Incorrect! Cannot parse." << endl;
			return ret;
		}
	}

	return vector<size_t>();
}

//Reduce function
void LR_1::reduce(stack<pair<string, size_t>>& st, size_t n)
{
	Rule r = rules[n];
	for (size_t i = 0; i < r.right.size(); i++)
		st.pop();

	size_t k = st.top().second;
	size_t m = distance(non_terminals.begin(), non_terminals.find(r.left));
	st.push({ r.left, GOTO[k][m] });
}

//Shift function
void LR_1::shift(stack<pair<string, size_t>>& st, string lookahead, size_t n) 
{
	st.push({ lookahead, n });
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Print functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LR_1::print_ALL()
{
	cout << "Grammer:" << endl;
	print_grammar();
	
	if (!noIssue)
	{
		cout << "Grammer incorrect." << endl;
		cout << "Cannot parse with LR(1)." << endl;
		return;
	}

	cout << endl << "States:" << endl;
	print_states();
	cout << endl << "Automate:" << endl;
	print_atomate();
	cout << endl << "Goto:" << endl;
	print_goto();
	cout << endl << "Action:" << endl;
	print_action();
	cout << endl;
}

void LR_1::print_rules(vector<size_t> rule_vec)
{
	if (!noIssue)
	{
		cout << "Grammer incorrect."<<endl;
		cout << "Cannot parse with LR(1)." << endl;
		return;
	}
	for (auto& i : rule_vec)
	{
		cout << rules[i].left << " -> ";
		for (auto& str : rules[i].right)
			cout << str << " ";
		cout << endl;
	}
}

void LR_1::print_grammar()
{
	cout << "All rules:" << endl;
	for (auto& x : rules)
	{
		x.print();
		cout << endl;
	}
}

void LR_1::print_states()
{
	size_t i = 0;
	if (!noIssue)
	{
		cout << "Grammer incorrect." << endl;
		cout << "Cannot parse with LR(1)." << endl;
		return;
	}
	for (auto& state : states)
	{
		cout << "State number " << i++ << ":" << endl;
		for (auto& item : state)
		{
			item.printWithSym();
			cout << endl;
		}
		cout << endl;
	}
}

void LR_1::print_atomate() 
{
	if (!noIssue)
	{
		cout << "Grammer incorrect." << endl;
		cout << "Cannot parse with LR(1)." << endl;
		return;
	}
	for (auto& state : automate)
	{
		cout << state.first << " : ";
		for (auto& p : state.second)
			cout << "[ " << p.first << " , " << p.second << " ]";
		cout << endl;
	}
}

void LR_1::print_goto() 
{
	if (!noIssue)
	{
		cout << "Grammer incorrect." << endl;
		cout << "Cannot parse with LR(1)." << endl;
		return;
	}
	cout << "\t";
	for (auto& str : non_terminals)
		cout << str << "\t";

	cout << endl;

	for (size_t i = 0; i < GOTO.size(); i++)
	{
		cout << i << "\t";
		for (size_t j = 0; j < GOTO[0].size(); j++)
		{
			cout << GOTO[i][j] << "\t";
		}
		cout << endl;
	}
}

void LR_1::print_action()
{
	if (!noIssue)
	{
		cout << "Grammer incorrect." << endl;
		cout << "Cannot parse with LR(1)." << endl;
		return;
	}
	cout << "\t";
	for (auto& str : terminals)
		cout << str << "\t";
	cout << endl;

	for (size_t i = 0; i < ACTION.size(); i++)
	{
		cout << i << "\t";
		for (size_t j = 0; j < ACTION[0].size(); j++)
		{
			cout << ACTION[i][j].first << "[" << ACTION[i][j].second << "]" << "\t";
		}
		cout << endl;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
