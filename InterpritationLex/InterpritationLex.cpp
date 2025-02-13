// InterpritationLex.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "Stack.h"
#include "AnalizerLex.h"
#include "ParserLex.h"
#include "Executer.h"
#include "Interpretator.h"

int main()
{
	try
	{
		Interpretator I((char*)"program.txt");
		I.interpretation();
		std::cout << std::endl;
		std::cin.get();
		return 0;
	}
	catch (char c)
	{
		std::cout << "unexpected symbol " << c << std::endl;
		std::cin.get();
		return 1;
	}
	catch (Lex l)
	{
		std::cout << "unexpected lexeme";
		std::cout << l;
		std::cin.get();
		return 1;
	}
	catch (const char *source)
	{
		std::cout << source << std::endl;
		std::cin.get();
		return 1;
	}
	std::cout << std::endl;
	std::cin.get();
	return 0;
}

