#ifndef __DFSA_
#define __DFSA_

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <cassert>

class DFSA
{
public:
	class StatesIterator
	{
	public:
		StatesIterator(DFSA const*, bool = false);
		int operator*() const;
		StatesIterator& operator++();
		bool operator!=(const StatesIterator&) const;

	private:
		DFSA const *a;
		int currentStateIndex;
	};

private:
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
		void operator=(unsigned int);
		operator unsigned int () const;
	private:
		state *s;
		char symbol;
	};

	struct state
	{
		friend class transitionProxy;
		friend class DFSA;
		transitionProxy operator [] (char);
		unsigned int operator [](char) const;
		SymbolIterator begin() const;
		SymbolIterator end() const;
		bool final() const;

	private:
		state (DFSA*);
		bool isFinal;
		int label;

		std::map<char, unsigned int> transitions;
		DFSA *a;
	};

public:

	DFSA(unsigned int = 0);

	void setFinalState(unsigned int);
	void addTransition(unsigned int, unsigned int, char);

	state& operator[] (unsigned int);
	const state& operator[] (unsigned int) const;

	unsigned int toState(unsigned int, char);

	bool hasSymbol(unsigned int, char);

	StatesIterator begin() const;
	StatesIterator end() const;

	bool readsWord(const std::string&, unsigned int, unsigned int) const;
private:
	std::vector<state> table;

	std::map<int, int> labelToIndex;
	unsigned int initialState;

	unsigned int indexOf(unsigned int);
	unsigned int indexOf(unsigned int) const;
	unsigned int labelOf(unsigned int) const;
};

#endif