#include "fsa.h"

DFSA::DFSA(unsigned int _initialState)
{
	initialState = indexOf(_initialState);
}

DFSA::transitionProxy::operator unsigned int() const
{
	assert(s->transitions.count(symbol) > 0);
	return s->a->labelOf(s->transitions.at(symbol));
}

DFSA::transitionProxy::transitionProxy(state* _s, char _symbol) : s(_s), symbol(_symbol) {}

void DFSA::transitionProxy::operator= (unsigned int label)
{
	s->a->addTransition(s->label, label, symbol);
}

DFSA::state& DFSA::operator[] (unsigned int label)
{
	return table[indexOf(label)];
}

const DFSA::state& DFSA::operator[] (unsigned int label) const
{
	return table[indexOf(label)];
}

DFSA::state::state(DFSA *_a) :a(_a), isFinal(false) {}

unsigned int DFSA::state::operator[] (char symbol) const
{
	return a->labelOf(transitions.at(symbol));
}

DFSA::transitionProxy DFSA::state::operator [] (char symbol)
{
	return transitionProxy(this, symbol);
}

bool DFSA::state::final() const
{
	return isFinal;
}

void DFSA::setFinalState(unsigned int s)
{
	unsigned ix = indexOf(s);
	table[ix].isFinal = true;
}

unsigned int DFSA::labelOf(unsigned int index) const
{
	assert(index < table.size());
	return table[index].label;
}

unsigned int DFSA::indexOf(unsigned int l) const
{
	assert(labelToIndex.count(l) != 0);
	return labelToIndex.at(l);
}

unsigned int DFSA::indexOf(unsigned int l)
{
	if (labelToIndex.count(l) == 0)
	{
		labelToIndex[l] = table.size();
		state newState(this);
		newState.label = l;
		table.push_back(newState);
	}

	return labelToIndex[l];
}

void DFSA::addTransition(unsigned int from, unsigned int to, char symbol)
{
	unsigned int indexFrom = indexOf(from);
	unsigned int indexTo = indexOf(to);
	table[indexFrom].transitions[symbol] = indexTo;
}

unsigned int DFSA::toState(unsigned int from, char s)
{
	return labelOf(table[from].transitions[s]);
}

bool DFSA::hasSymbol(unsigned int label, char s)
{
	if (labelToIndex.count(label) == 0)
		return false;

	return table[indexOf(label)].transitions.count(s) > 0;
}

DFSA::StatesIterator DFSA::begin() const
{
	return DFSA::StatesIterator(this);
}

DFSA::StatesIterator DFSA::end() const
{
	return DFSA::StatesIterator(this, true);
}

DFSA::StatesIterator::StatesIterator(DFSA const *_a, bool end) : a(_a)
{
	if (!end)
	{
		currentStateIndex = 0;
	}
	else
	{
		currentStateIndex = a->table.size();
	}
}

int DFSA::StatesIterator::operator*() const
{
	assert(currentStateIndex < a->table.size());
	return a->table[currentStateIndex].label;
}

DFSA::StatesIterator& DFSA::StatesIterator::operator++()
{
	assert(currentStateIndex < a->table.size());
	++currentStateIndex;
	return *this;
}

bool DFSA::StatesIterator::operator!=(const DFSA::StatesIterator& obj) const
{
	return this->a != obj.a || this->currentStateIndex != obj.currentStateIndex;
}

DFSA::SymbolIterator::SymbolIterator(state const *s, bool end)
{
	if (!end)
	{
		transitionIterator = s->transitions.begin();
	}
	else
	{
		transitionIterator = s->transitions.end();
	}
}

char DFSA::SymbolIterator::operator*() const
{
	return transitionIterator->first;
}

DFSA::SymbolIterator& DFSA::SymbolIterator::operator++()
{
	++transitionIterator;
	return *this;
}

bool DFSA::SymbolIterator::operator!=(const DFSA::SymbolIterator& obj) const
{
	return this->transitionIterator != obj.transitionIterator;
}

DFSA::SymbolIterator DFSA::state::begin() const
{
	return DFSA::SymbolIterator(this);
}

DFSA::SymbolIterator DFSA::state::end() const
{
	return DFSA::SymbolIterator(this, true);
}

bool DFSA::readsWord(const std::string& str, unsigned int fromState, unsigned int readSymbols = 0) const
{
	if (readSymbols == str.size() && (*this)[fromState].isFinal)
		return true;

	for (auto s : (*this)[fromState])
	{
		if (s == str[readSymbols])
			return readsWord(str, (*this)[fromState][s], ++readSymbols);
	}
	return false;
}