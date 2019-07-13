//
//  LR_0.h
//  LR_Parser
//
//  Created by Dan Lemberg on 10/03/2019.
//  Copyright © 2019 Dan Lemberg. All rights reserved.
//

#ifndef LR_0_h
#define LR_0_h

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <algorithm>
#include <string>
using namespace std;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Rule{
public:
    string left;
    vector<string> mutable right;
    long number = -1;
    
    
    Rule(){}
    Rule(string left_, vector<string> right_);
    Rule(string str_rule);
    
    void set_number(size_t num);
    virtual bool operator < (const Rule& x) const;
    virtual bool operator == (const Rule& x) const;
    virtual void print() const;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Item_lr_0 : public Rule {
public:
    size_t separator; //place of dot_separator (before symbol namber - separator)
    bool mutable reduct = false;
    
    Item_lr_0();
    Item_lr_0(string left_, vector<string> right_, size_t separator_);
    Item_lr_0(string str_rule, size_t separator_);
    Item_lr_0(Rule& rule, size_t separator_);
    
    bool operator < (const Item_lr_0& x) const;
    bool operator == (const Item_lr_0& x) const;
    
    void set_reduct(bool r);
    string get_next() const;
    void print() const;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LR_0{
    
    string start;
    Rule start_rule;
    Item_lr_0 start_item;
    Item_lr_0 end_item;
    set<string> terminals = {};
    set<string> non_terminals = {};
    vector<Rule> rules;
    vector<vector<pair<char, size_t>>> ACTION;
    vector<vector<size_t>> GOTO;
    
    vector<set<Item_lr_0>> states;
    map<size_t, set<pair<size_t, string>>> automate;
    
    void get_grammar(vector<string>& grammar);
    set<Item_lr_0> closure(set<Item_lr_0>& item_set);
    set<Item_lr_0> delta(const set<Item_lr_0>& item_set, string x);
    void set_automate();
    void set_tables();
    
    bool is_terminal(string x);
    bool is_non_terminal(string x);
    
    void reduce(stack<pair<string, size_t>>& st, size_t n);
    void shift(stack<pair<string, size_t>>& st, string lookahead, size_t n);
    
public:
    LR_0();
    LR_0(vector<string>& grammar, string start_non_terminal);
    
    vector<size_t> parse(vector<string> input);
    
    void print_grammar();
    void print_states();
    void print_atomate();
    void print_goto();
    void print_action();
    void print_rules(vector<size_t> rule_vec);
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif /* LR_0_h */
