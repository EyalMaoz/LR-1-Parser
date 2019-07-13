// Eyal Maoz - 205911936
///////////////////////////// IMPORTANT ////////////////////////////////////////////////////////////
/* I didnt inherit from "LR(0)" but i used its functions.										  */
/* The original class had lots of exception therefor i started over and copied					  */
/* the common functions. I also used "Rule" struct,"Item_lr_0" in "Item_lr_1" and etc...	      */
/* Please make sure you are compiling using C++ language standart: ISO C++17 Standard (/std:c++17)*/
/* Take notice - epsilon is "e", do not use "e" as a terminal									  */
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  LR_1.h
//  LR1_Parser header
//	I used the function,types and attributes from LR(0) that are the same and created 
//  the missing for LR(1), i took function "first" from Dan's Lamberg LL Parser
//  Created by Eyal Maoz, using Dan Lamberg class's.
//  Copyright © 2019 Dan Lemberg. All rights reserved.
//

#include "LR_0.h"

#ifndef LR_1_h
#define LR_1_h
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Item_lr_1 : public Item_lr_0
{
public:
	Item_lr_1();
	Item_lr_1(string left_, vector<string> right_, size_t separator_);
	Item_lr_1(string str_rule, size_t separator_);
	Item_lr_1(Rule& rule, size_t separator_);

	bool operator < (const Item_lr_1& x) const;
	bool operator == (const Item_lr_1& x) const;
	void printWithSym() const;
	string symbol;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LR_1 
{
	bool noIssue;// parsing to lr1 hasnt run into trouble
	string start;
	Rule start_rule;
	Item_lr_1 start_item;
	Item_lr_1 end_item;
	set<string> terminals = {};
	set<string> non_terminals = {};
	vector<Rule> rules;
	vector<vector<pair<char, size_t>>> ACTION;
	vector<vector<size_t>> GOTO;

	vector<set<Item_lr_1>> states;
	map<size_t, set<pair<size_t, string>>> automate;

	void get_grammar(vector<string>& grammar);
	set<Item_lr_1> closureForLR1(set<Item_lr_1>& item_set);
	set<Item_lr_1> deltaForLR1(const set<Item_lr_1>& item_set, string x);
	void set_automateForLR1();
	void set_tablesForLR1();
	// From LL_Parser
	set<string> First(string temp);
	map<string, set<string>> first_map_creater();
	set<string> eps_set_creater();
	//
	bool is_terminalForLR1(string x);
	bool is_non_terminalForLR1(string x);

	void reduce(stack<pair<string, size_t>>& st, size_t n);
	void shift(stack<pair<string, size_t>>& st, string lookahead, size_t n);

public:
	LR_1(vector<string>& grammar, string start_non_terminal);

	vector<size_t> parse(vector<string> input);

	void print_ALL();
	void print_rules(vector<size_t> rule_vec);
	void print_grammar();
	void print_states();
	void print_atomate();
	void print_goto();
	void print_action();
};

#endif /* LR_1_h */