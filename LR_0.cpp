//
//  LR_0.cpp
//  LR_Parser
//
//  Created by Dan Lemberg on 10/03/2019.
//  Copyright © 2019 Dan Lemberg. All rights reserved.
//

#include "LR_0.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Rule::Rule(string left_, vector<string> right_) : left(left_), right(right_) {}

//A constructor that inputs a rule as a string, is possible with extra spaces.
Rule::Rule(string str_rule)
{
	size_t pos = str_rule.find(':');
	left = str_rule.substr(0, pos);
	left.erase(remove_if(left.begin(), left.end(), [](char x) { return isspace(x); }), left.end());

	string tmp = str_rule.substr(pos + 1);
	std::size_t current, previous = 0;
	while (tmp[previous] == ' ') previous++;

	current = tmp.find(' ', previous);
	while (current != std::string::npos) {
		right.push_back(tmp.substr(previous, current - previous));
		previous = current + 1;
		while (tmp[previous] == ' ') previous++;
		current = tmp.find(' ', previous);
	}

	if (auto str = tmp.substr(previous, current - previous); str != "")
		right.push_back(str);
}

bool Rule::operator < (const Rule& x) const {
	if (left < x.left) return true;
	if (left > x.left) return false;
	return right < x.right;
}

bool Rule::operator == (const Rule& x) const {
	return (left == x.left) && (right == x.right);
}

void Rule::set_number(size_t num) { number = num; }

