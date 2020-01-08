#include "stdafx.h"
#include "Source.h"

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
	int d, j;

	CS = H;
	do
	{
		switch (CS)
		{
		case H:
			if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
				gc();
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
			else if (c == '//')
			{
				gc();
				CS = COM;
			}
			else if (c == ':' || c == '<' || c == '>')
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
		case COM:
			if (c == '//')
			{
				gc();
				CS = H;
			}
			else if (c == '@' || c == '{')
				throw c;
			else
				gc();
			break;
		case ALE:
			if (c == '=')
			{
				add();
				gc();
				j = look(buf, TD);
				return Lex(dlms[j], j);
			}
			else
			{
				j = look(buf, TD);
				return Lex(dlms[j], j);
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
		}//end switch
	} while (true);
}

void Parser::analyze()
{
	gl();
	Start();
	prog.print();
	std::cout << std::endl << "Yes!!!" << std::endl;
}

void Parser::Start()
{
	if (c_type == LEX_USING)
	{
		gl();
		while (c_type == LEX_SEMICOLON)
		{
			gl();
			if (c_type != LEX_USING)
				NS();
		}
	}
	else
		throw curr_lex;
	D1();
	if (c_type == LEX_SEMICOLON)
		gl();
	else
		throw curr_lex;
	B();
	if (c_type != LEX_FIN)
		throw curr_lex;
}

void Parser::D1()
{
	if (c_type == LEX_INT || c_type == LEX_BOOL || c_type == LEX_STRING)
	{
		gl();
		D();
		while (c_type == LEX_COMMA)
		{
			gl();
			D();
		}
	}
	else
		throw curr_lex;
}

void Parser::D()
{
	st_int.reset();
	if (c_type != LEX_ID)
		throw curr_lex;
	else
	{
		st_int.push(c_val);
		gl();
		while (c_type == LEX_COMMA)
		{
			gl();
			if (c_type != LEX_ID)
				throw curr_lex;
			else
			{
				st_int.push(c_val);
				gl();
			}
		}
		if (c_type != LEX_COLON)
			throw curr_lex;
		else
		{
			gl();
			if (c_type == LEX_INT)
			{
				dec(LEX_INT);
				gl();
			}
			else
				if (c_type == LEX_BOOL)
				{
					dec(LEX_BOOL);
					gl();
				}
				else throw curr_lex;
		}
	}
}

void Parser::B()
{
	if (c_type == LEX_LCBRACE)
	{
		gl();
		S();
		while (c_type == LEX_SEMICOLON)
		{
			gl();
			S();
		}
		if (c_type == LEX_RCBRACE)
			gl();
		else
			throw curr_lex;
	}
	else
		throw curr_lex;
}

