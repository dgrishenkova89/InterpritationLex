#include "stdafx.h"
#include "AnalizerLex.h"

Tabl_ident TID(100);
int Tabl_ident::put(const char *buf)
{
	for (int j = 1; j < top; j++)
		if (!strcmp(buf, p[j].get_name()))
			return j;
	p[top].put_name(buf);
	++top;
	return top - 1;
}

Lex Scanner::get_lex()
{
	int d, j, b = -1;

	CS = H;
	do
	{
		switch (CS)
		{
		case H:
			if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
				gc();
			else if (c == '{' || c == '}')
			{
				type_of_lex currentLex = c == '{' ? LEX_BEGIN : LEX_END;
				gc();
				return Lex(currentLex);
			}
			else if (isalpha(c))
			{
				clear();
				add();
				gc();
				CS = IDENT;
			}
			else if (isdigit(c))
			{
				d = c - '0';
				gc();
				CS = NUMB;
			}
			else if (c == '/')
			{
				gc();
				CS = COM;
			}
			else if (c == '"')
			{
				d = c;
				clear();
				gc();
				CS = STR;
			}
			else if (c == ',')
			{
				gc();
				return Lex(LEX_COMMA);
			}
			else if (c == '=' || c == '<' || c == '>' || c == '(' || c == ')')
			{
				clear();
				add();
				gc();
				CS = ALE;
			}
			else if (c == '@')
				return Lex(LEX_FIN);
			else if (c == '!')
			{
				clear();
				add();
				gc();
				CS = NEQ;
			}
			else
				CS = DELIM;
			break;
		case IDENT:
			if (isalpha(c) || isdigit(c))
			{
				add();
				gc();
			}
			else if (j = look(buf, TW))
				return Lex(words[j], j);
			else
			{
				j = TID.put(buf);
				return Lex(LEX_ID, j);
			}
			break;
		case NUMB:
			if (isdigit(c))
			{
				d = d * 10 + (c - '0'); gc();
			}
			else
				return Lex(LEX_NUM, d);
			break;
		case ALE:
			if (c == '=')
			{
				add();
				gc();
			}
			j = look(buf, TD);
			return Lex(dlms[j], j);
		case STR:
			if (c == '\n' || c == '\r' || c == '\t')
				throw c;
			if (c != '"')
			{
				d += c;
				add();
				gc();
			}
			else
			{
				gc();
				return Lex(LEX_STRING, d, buf);
			}
			break;
		case NEQ:
			if (c == '=')
			{
				add();
				gc();
				j = look(buf, TD);
				return Lex(LEX_NEQ, j);
			}
			else
				throw '!';
			break;
		case DELIM:
			clear();
			add();
			if (j = look(buf, TD))
			{
				gc();
				return Lex(dlms[j], j);
			}
			else
				throw c;
			break;
		// Пробегаем комментарий до переноса
		case COM:
			if (c == '\n')
			{
				gc();
				CS = H;
			}
			else if (c == '@' || c == '{')
				throw c;
			else
				gc();
			break;
		}
	} while (true);
}

char *
Scanner::TW[] = { (char*)"", (char*)"and", (char*)"{", (char*)"bool", (char*)"string", (char*)"do", (char*)"else",
(char*)"}", (char*)"if", (char*)"false", (char*)"int", (char*)"not", (char*)"or", (char*)"program",
(char*)"read", (char*)"then", (char*)"true", (char*)"var", (char*)"while", (char*)"write", NULL };

type_of_lex
Scanner::words[] = { LEX_NULL, LEX_AND, LEX_BEGIN, LEX_BOOL, LEX_STRING, LEX_DO, LEX_ELSE, LEX_END, LEX_IF, LEX_FALSE, LEX_INT,
LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_THEN, LEX_TRUE, LEX_VAR, LEX_WHILE, LEX_WRITE, LEX_NULL };

char *
Scanner::TD[] = { (char*)"", (char*)"@", (char*)";", (char*)",", (char*)":", (char*)"=", (char*)"(",
(char*)")", (char*)"==", (char*)"<", (char*)">", (char*)"+", (char*)"-", (char*)"*",
(char*)"/", (char*)"<=", (char*)"!=", (char*)">=", NULL };

type_of_lex
Scanner::dlms[] = { LEX_NULL, LEX_FIN, LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_EQ,
LEX_LSS, LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ, LEX_NULL };