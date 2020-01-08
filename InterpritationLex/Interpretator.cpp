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

void Interpretator::interpretation()
{
	pars.analyze();
	E.execute(pars.prog);
}
