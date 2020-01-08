#include "stdafx.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "Stack.h"
#include "AnalizerLex.h"
#include "Executer.h"

extern Tabl_ident TID;
void Executer::execute(Poliz & prog)
{
	Stack < LexValue, 100 > args;
	int index = 0, size = prog.get_free(), resultCmp = 0;
	type_of_lex lastLex;
	LexValue i, j;
	while (index < size)
	{
		char str[100];
		LexValue resultLex;
		pc_el = prog[index];
		switch (pc_el.get_type())
		{
		case LEX_TRUE: case LEX_FALSE: case LEX_NUM: case LEX_INT:
		case POLIZ_ADDRESS: case POLIZ_LABEL: case LEX_STRING:
			args.push(pc_el.get_value());
			break;
		case LEX_ID:
			i = pc_el.get_value();
			if (TID[i.intVal].get_assign())
			{
				resultLex = TID[i.intVal].get_value();
				args.push(resultLex);
				break;
			}
			else
				throw "POLIZ: indefinite identifier";
		case LEX_NOT:
			j = args.pop();
			resultLex.intVal = !j.intVal;
			args.push(resultLex);
			break;
		case LEX_OR:
			i = args.pop();
			j = args.pop();
			resultLex.intVal = i.intVal || j.strVal;
			args.push(resultLex);
			break;
		case LEX_AND:
			i = args.pop();
			j = args.pop();
			resultLex.intVal = i.intVal && j.strVal;
			args.push(resultLex);
			break;
		case POLIZ_GO:
			i = args.pop();
			index = i.intVal - 1;
			break;
		case POLIZ_FGO:
			i = args.pop();
			if (!args.pop().intVal)
				index = i.intVal - 1;
			break;
		case LEX_WRITE:
			i = args.pop();
			if (i.lexType == LEX_INT || i.lexType == LEX_BOOL || i.lexType == LEX_NUM)
				std::cout << i.intVal << std::endl;
			else
				std::cout << i.strVal << std::endl;
			break;
		case LEX_READ:
		{
			LexValue currVal;
			int k;
			i = args.pop();
			type_of_lex currentValueType = TID[i.intVal].get_type();
			if (currentValueType == LEX_INT || currentValueType == LEX_STRING)
			{
				char *currentType = currentValueType == LEX_INT ? (char*)"int" : (char*)"string";
				std::cout << "Input " << currentType << " value for " << TID[i.intVal].get_name() << std::endl;
				if (currentValueType == LEX_INT)
				{
					std::cin >> k;
					currVal.intVal = k;
					currVal.lexType = currentValueType;
				}
				else
				{
					std::cin >> str;
					strcpy_s(currVal.strVal, strlen(str) + 1, str);
					currVal.intVal = 0;
					currVal.lexType = currentValueType;
				}
			}
			else
			{
				char j[20];
			rep:
				std::cout << "Input boolean value (true or false) for " << TID[i.intVal].get_name() << std::endl;
				std::cin >> j;
				if (!strcmp(j, "true"))
					currVal.intVal = k = 1;
				else
					if (!strcmp(j, "false"))
						currVal.intVal = k = 0;
					else
					{
						std::cout << "Error in input:true/false" << std::endl;
						goto rep;
					}
			}
			TID[i.intVal].put_value(currVal);
			TID[i.intVal].put_assign();
			break;
		}
		case LEX_PLUS:
			i = args.pop();
			j = args.pop();
			if (i.lexType == j.lexType && (i.lexType == LEX_INT || i.lexType == LEX_BOOL || i.lexType == LEX_NUM))
				resultLex.intVal = j.intVal + i.intVal;
			else
				throw "POLIZ invalid operation '+' for type string";
			args.push(resultLex);
			break;
		case LEX_TIMES:
			i = args.pop();
			j = args.pop();
			if (i.lexType == j.lexType && (i.lexType == LEX_INT || i.lexType == LEX_NUM))
				resultLex.intVal = j.intVal * i.intVal;
			else
				throw "POLIZ invalid operation '*' for type string";
			args.push(resultLex);
			break;
		case LEX_MINUS:
			i = args.pop();
			j = args.pop();
			if (i.lexType == j.lexType && (i.lexType == LEX_INT || i.lexType == LEX_NUM))
				resultLex.intVal = j.intVal - i.intVal;
			else
				throw "POLIZ invalid operation '-' for type string";
			args.push(resultLex);
			break;
		case LEX_SLASH:
			i = args.pop();
			if (!i.intVal)
			{
				resultLex.intVal = args.pop().intVal / i.intVal;
				args.push(resultLex);
				break;
			}
			else
				throw "POLIZ:divide by zero";
		case LEX_EQ:
			i = args.pop();
			j = args.pop();
			if (i.lexType == j.lexType && (i.lexType == LEX_INT || i.lexType == LEX_NUM))
				resultLex.intVal = j.intVal == i.intVal;
			else
				resultCmp = strcmp(j.strVal, i.strVal);
				resultLex.intVal = resultCmp < 0 || resultCmp > 0 ? 0 : 1;
			args.push(resultLex);
			break;
		case LEX_LSS:
			i = args.pop();
			j = args.pop();
			if (i.lexType == j.lexType && (i.lexType == LEX_INT || i.lexType == LEX_NUM))
				resultLex.intVal = j.intVal < i.intVal;
			else
				throw "POLIZ invalid operation '<' for type string";
			args.push(resultLex);
			break;
		case LEX_GTR:
			i = args.pop();
			j = args.pop();
			if (i.lexType == j.lexType && (i.lexType == LEX_INT || i.lexType == LEX_BOOL || i.lexType == LEX_NUM))
				resultLex.intVal = j.intVal > i.intVal;
			else
				throw "POLIZ invalid operation '>' for type string";
			args.push(resultLex);
			break;
		case LEX_LEQ:
			i = args.pop();
			j = args.pop();
			if (i.lexType == j.lexType && (i.lexType == LEX_INT || i.lexType == LEX_BOOL || i.lexType == LEX_NUM))
				resultLex.intVal = j.intVal <= i.intVal;
			else
				throw "POLIZ invalid operation '<=' for type string";
			args.push(resultLex);
			break;
		case LEX_GEQ:
			i = args.pop();
			j = args.pop();
			resultLex.intVal = j.intVal >= i.intVal;
			if (i.lexType == j.lexType && (i.lexType == LEX_INT || i.lexType == LEX_BOOL || i.lexType == LEX_NUM))
				resultLex.intVal = j.intVal >= i.intVal;
			else
				throw "POLIZ invalid operation '>=' for type string";
			args.push(resultLex);
			break;
		case LEX_NEQ:
			i = args.pop();
			j = args.pop();
			if (i.lexType == j.lexType && (i.lexType == LEX_INT || i.lexType == LEX_BOOL || i.lexType == LEX_NUM))
				resultLex.intVal = j.intVal != i.intVal;
			else
				throw "POLIZ invalid operation '!' for type string";
			args.push(resultLex);
			break;
		case LEX_ASSIGN:
			i = args.pop();
			j = args.pop();
			TID[j.intVal].put_value(i);
			TID[j.intVal].put_assign();
			break;
		case LEX_NULL: break;
		default:
			throw "POLIZ: unexpected elem";
		}//end of switch
		++index;
	};//end of while
	std::cout << "Finish of executing!!!" << std::endl;
}
