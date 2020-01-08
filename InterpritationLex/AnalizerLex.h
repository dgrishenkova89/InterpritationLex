#include "stdafx.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "Stack.h"

#pragma once
enum type_of_lex
{
	LEX_NULL, /*0*/
	LEX_AND, LEX_BEGIN, LEX_BOOL, LEX_DO, LEX_ELSE, LEX_END, LEX_IF, LEX_FALSE, LEX_INT, /*9*/
	LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_THEN, LEX_TRUE, LEX_VAR, LEX_WHILE, LEX_WRITE, /*18*/
	LEX_FIN, /*19*/
	LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_EQ, LEX_LSS, /*27*/
	LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ, /*35*/
	LEX_NUM, /*36*/
	LEX_ID, /*37*/
	POLIZ_LABEL, /*38*/
	POLIZ_ADDRESS, /*39*/
	POLIZ_GO, /*40*/
	POLIZ_FGO,
	LEX_STRING
}; /*42*/

   /////////////////////////  Класс Lex  //////////////////////////
struct LexValue
{
	int intVal;
	char strVal[10];
	type_of_lex lexType;
};


class Lex
{
	type_of_lex t_lex;
	LexValue v_lex;
public:
	Lex(type_of_lex t = LEX_NULL, int _intVal = 0, char* _charVal = (char*)"") : t_lex(t)
	{
		v_lex.intVal = _intVal;
		if (strlen(_charVal) > 0)
		{
			_charVal = _charVal + '\0';
			strcpy_s(v_lex.strVal, strlen(_charVal) + 1, _charVal);
		}
		v_lex.lexType = t;
	}
	type_of_lex  get_type() { return t_lex; }
	LexValue     get_value() { return v_lex; }
	friend std::ostream &    operator<< (std::ostream &s, Lex l)
	{
		s << '(' << l.t_lex << ',' << l.v_lex.intVal << ");";
		return s;
	}
};

class Ident
{
	char       * name;
	bool         declare;
	type_of_lex  type;
	bool         assign;
	LexValue     value;
public:
	Ident() { declare = false; assign = false; }
	char       * get_name() { return name; }
	void         put_name(const char *n)
	{
		int size = strlen(n) + 1;
		name = new char[size];
		strcpy_s(name, size, n);
	}
	bool         get_declare() { return declare; }
	void         put_declare() { declare = true; }
	type_of_lex  get_type() { return type; }
	void         put_type(type_of_lex t) { type = t; }
	bool         get_assign() { return assign; }
	void         put_assign() { assign = true; }
	LexValue     get_value() { return value; }
	void         put_value(LexValue v) { value = v; }
};

class Tabl_ident
{
	Ident      * p;
	int          size;
	int          top;
public:
	Tabl_ident(int max_size)
	{
		p = new Ident[size = max_size];
		top = 1;
	}
	~Tabl_ident() { delete[] p; }
	Ident      & operator[] (int k) 
	{
		int c = 1;
		return p[k];
	}
	int          put(const char *buf);
};

class Poliz
{
	Lex        * p;
	int          size;
	int          free;
public:
	Poliz(int max_size)
	{
		p = new Lex[size = max_size];
		free = 0;
	}
	~Poliz() { delete[] p; }
	void         put_lex(Lex l)
	{
		p[free] = l;
		free++;
	}
	void         put_lex(Lex l, int place) { p[place] = l; }
	void         blank() { free++; }
	int          get_free() { return free; }
	Lex        & operator[] (int index)
	{
		if (index > size)
			throw "POLIZ:out of array";
		else
			if (index > free)
				throw "POLIZ:indefinite element of array";
			else
				return p[index];
	}
	void         print()
	{
		for (int i = 0; i < free; ++i)
			std::cout << p[i];
	}
};

class Scanner
{
	enum         state { H, BEGIN, END, IDENT, NUMB, STR, COM, ALE, PARSEVAL, DELIM, NEQ };
	static char       * TW[];
	static type_of_lex  words[];
	static char       * TD[];
	static type_of_lex  dlms[];
	state        CS;
	FILE       * fp;
	char         c;
	char         buf[80];
	int          buf_top;
	void         clear()
	{
		buf_top = 0;
		for (int j = 0; j < 80; j++)
			buf[j] = '\0';
	}
	void         add()
	{
		buf[buf_top++] = c;
	}
	int          look(const char *buf, char **list)
	{
		int i = 0;
		while (list[i])
		{
			if (!strcmp(buf, list[i]))
				return i;
			++i;
		}
		return 0;
	}
	void         gc()
	{
		c = fgetc(fp);
	}
public:
	Scanner(const char * program)
	{
		fopen_s(&fp, program, "r");
		CS = H;
		clear();
		gc();
	}
	Lex          get_lex();
};