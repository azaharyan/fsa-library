#define SENTINEL INT_MAX

template<typename T = unsigned int>
DFSA<T>::DFSA(T _initialState) : initialState(_initialState)
{}

template<typename T>
DFSA<T>::DFSA(const DFSA<T>& obj) : DFSA(obj.initialState)
{
	this->table = obj.table;
	this->labelToIndex = obj.labelToIndex;
}

template<typename T>
DFSA<T>& DFSA<T>::operator=(const DFSA<T>& obj)
{
	if (this != &obj)
	{
		this->initialState = obj.initialState;
		this->labelToIndex = obj.labelToIndex;
		this->table = obj.table;
	}
	return *this;
}

template<typename T>
DFSA<T>::transitionProxy::operator unsigned int() const
{
	assert(s->transitions.count(symbol) > 0);
	return s->a->labelOf(s->transitions.at(symbol));
}

template<typename T>
DFSA<T>::transitionProxy::transitionProxy(state* _s, char _symbol) : s(_s), symbol(_symbol) {}

template<typename T>
void DFSA<T>::transitionProxy::operator= (T label)
{
	s->a->addTransition(s->label, label, symbol);
}

template<typename T>
typename DFSA<T>::state& DFSA<T>::operator[] (T label)
{
	return table[indexOf(label)];
}

template<typename T>
typename const DFSA<T>::state& DFSA<T>::operator[] (T label) const
{
	return table[indexOf(label)];
}

template<typename T>
DFSA<T>::state::state(DFSA *_a) :a(_a), isFinal(false) {}

template<typename T>
typename DFSA<T>::state& DFSA<T>::state::operator=(typename const DFSA<T>::state& obj)
{
	if (this != &obj)
	{
		this->a = obj.a;
		this->isFinal = obj.isFinal;
		this->label = obj.label;
		this->transitions = obj.transitions;
	}
	return *this;
}

template<typename T>
DFSA<T>::state::state(typename const DFSA<T>::state& obj) : DFSA<T>::state::state(obj.a)
{
	this->isFinal = obj.isFinal;
	this->label = obj.label;
	this->transitions = obj.transitions;
}

template<typename T>
T DFSA<T>::state::operator[] (char symbol) const
{
	return a->labelOf(transitions.at(symbol));
}

template<typename T>
typename DFSA<T>::transitionProxy DFSA<T>::state::operator [] (char symbol)
{
	return transitionProxy(this, symbol);
}

template<typename T>
bool DFSA<T>::state::final() const
{
	return isFinal;
}

template<typename T>
void DFSA<T>::setFinalState(T s)
{
	unsigned int ix = indexOf(s);
	table[ix].isFinal = true;
}

template<typename T>
T DFSA<T>::labelOf(unsigned int index) const
{
	assert(index < table.size());
	return table[index].label;
}

template<typename T>
unsigned int DFSA<T>::indexOf(T l) const
{
	assert(labelToIndex.count(l) != 0);
	return labelToIndex.at(l);
}

template<typename T>
unsigned int DFSA<T>::indexOf(T l)
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

template<typename T>
void DFSA<T>::addTransition(T from, T to, char symbol)
{
	unsigned int indexFrom = indexOf(from);
	unsigned int indexTo = indexOf(to);
	table[indexFrom].transitions[symbol] = indexTo;
}

template<typename T>
T DFSA<T>::toState(T from, char s)
{
	return labelOf(table[indexOf(from)].transitions[s]);
}

//template<typename T>
//bool DFSA<T>::hasTransition(T from, T to, char a) const
//{
//	if()
//}

template<typename T>
bool DFSA<T>::hasSymbol(T label, char s) const
{
	if (labelToIndex.count(label) == 0)
		return false;

	return table[indexOf(label)].transitions.count(s) > 0;
}

template<typename T>
typename DFSA<T>::StatesIterator DFSA<T>::begin() const
{
	return DFSA::StatesIterator(this);
}

template<typename T>
typename DFSA<T>::StatesIterator DFSA<T>::end() const
{
	return DFSA::StatesIterator(this, true);
}

template<typename T>
DFSA<T>::StatesIterator::StatesIterator(DFSA<T> const *_a, bool end) : a(_a)
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

template<typename T>
int DFSA<T>::StatesIterator::operator*() const
{
	assert(currentStateIndex < a->table.size());
	return a->table[currentStateIndex].label;
}

template<typename T>
typename DFSA<T>::StatesIterator& DFSA<T>::StatesIterator::operator++()
{
	assert(currentStateIndex < a->table.size());
	++currentStateIndex;
	return *this;
}

template<typename T>
bool DFSA<T>::StatesIterator::operator!= (typename const DFSA<T>::StatesIterator& obj) const
{
	return this->a != obj.a || this->currentStateIndex != obj.currentStateIndex;
}