void Parser::S()
{
	int pl0, pl1, pl2, pl3;

	if (c_type == LEX_IF)
	{
		gl();
		E();
		eq_bool();
		pl2 = prog.get_free();
		prog.blank();
		prog.put_lex(Lex(POLIZ_FGO));
		if (c_type == LEX_DEFAULT)
		{
			gl();
			S();
			pl3 = prog.get_free();
			prog.blank();
			prog.put_lex(Lex(POLIZ_GO));
			prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl2);
			if (c_type == LEX_ELSE)
			{
				gl();
				S();
				prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl3);
			}
			else
				throw curr_lex;
		}
		else
			throw curr_lex;
	}//end if
	else if (c_type == LEX_WHILE)
	{
		pl0 = prog.get_free();
		gl();
		E();
		eq_bool();
		pl1 = prog.get_free(); prog.blank();
		prog.put_lex(Lex(POLIZ_FGO));
		if (c_type == LEX_STRING)
		{
			gl();
			S();
			prog.put_lex(Lex(POLIZ_LABEL, pl0));
			prog.put_lex(Lex(POLIZ_GO));
			prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl1);
		}
		else
			throw curr_lex;
	}//end while
	else if (c_type == LEX_READ)
	{
		gl();
		if (c_type == LEX_LPAREN)
		{
			gl();
			if (c_type == LEX_ID) {
				check_id_in_read();
				prog.put_lex(Lex(POLIZ_ADDRESS, c_val));
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
		prog.put_lex(Lex(POLIZ_ADDRESS, c_val));
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
	else
		B();
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
		st_lex.push(c_type);
		gl();
		T();
		check_op();
	}
}

void Parser::T()
{
	F();
	while (c_type == LEX_TIMES || c_type == LEX_SLASH || c_type == LEX_AND)
	{
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
		prog.put_lex(Lex(LEX_ID, c_val));
		gl();
	}
	else if (c_type == LEX_NUM)
	{
		st_lex.push(LEX_INT);
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
		if (c_type == LEX_RPAREN)
			gl();
		else
			throw curr_lex;
	}
	else
		throw curr_lex;
}

void Parser::dec(type_of_lex type)
{
	int i;
	while (!st_int.is_empty())
	{
		i = st_int.pop();
		if (TID[i].get_declare())
			throw "twice";
		else
		{
			TID[i].put_declare();
			TID[i].put_type(type);
		}
	}
}

void Parser::check_id()
{
	if (TID[c_val].get_declare())
		st_lex.push(TID[c_val].get_type());
	else
		throw "not declared";
}

void Parser::check_op()
{
	type_of_lex t1, t2, op, t = LEX_INT, r = LEX_BOOL;

	t2 = st_lex.pop();
	op = st_lex.pop();
	t1 = st_lex.pop();
	if (op == LEX_PLUS || op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH)
		r = LEX_INT;
	if (op == LEX_OR || op == LEX_AND)
		t = LEX_BOOL;
	if (t1 == t2 && t1 == t)
		st_lex.push(r);
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
	if (st_lex.pop() != st_lex.pop())
		throw "wrong types are in :=";
}

void Parser::eq_bool()
{
	if (st_lex.pop() != LEX_BOOL)
		throw "expression is not boolean";
}

void Parser::check_id_in_read()
{
	if (!TID[c_val].get_declare())
		throw "not declared";
}

void Executer::execute(Poliz & prog)
{
	Stack < int, 100 > args;
	int i, j, index = 0, size = prog.get_free();
	while (index < size)
	{
		pc_el = prog[index];
		switch (pc_el.get_type())
		{
		case LEX_TRUE: case LEX_FALSE: case LEX_NUM: case POLIZ_ADDRESS: case POLIZ_LABEL:
			args.push(pc_el.get_value());
			break;
		case LEX_ID:
			i = pc_el.get_value();
			if (TID[i].get_assign())
			{
				args.push(TID[i].get_value());
				break;
			}
			else
				throw "POLIZ: indefinite identifier";
		case LEX_NOT:
			args.push(!args.pop());
			break;
		case LEX_OR:
			i = args.pop();
			args.push(args.pop() || i);
			break;
		case LEX_AND:
			i = args.pop();
			args.push(args.pop() && i);
			break;
		case POLIZ_GO:
			index = args.pop() - 1;
			break;
		case POLIZ_FGO:
			i = args.pop();
			if (!args.pop()) index = i - 1;
			break;
		case LEX_WRITE:
			std::cout << args.pop() << std::endl;
			break;
		case LEX_READ:
		{
			int k;
			i = args.pop();
			if (TID[i].get_type() == LEX_INT)
			{
				std::cout << "Input int value for" << TID[i].get_name() << std::endl;
				std::cin >> k;
			}
			else
			{
				char j[20];
			rep:
				std::cout << "Input boolean value (true or false) for" << TID[i].get_name() << std::endl;
				std::cin >> j;
				if (!strcmp(j, "true"))
					k = 1;
				else
					if (!strcmp(j, "false"))
						k = 0;
					else
					{
						std::cout << "Error in input:true/false" << std::endl;
						goto rep;
					}
			}
			TID[i].put_value(k);
			TID[i].put_assign();
			break;
		}
		case LEX_PLUS:
			args.push(args.pop() + args.pop());
			break;
		case LEX_TIMES:
			args.push(args.pop() * args.pop());
			break;
		case LEX_MINUS:
			i = args.pop();
			args.push(args.pop() - i);
			break;
		case LEX_SLASH:
			i = args.pop();
			if (!i)
			{
				args.push(args.pop() / i);
				break;
			}
			else
				throw "POLIZ:divide by zero";
		case LEX_EQ:
			args.push(args.pop() == args.pop());
			break;
		case LEX_LSS:
			i = args.pop();
			args.push(args.pop() < i);
			break;
		case LEX_GTR:
			i = args.pop();
			args.push(args.pop() > i);
			break;
		case LEX_LEQ:
			i = args.pop();
			args.push(args.pop() <= i);
			break;
		case LEX_GEQ:
			i = args.pop();
			args.push(args.pop() >= i);
			break;
		case LEX_NEQ:
			i = args.pop();
			args.push(args.pop() != i);
			break;
		case LEX_ASSIGN:
			i = args.pop();
			j = args.pop();
			TID[j].put_value(i);
			TID[j].put_assign(); break;
		default:
			throw "POLIZ: unexpected elem";
		}//end of switch
		++index;
	};//end of while
	std::cout << "Finish of executing!!!" << std::endl;
}

void Interpretator::interpretation()
{
	pars.analyze();
	E.execute(pars.prog);
}