//
//  main.cpp
//  LR_Parser
//
//  Created by Dan Lemberg on 06/03/2019.
//  Copyright © 2019 Dan Lemberg. All rights reserved.
//

#include "LR_1.h"

int main(int argc, const char* argv[]) 
{
	/* Take notice - epsilon is "e", do not use "e" as a terminal	*/

	// Example 1
	vector<string> gr1 = { "<E> : <T> | <E> + <T>", "<T> : i | ( <E> )" };
	vector<string> GoodInput1 = { "(", "i", "+", "i", ")", "$" };
	vector<string> BadInput1 = { "i", "+", "i", ")", "$" };
	// Example 2

	vector<string> gr2 = { "<S> : <B> c d  |  <E> c f " ,"<B> : x y","<E> : x y"};


	// Example 3
	vector<string> gr3 = { "<R> : <L>","<S> : <R> | <L> = <R>", "<L> : x | * <R>" };
	vector<string> GoodInput3 = { "*","x", "=", "x",  "$" };
	vector<string> BadInput3 = { "x","*", "=", "x", "x", "$" };

	// Example 4

	vector<string> gr4 = { "<S> : <E>", "<E> : 1 <E> | 1" };
	vector<string> GoodInput4 = { "1", "1", "1", "1","$" };
	vector<string> BadInput4 = { "1", "1", "1", "2", "$" };
	// Parsers for each examples
	cout  << "Create Parsers:" << endl;
	LR_1 LR_gr1(gr1, "<E>"), 
		LR_gr2(gr2, "<S>") ,
		LR_gr3(gr3, "<S>"), 
		LR_gr4(gr4, "<S>");
	cout << endl << "Print parsers details and parse good and bad example:" << endl;
	// Parsing examples:
	// 1
	cout << endl << "Example 1:" << endl;
	LR_gr1.print_ALL();
	cout << endl << "Parsing:" << endl;
	LR_gr1.parse(GoodInput1);
	LR_gr1.parse(BadInput1);
	//2
	cout << endl << "Example 2:" << endl;
	LR_gr2.print_ALL();
	//3
	cout << endl << "Example 3:" << endl;
	LR_gr3.print_ALL();
	cout << endl << "Parsing:" << endl;
	LR_gr3.parse(GoodInput3);
	LR_gr3.parse(BadInput3);
	//4
	cout << endl << "Example 4:" << endl;
	LR_gr4.print_ALL();
	cout << endl << "Parsing:" << endl;
	LR_gr4.parse(GoodInput4);
	LR_gr4.parse(BadInput4);
	


	return 0;
}
