#include "stdafx.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "Stack.h"
#include "AnalizerLex.h"
#include "ParserLex.h"

extern Tabl_ident TID;
void Parser::analyze()
{
	gl();
	P();
	prog.print();
	std::cout << std::endl << "Yes!!!" << std::endl;
}

////////////////////////////////////////////////////////////////
void Parser::P()
{
	if (c_type == LEX_PROGRAM)
		gl();
	else
		throw curr_lex;
	if (c_type == LEX_BEGIN)
		gl();
	else
		throw curr_lex;
	if (c_type == LEX_INT || c_type == LEX_BOOL || c_type == LEX_STRING)
		DeclareVariable();
	else
		throw curr_lex;
	Body();
	if (c_type != LEX_FIN)
		throw curr_lex;
}

void Parser::DeclareVariable()
{
	if (c_type == LEX_INT || c_type == LEX_BOOL || c_type == LEX_STRING)
	{
		type_of_lex thisType = c_type;
		gl();
		if (c_type == LEX_ID)
		{
			D();
			if (c_type == LEX_ASSIGN) gl();
			// ƒобавл€ем полученный тип данных и смотрим следующие лексемы
			dec(thisType, c_val);
			if (c_type != LEX_SEMICOLON) gl();
		}
		// ќбъ€вление лексем через ","
		while (c_type == LEX_COMMA)
		{
			gl();
			if (c_type == LEX_ID)
			{
				LexValue lastVal = c_val;
				D();
				type_of_lex secondLex = c_type;
				if (secondLex == LEX_ASSIGN) gl();
				// ƒобавл€ем полученный тип данных и смотрим следующие лексемы
				if ((c_type != LEX_SEMICOLON && c_type != LEX_COMMA) ||
					(lastVal.lexType == LEX_ID && (c_type == LEX_SEMICOLON || c_type == LEX_COMMA)))
				{
					LexValue currentLex =
						(lastVal.lexType == LEX_ID && (c_type == LEX_SEMICOLON || c_type == LEX_COMMA)) ?
						lastVal : c_val;
					if (secondLex != LEX_ASSIGN)
						c_val.intVal = NULL;

					dec(thisType, currentLex);
					if (c_type != LEX_SEMICOLON && c_type != LEX_COMMA) gl();
				}
			}
			else throw curr_lex;
		}
		if (c_type != LEX_SEMICOLON)
			throw curr_lex;
		gl();
		DeclareVariable();
	}
}

void Parser::D()
{
	st_int.reset();
	if (c_type != LEX_ID)
		throw curr_lex;
	else
		st_int.push(c_val);
	gl();
}

void Parser::Body()
{
	S();
	while (c_type == LEX_SEMICOLON || c_type == LEX_BEGIN || c_type == LEX_END)
	{
		gl();
		S();
	}
	if (c_type == LEX_FIN)
		return;
	else
		throw curr_lex;
}

void Parser::S()
{
	int pl0, pl1, pl2, pl3;

	if (c_type == LEX_IF)
	{
		type_of_lex firstLex = c_type;
		gl();
		E();
		eq_bool();
		pl2 = prog.get_free();
		prog.blank();
		prog.put_lex(Lex(POLIZ_FGO));
		if (c_type == LEX_BEGIN)
		{
			gl();
			S();
			while (c_type == LEX_SEMICOLON)
			{	
				gl();
				if (c_type == LEX_END) break;
				S();
				if (c_type == LEX_END) break;
			}
			if (c_type != LEX_END) throw curr_lex;
			prog.blank();
			prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl2);
		}
		else throw curr_lex;
		gl();
		S();
	}//end if
	else if (c_type == LEX_WHILE)
	{
		pl0 = prog.get_free();
		gl();
		E();
		eq_bool();
		pl1 = prog.get_free();
		prog.blank();
		prog.put_lex(Lex(POLIZ_FGO));
		if (c_type == LEX_BEGIN)
		{
			prog.blank();
			prog.put_lex(Lex(POLIZ_LABEL, pl0));
			gl();
			S();
			while (c_type == LEX_SEMICOLON)
			{
				gl();
				if (c_type == LEX_END) break;
				S();
				if (c_type == LEX_END) break;
			}
			if (c_type != LEX_END) throw curr_lex;
			prog.blank();
			prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl1);
		}
		else throw curr_lex;
		gl();
		S();
	}//end while
	else if (c_type == LEX_READ)
	{
		gl();
		if (c_type == LEX_LPAREN)
		{
			gl();
			if (c_type == LEX_ID) {
				check_id_in_read();
				prog.put_lex(Lex(POLIZ_ADDRESS, c_val.intVal));
				gl();
			}
			else
				throw curr_lex;
			if (c_type == LEX_RPAREN)
			{
				gl();
				prog.put_lex(Lex(LEX_READ));
			}
			else
				throw curr_lex;
		}
		else
			throw curr_lex;
	}//end read
	else if (c_type == LEX_WRITE)
	{
		gl();
		if (c_type == LEX_LPAREN)
		{
			gl();
			E();
			while (c_type == LEX_COMMA)
			{
				gl();
				E();
			}
			if (c_type == LEX_RPAREN)
			{
				gl();
				prog.put_lex(Lex(LEX_WRITE));
			}
			else
				throw curr_lex;
		}
		else
			throw curr_lex;
	}//end write
	else if (c_type == LEX_ID)
	{
		check_id();
		prog.put_lex(Lex(POLIZ_ADDRESS, c_val.intVal));
		gl();
		if (c_type == LEX_ASSIGN)
		{
			gl();
			E();
			eq_type();
			prog.put_lex(Lex(LEX_ASSIGN));
		}
		else
			throw curr_lex;
	}//assign-end
	else if (c_type == LEX_SEMICOLON) gl();
	else if (c_type == LEX_BEGIN || c_type == LEX_END || c_type == LEX_FIN) return;
	else
		Body();
}

