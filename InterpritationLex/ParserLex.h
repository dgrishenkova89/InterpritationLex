#include "stdafx.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "Stack.h"
#include "AnalizerLex.h"

#pragma once
class Parser
{
	Lex          curr_lex;
	type_of_lex  c_type = LEX_NULL;
	LexValue          c_val;
	Scanner      scan;
	Stack < LexValue, 100 > st_int;
	Stack < type_of_lex, 100 >  st_lex;
	void         P();
	void         DeclareVariable();
	void         D();
	void         Body();
	void         S();
	void          E();
	void         E1();
	void         T();
	void         F();
	void         dec(type_of_lex type, LexValue val);
	void         check_id();
	void         check_op();
	void         check_not();
	void         eq_type();
	void         eq_bool();
	void         check_id_in_read();
	void         gl()
	{
		curr_lex = scan.get_lex();
		c_type = curr_lex.get_type();
		c_val.lexType = c_type;
		if (c_type == LEX_STRING)
		{
			if (strlen(curr_lex.get_value().strVal) > 0)
				strcpy_s(c_val.strVal, strlen(curr_lex.get_value().strVal) + 1, curr_lex.get_value().strVal);
		}
		else
			c_val.intVal = curr_lex.get_value().intVal;
	}
public:
	Poliz        prog;
	Parser(const char *program) : scan(program), prog(1000) {}
	void         analyze();
};