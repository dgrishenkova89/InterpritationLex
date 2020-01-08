#include "stdafx.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "Stack.h"
#include "AnalizerLex.h"

#pragma once
class Executer
{
	Lex   pc_el;
public:
	void  execute(Poliz & prog);
};