void Parser::E()
{
	E1();
	if (c_type == LEX_EQ || c_type == LEX_LSS || c_type == LEX_GTR ||
		c_type == LEX_LEQ || c_type == LEX_GEQ || c_type == LEX_NEQ)
	{
		st_lex.push(c_type);
		gl();
		E1();
		check_op();
	}
}

void Parser::E1()
{
	T();
	while (c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_OR)
	{
		if (c_type == LEX_OR)
			check_op();
		st_lex.push(c_type);
		gl();
		T();
		if (c_type == LEX_EQ || c_type == LEX_LSS || c_type == LEX_GTR ||
			c_type == LEX_LEQ || c_type == LEX_GEQ || c_type == LEX_NEQ)
		{
			st_lex.push(c_type);
			gl();
			T();
		}
		check_op();
	}
}

void Parser::T()
{
	F();
	while (c_type == LEX_TIMES || c_type == LEX_SLASH || c_type == LEX_AND)
	{
		if (c_type == LEX_AND)
			check_op();
		st_lex.push(c_type);
		gl();
		F();
		check_op();
	}
}

void Parser::F()
{
	if (c_type == LEX_ID)
	{
		check_id();
		prog.put_lex(Lex(LEX_ID, c_val.intVal));
		gl();
	}
	else if (c_type == LEX_NUM || c_type == LEX_INT)
	{
		st_lex.push(LEX_INT);
		prog.put_lex(curr_lex);
		gl();
	}
	else if (c_type == LEX_STRING)
	{
		st_lex.push(LEX_STRING);
		prog.put_lex(curr_lex);
		gl();
	}
	else if (c_type == LEX_TRUE)
	{
		st_lex.push(LEX_BOOL);
		prog.put_lex(Lex(LEX_TRUE, 1));
		gl();
	}
	else if (c_type == LEX_FALSE)
	{
		st_lex.push(LEX_BOOL);
		prog.put_lex(Lex(LEX_FALSE, 0));
		gl();
	}
	else if (c_type == LEX_NOT)
	{
		gl();
		F();
		check_not();
	}
	else if (c_type == LEX_LPAREN)
	{
		gl();
		E();
		if (c_type != LEX_RPAREN)
			throw curr_lex;
		else
			gl();
	}
	else if (c_type == LEX_PLUS)
	{
		gl();
		T();
	}
	else
		throw curr_lex;
}

////////////////////////////////////////////////////////////////
void Parser::dec(type_of_lex type, LexValue val)
{
	LexValue i;
	while (!st_int.is_empty())
	{
		i = st_int.pop();
		if (TID[i.intVal].get_declare())
			throw "twice";
		else
		{
			TID[i.intVal].put_declare();
			TID[i.intVal].put_type(type);
			TID[i.intVal].put_value(val);
			if ((val.lexType == LEX_STRING && val.strVal != (char*)"") ||
				((val.lexType == LEX_INT || val.lexType == LEX_NUM || val.lexType == LEX_BOOL) && val.intVal != NULL))
				TID[i.intVal].put_assign();
		}
	}
}

void Parser::check_id()
{
	if (TID[c_val.intVal].get_declare())
		st_lex.push(TID[c_val.intVal].get_type());
	else
	{
		char *name = TID[c_val.intVal].get_name();
		throw "not declared";
	}
}

void Parser::check_op()
{
	type_of_lex t1, t2, op, t = LEX_INT;

	t2 = st_lex.pop();
	op = st_lex.pop();
	t1 = st_lex.pop();
	if (op == LEX_PLUS || op == LEX_EQ || op == LEX_GEQ || op == LEX_GTR || op == LEX_LSS)
	{
		if (t1 == t2 && t1 == LEX_INT && !(op == LEX_GEQ || op == LEX_EQ || op == LEX_GTR || op == LEX_LSS))
			t = LEX_INT;
		if (t1 == t2 && t1 == LEX_STRING && !(op == LEX_GEQ || op == LEX_EQ || op == LEX_GTR || op == LEX_LSS))
			t = LEX_STRING;
		if (t1 == t2 && (op == LEX_GEQ || op == LEX_EQ || op == LEX_GTR || op == LEX_LSS))
			t = LEX_BOOL;
	}
	else if (op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH)
		t = LEX_INT;
	else if (op == LEX_OR || op == LEX_AND)
		t = LEX_BOOL;
	if (t1 == t2 && !(op == LEX_OR || op == LEX_AND || op == LEX_EQ || op == LEX_GEQ || op == LEX_GTR || op == LEX_LSS))
		st_lex.push(t);
	else if (op == LEX_OR || op == LEX_AND || op == LEX_EQ || op == LEX_GEQ || op == LEX_GTR || op == LEX_LSS)
		st_lex.push(t);
	else
		throw "wrong types are in operation";
	prog.put_lex(Lex(op));
}

void Parser::check_not()
{
	if (st_lex.pop() != LEX_BOOL)
		throw "wrong type is in not";
	else
	{
		st_lex.push(LEX_BOOL);
		prog.put_lex(Lex(LEX_NOT));
	}
}

void Parser::eq_type()
{
	type_of_lex first = st_lex.pop();
	type_of_lex second = st_lex.pop();
	if (first != second)
		throw "wrong types are in =";
}

void Parser::eq_bool()
{
	if (st_lex.pop() != LEX_BOOL)
		throw "expression is not boolean";
}

void Parser::check_id_in_read()
{
	if (!TID[c_val.intVal].get_declare())
		throw "not declared";
}