void Rule::print() const {
	cout << left << " -> ";
	for (auto& x : right)
		cout << x << " ";
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Item_lr_0::Item_lr_0() {}
Item_lr_0::Item_lr_0(string left_, vector<string> right_, size_t separator_) : Rule(left_, right_), separator(separator_) {}
Item_lr_0::Item_lr_0(string str_rule, size_t separator_) : Rule(str_rule), separator(separator_) {}
Item_lr_0::Item_lr_0(Rule& rule, size_t separator_) : Rule(rule), separator(separator_) {}

string Item_lr_0::get_next() const {
	if (separator < right.size())
		return right[separator];
	else
		return "";
}

bool Item_lr_0::operator < (const Item_lr_0& x) const {
	if (left < x.left) return true;
	if (left > x.left) return false;
	if (right < x.right) return true;
	if (right > x.right) return false;

	return separator < x.separator;
}

bool Item_lr_0::operator == (const Item_lr_0& x) const {
	return (left == x.left) && (right == x.right) && (separator == x.separator);
}

void Item_lr_0::set_reduct(bool r) { reduct = r; }

void Item_lr_0::print() const {
	cout << left << " -> ";
	for (size_t i = 0; i < separator; i++)
		cout << right[i] << " ";
	cout << ". ";
	for (size_t i = separator; i < right.size(); i++)
		cout << right[i] << " ";
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 1. Grammar is a vector of strings.
 2. Form of each string: <non_terminal> : sequense of terminals and non-terminals | sequense of terminals and non-terminals | ... | sequense of terminals and non-terminals
 3. There must be a space between each terminal, non-terminal, |, :.
 4. Names of terminals and non-terminals consists from A-Z, a-z, _ only.
 5. Each name of non-terminal mast be between <...>
 */
LR_0::LR_0() {};

//A constructor that inputs a grammar as a vector of strings and initial non-terminal.
LR_0::LR_0(vector<string>& grammar, string start_non_terminal) : start(start_non_terminal) {
	get_grammar(grammar); //parse a grammar

	for (auto& r : rules) //collecting all terminals and non-terminals
	{
		non_terminals.insert(r.left);
		for (auto& x : r.right)
			if (x[0] == '<' && x[x.size() - 1] == '>' && x.size() > 2)
				non_terminals.insert(x);
			else if (x != "")
				terminals.insert(x);
	}
	non_terminals.insert(start);
	terminals.insert("$"); //add $ to terminals

	//augumenting a grammar by new initial non-terminal and new start rule
	non_terminals.insert("<--->");
	string st = "<---> : " + start;
	st += " $";
	start_rule = Rule(st);
	start_rule.set_number(rules.size());
	rules.push_back(start_rule);
	
	//creating a start_item and a end_item
	start_item = Item_lr_0(start_rule, 0);
	end_item = Item_lr_0(start_rule, start_rule.right.size());
	end_item.set_reduct(true);

	set_automate(); //creating an automate
	set_tables();   //creating a tables GOTO and ACTION
}

//The function inputs grammar as a vector of strings and creates rules.
void LR_0::get_grammar(vector<string>& grammar) 
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

//The function inputs set of lr0-items and returns a closure of them.
set<Item_lr_0> LR_0::closure(set<Item_lr_0>& item_set) {
	set<Item_lr_0> cl_set = item_set;
	size_t sz_old = 0, sz_new = cl_set.size();

	while (sz_old != sz_new)
	{
		sz_old = sz_new;
		for (auto& item : cl_set)
		{
			string nt = item.get_next();
			if (is_non_terminal(nt))
				for (auto& r : rules)
					if (r.left == nt)
					{
						Item_lr_0 tmp = static_cast<Item_lr_0&>(r);
						tmp.separator = 0;
						cl_set.insert(tmp);
					}
		}
		sz_new = cl_set.size();
	}

	return cl_set;
}

//Transition function
set<Item_lr_0> LR_0::delta(const set<Item_lr_0>& item_set, string x) {
	set<Item_lr_0> ret;
	for (auto& item : item_set)
	{
		if (item.get_next() == x) {
			auto tmp = item;
			tmp.separator++;
			if (tmp.separator == tmp.right.size())
				tmp.set_reduct(true);
			ret.insert(tmp);
		}
	}
	return closure(ret);
}

//Function that create an automate
void LR_0::set_automate() 
{
	set<Item_lr_0> start_set = { start_item };
	set<set<Item_lr_0>> set_of_states = { closure(start_set) };
	size_t sz_old = 0, sz_new = set_of_states.size();

	while (sz_old != sz_new)
	{
		sz_old = sz_new;

		for (auto& state : set_of_states)
			for (auto& item : state)
				if (string str = item.get_next(); str != "")
					set_of_states.insert(delta(state, str));

		sz_new = set_of_states.size();
	}

	size_t i = 0;
	for (auto& state : set_of_states) {
		states.push_back(state);
		i++;
	}

	i = 0;
	for (auto& state : states) {
		for (auto& item : state)
			if (string str = item.get_next(); str != "")
				automate[i].insert({ distance(set_of_states.begin(), set_of_states.find(delta(state, str))), str });
		i++;
	}
}

//Function that create a tables ACTION adn GOTO
void LR_0::set_tables() {
	GOTO = vector<vector<size_t>>(states.size(), vector<size_t>(non_terminals.size(), 0));
	ACTION = vector<vector<pair<char, size_t>>>(states.size(), vector<pair<char, size_t>>(terminals.size(), { ' ', 0 }));

	size_t j = 0;
	for (auto& nt : non_terminals)
	{
		for (auto& [state, val] : automate)
			for (auto& [i, str] : val)
				if (nt == str) GOTO[state][j] = i;
		j++;
	}

	for (size_t j = 0; j < terminals.size(); j++)
	{
		size_t i = 0;
		for (auto& state : states)
		{
			for (auto& item : state)
			{
				if (item == end_item) {
					ACTION[i][j] = { 'a', 0 };
					break;
				}
				else if (item.reduct) {
					ACTION[i][j] = { 'r', item.number };
					break;
				}
			}
			i++;
		}
	}

	j = 0;
	for (auto& nt : terminals)
	{
		for (auto& [state, val] : automate)
			for (auto& [i, str] : val)
				if (nt == str) ACTION[state][j] = { 's', i };
		j++;
	}
}

bool LR_0::is_terminal(string x) { return terminals.count(x); }
bool LR_0::is_non_terminal(string x) { return non_terminals.count(x); }

//Reduce function
void LR_0::reduce(stack<pair<string, size_t>>& st, size_t n) {
	Rule r = rules[n];
	for (size_t i = 0; i < r.right.size(); i++)
		st.pop();

	size_t k = st.top().second;
	size_t m = distance(non_terminals.begin(), non_terminals.find(r.left));
	st.push({ r.left, GOTO[k][m] });
}

//Shift function
void LR_0::shift(stack<pair<string, size_t>>& st, string lookahead, size_t n) {
	st.push({ lookahead, n });
}

//Parser
vector<size_t> LR_0::parse(vector<string> input) {

	vector<size_t> ret;
	size_t pos = 0, i, ix;
	string x;
	stack<pair<string, size_t>> st;
	st.push({ "", 0 });

	while (true) {
		i = st.top().second;
		x = input[pos];
		ix = distance(terminals.begin(), terminals.find(x));

		if (pos == input.size() && ACTION[i][0].first == 'a') {
			cout << "Parsing sacces." << endl;
			return ret;
		}

		auto [act, n] = ACTION[i][ix];

		if (act == 's') {
			shift(st, x, n);
			pos++;
		}
		else if (act == 'r') {
			ret.push_back(n);
			reduce(st, n);
		}
		else {
			cout << "Error of parsing." << endl;
			return ret;
		}
	}

	return vector<size_t>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Print functions

void LR_0::print_rules(vector<size_t> rule_vec) {
	for (auto& i : rule_vec) {
		cout << rules[i].left << " -> ";
		for (auto& str : rules[i].right)
			cout << str << " ";
		cout << endl;
	}
}

void LR_0::print_grammar() {
	cout << "All rules:" << endl;
	for (auto& x : rules) {
		x.print();
		cout << endl;
	}
}

void LR_0::print_states() {
	size_t i = 0;
	for (auto& state : states) {
		cout << "State number " << i++ << ":" << endl;
		for (auto& item : state) {
			item.print();
			cout << endl;
		}
		cout << endl;
	}
}

void LR_0::print_atomate() {
	for (auto& state : automate) {
		cout << state.first << " : ";
		for (auto& p : state.second)
			cout << "[ " << p.first << " , " << p.second << " ]";
		cout << endl;
	}
}

void LR_0::print_goto() {
	cout << " ";
	for (auto& str : non_terminals)
		cout << str << "\t";

	cout << endl;

	for (size_t i = 0; i < GOTO.size(); i++) {
		cout << i << "\t";
		for (size_t j = 0; j < GOTO[0].size(); j++) {
			cout << GOTO[i][j] << "\t";
		}
		cout << endl;
	}
}

void LR_0::print_action() {
	cout << "\t";
	for (auto& str : terminals)
		cout << str << "\t";
	cout << endl;

	for (size_t i = 0; i < ACTION.size(); i++) {
		cout << i << "\t";
		for (size_t j = 0; j < ACTION[0].size(); j++) {
			cout << ACTION[i][j].first << "[" << ACTION[i][j].second << "]" << "\t";
		}
		cout << endl;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
