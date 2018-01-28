#ifndef __DFSA_
#define __DFSA_

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <cassert>
#include <utility>
#include <limits.h>

template<typename T>
class DFSA
{
public:
	class StatesIterator
	{
	public:
		StatesIterator(DFSA<T> const*, bool = false);
		int operator*() const;
		StatesIterator& operator++();
		bool operator!=(const StatesIterator&) const;

	private:
		DFSA<T> const *a;
		int currentStateIndex;
	};

public:
	struct state;
public:
	class SymbolIterator
	{
	public:
		SymbolIterator(state const*, bool = false);
		char operator*() const;
		SymbolIterator& operator++();
		bool operator!=(const SymbolIterator&) const;

	private:
		std::map<char, unsigned int>::const_iterator transitionIterator;
	};

private:
	struct transitionProxy
	{
		transitionProxy(state*, char);
		void operator=(T);
		operator unsigned int () const;
	private:
		state *s;
		char symbol;
	};

	struct state
	{
		friend class transitionProxy;
		friend class DFSA<T>;

		state(const state&);
		state& operator=(const state&);

		transitionProxy operator [] (char);
		T operator [](char) const;
		SymbolIterator begin() const;
		SymbolIterator end() const;
		bool final() const;

		T getLabel() const
		{
			return this->label;
		}

	private:
		state(DFSA<T>*);
		bool isFinal;
		T label;

		std::map<char, unsigned int> transitions;
		DFSA<T> *a;
	};

public:
	DFSA(T = T());
	DFSA(const DFSA<T>&);
	DFSA<T>& operator=(const DFSA<T>&);

	void setFinalState(T);
	void addTransition(T, T, char);

	state& operator[] (T);
	const state& operator[] (T) const;

	T toState(T, char);

	bool hasSymbol(T, char) const;

	StatesIterator begin() const;
	StatesIterator end() const;

	unsigned int getInitialState() const
	{
		return this->initialState;
	}

	bool readsWord(const std::string&, T, unsigned int = 0) const;
	DFSA<T> complement();
	void print() const;
	
	void setString(std::string& str)
	{
		this->currentString = str;
	}

	std::string getCurrentString() const
	{
		return this->currentString;
	}
private:
	std::vector<state> table;

	std::map<T, int> labelToIndex;
	T initialState;
	std::string currentString;

	unsigned int indexOf(T);
	unsigned int indexOf(T) const;
	T labelOf(unsigned int) const;

	void getAlphabet(T,std::vector<char>&) const;
};

#include "fsa.hpp"
#endif
