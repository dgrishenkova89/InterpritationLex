#include "stdafx.h"
#include <iostream>

#pragma once
template < class T, int max_size >
class Stack
{
	T            s[max_size];
	int          top;
public:
	Stack() { top = 0; }
	void         reset() { top = 0; }
	void         push(T i);
	T            pop();
	bool         is_empty() { return top == 0; }
	bool         is_full() { return top == max_size; }
};

template < class T, int max_size >
void Stack < T, max_size > ::push(T i)
{
	if (!is_full())
		s[top++] = i;
	else
		throw "Stack_is_full";
}

template <class T, int max_size >
T Stack < T, max_size > ::pop()
{
	if (!is_empty())
		return s[--top];
	else
		throw "stack_is_empty";
}