template<typename T>
DFSA<T>::SymbolIterator::SymbolIterator(state const *s, bool end)
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

template<typename T>
char DFSA<T>::SymbolIterator::operator*() const
{
	return transitionIterator->first;
}

template<typename T>
typename DFSA<T>::SymbolIterator& DFSA<T>::SymbolIterator::operator++()
{
	++transitionIterator;
	return *this;
}

template<typename T>
bool DFSA<T>::SymbolIterator::operator!=(const typename DFSA<T>::SymbolIterator& obj) const
{
	return this->transitionIterator != obj.transitionIterator;
}

template<typename T>
typename DFSA<T>::SymbolIterator DFSA<T>::state::begin() const
{
	return DFSA<T>::SymbolIterator(this);
}

template<typename T>
typename DFSA<T>::SymbolIterator DFSA<T>::state::end() const
{
	return DFSA<T>::SymbolIterator(this, true);
}

/*
	Checks if the given string belongs to the language of the automaton
	a.k.a if the automaton recognizes the gives string.
*/
template<typename T>
bool DFSA<T>::readsWord(const std::string& str, T fromState, unsigned int readSymbols = 0) const
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

template<typename T>
using TPair = std::pair<T, T>;

template<typename T>
DFSA<TPair<T>> uniteAutomata(const DFSA<T>& a1, const DFSA<T>& a2)
{
	auto initState1 = a1.getInitialState();
	auto initState2 = a2.getInitialState();
	DFSA<TPair<T>> united(TPair<T>(initState1, initState2));

	uniteAutomataHelper(a1, a2, united,initState1, initState2);

	DFSA<TPair<T>> united2 = united;
	return united2;
}

/*
	Recursively generates a new automaton with states that are pairs of the states of the two original automata.
	The method for generating the automaton is the same for the operations union and intersect.
	The only difference is in the set of final states.
*/
template<typename T>
void uniteAutomataHelper(const DFSA<T>& a1, const DFSA<T>& a2, DFSA<TPair<T>>& a, T initState1, T initState2, bool isUnion = 1)
{
	for (auto s : a1[initState1])
	{
		if (a2.hasSymbol(initState2, s))
		{
			TPair<T> state(initState1,initState2);
			if (!a.hasSymbol(state, s))
			{
				a[state][s] = TPair<T>(a1[initState1][s], a2[initState2][s]);

				if ( (isUnion == 1 && a1[initState1].final() && a2[initState2].final()) ||
					(isUnion == 0 && (a1[initState1].final() || a2[initState2].final())) )
					a.setFinalState(TPair<T>(initState1, initState2));

				uniteAutomataHelper(a1, a2, a, a1[initState1][s], a2[initState2][s]);
			}
		}

	}
}

/*
	Return the alphabet of the automaton
*/
template<typename T>
void DFSA<T>::getAlphabet(T initial, std::vector<char>& alphabet) const
{
	for (auto s : (*this)[initial])
	{
		if (std::find(alphabet.begin(), alphabet.end(), s) == alphabet.end())
		{
			alphabet.push_back(s);
			getAlphabet((*this)[initial][s], alphabet);
		}
	}
}

/*
	Prints the initial state, the alphabet,
	the set of states and the final state
	of the automaton
*/
template<typename T>
void DFSA<T>::print() const
{
	std::cout << "=== M ====\n";
	std::cout << "q0 = " << this->initialState << std::endl;
	std::cout << "\u03A3 = {";
	std::vector<char> alphabet;
	std::vector<T> finalStates;
	getAlphabet(this->initialState, alphabet);
	for (auto s : alphabet)
	{
		std::cout << s << " ";
	}
	std::cout << "}\n";

	std::cout << "Q = {";
	for (auto& st : this->table)
	{
		if (st.isFinal)
			finalStates.push_back(st.getLabel());
		std::cout << st.getLabel() << " ";
	}
	std::cout << "}\n F = { ";
	for (auto& s : finalStates)
	{
		std::cout << s << " ";
	}
	std::cout << "} \n";
}

/*
	Return the complement automaton of the current
	The complement automaton is the original with reversed final states
*/
template<typename T>
DFSA<T> DFSA<T>::complement()
{
	DFSA<T> complemented = (*this);

	for (auto& s : complemented.table)
	{
		if (!s.isFinal)
			s.isFinal = true;
		else
			s.isFinal = false;
	}
	return complemented;
}

/*
	Consumes a string and set it as the currentString for the automaton
*/
template<typename T>
std::istream& operator >> (std::istream& is, DFSA<T>& a)
{
	std::string str;
	is >> str;
	a.setString(str);
	return is;
}
