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

#pragma once
class Interpretator
{
	Parser   pars;
	Executer E;
public:
	Interpretator(char* program) : pars(program) {}
	void     interpretation();
};

