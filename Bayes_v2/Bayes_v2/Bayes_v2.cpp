#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <ranges>
#include <vector>

#define DEBUG true

using namespace std;

/**
 * \brief Klasa prawdopodobieñstw warunkowych
 */
class CondProbabilities
{
public:
	/**
	 * \brief Konstruktor
	 * \param no_states Liczba stanów
	 */
	CondProbabilities(unsigned int no_states)
	{
		if (no_states < 2) no_states = 2;
		FillMap(no_states, {});
	}

	/**
	 * \brief Konstruktor
	 * \param no_states Liczba stanów
	 * \param parents_states Stany rodziców
	 */
	CondProbabilities(unsigned int no_states, const vector<unsigned int>& parents_states)
	{
		if (no_states < 2) no_states = 2;
		FillMap(no_states, parents_states);
		parentTotalStates = parents_states;
	}

	/**
	 * \brief Destruktor
	 */
	~CondProbabilities() = default;

	/**
	 * \brief Dodanie nowej zmiennej warunkowej
	 * \param total_states Liczba stanów
	 */
	void AddVariable(unsigned int total_states = 2)
	{
		if (total_states < 2) total_states = 2;
		parentTotalStates.push_back(total_states);

		const unsigned int tot_columns = GetColumnns();

		conditionalMap.clear();

		FillMap(tot_columns, parentTotalStates);
	}

	/**
	 * \brief Zwracanie liczby kolumn mapy prawdopodobieñstw
	 * \return Liczba kolumn mapy prawdopodobieñstw
	 */
	unsigned int GetColumnns()
	{
		if (conditionalMap.empty()) return 0;
		return conditionalMap.begin()->second.size();
	}

	/**
	 * \brief Ustawianie prawdopodobieñstwa
	 * \param parents_states Stany rodziców
	 * \param probabilities Wektor prawdopodobieñstw
	 */
	void SetProbabilities(const vector<unsigned int>& parents_states, const vector<double>& probabilities)
	{
		if (conditionalMap.contains(parents_states))
		{
			conditionalMap[parents_states] = probabilities;
		}
	}

	/**
	 * \brief Pobranie prawdopodobieñstwa
	 * \param variableState Stan zmiennej
	 * \param parents_states Stany rodziców
	 * \return Prawdopodobieñstwo
	 */
	double GetProbability(unsigned int variableState, const vector<unsigned int>& parents_states)
	{
		const auto row_vector = conditionalMap[parents_states];
		return row_vector.at(variableState);
	}

	/**
	 * \brief Mapa prawdopodobiañstw warunkowych
	 */
	map<vector<unsigned int>, vector<double> > conditionalMap;
private:
	/**
	 * \brief Liczba stanów rodziców
	 */
	vector<unsigned int> parentTotalStates;

	/**
	 * \brief Wype³nienie mapy prawdopodobieñstw
	 * \param nodeStatesNumber Liczba stanów
	 * \param parentsStates Liczby stanów rodziców
	 */
	void FillMap(unsigned int nodeStatesNumber, vector<unsigned int> parentsStates)
	{
		//0- In case of a root node without parents, it adds only one row
		if (parentsStates.empty())
		{
			vector<double> data_vector;
			vector<unsigned int> key_vector;
			for (unsigned int i = 0; i < nodeStatesNumber; i++)
			{
				data_vector.push_back(1.0 / static_cast<double>(nodeStatesNumber));
			}
			conditionalMap.insert({ key_vector, data_vector });
			return;
		}

		//1- Creation of the vector that contains the Subsets of unsigned int
		vector<vector<unsigned int>> vector_container;
		for (auto it = parentsStates.begin(); it != parentsStates.end(); ++it)
		{
			vector<unsigned int> temp_vector;
			unsigned int total_states = (*it);
			for (unsigned int state_counter = 0; state_counter < total_states; state_counter++)
			{
				temp_vector.push_back(state_counter);
			}
			vector_container.push_back(temp_vector);
		}

		//2- Creation of the vector that contains the Iterators
		vector< vector<unsigned int>::iterator > iterator_container;
		for (auto it = vector_container.begin(); it != vector_container.end(); ++it)
		{
			vector<unsigned int>::iterator temp_iterator;
			temp_iterator = (*it).begin();
			iterator_container.push_back(temp_iterator);
		}

		//3- filling the data_vector
		vector<double> data_vector;
		data_vector.reserve(nodeStatesNumber);
		for (unsigned int p_counter = 0; p_counter < nodeStatesNumber; p_counter++)
		{
			data_vector.push_back((double)1.0 / (double)nodeStatesNumber);
		}

		//4- Cascade iteration for storing the whole set of combination
		unsigned int K = iterator_container.size();

		while (iterator_container[0] != vector_container[0].end())
		{

			//Clear the key_vector before pushing a new key
			vector<unsigned int> key_vector;
			key_vector.reserve(parentsStates.size());

			//Filling the key_vector 
			for (auto it_key = iterator_container.begin(); it_key != iterator_container.end(); ++it_key)
			{
				key_vector.push_back(**it_key); //pushing a new key
			}
			
			conditionalMap.insert({ key_vector, data_vector });

			//Move to next value
			++iterator_container[K - 1];
			for (int i = K - 1; (i > 0) && (iterator_container[i] == vector_container[i].end()); --i)
			{
				//subtracting the counter
				iterator_container[i] = vector_container[i].begin();
				++iterator_container[i - 1];
			}
		}
	}
};

/**
 * \brief Wêze³ sieci Bayesowskiej
 */
class BNode
{
public:
	/**
	 * \brief Liczba stanów
	 */
	int no_states;

	/**
	 * \brief Lista s¹siedztwa
	 */
	list<unsigned int> adj;

	/**
	 * \brief Prawdopodobieñstwa warunkowe
	 */
	CondProbabilities conds;

	/**
	 * \brief Sprawdzenie czy wêze³ jest w liœcie s¹siedztwa
	 * \param index Indeks wêz³a
	 * \return Prawda, jeœli wêze³ jest w liœcie s¹siedztwa
	 */
	bool IsInAdjacencyList(unsigned int index)
	{
		for (auto it = adj.begin(); it != adj.end(); ++it)
		{
			if (*it == index) return  true;
		}
		return false;
	}

	/**
	 * \brief Konstruktor
	 * \param no_states Liczba stanów
	 */
	BNode(unsigned int no_states) : no_states(no_states), conds(no_states)
	{}
};

/**
 * \brief Sieæ Bayesowska
 */
class Bayes
{
public:
	/**
	 * \brief Konstruktor
	 * \param no_states Liczba stanów
	 */
	Bayes(const vector<unsigned int>& no_states)
	{
		nodes.reserve(no_states.size());
		for (auto it = no_states.begin(); it != no_states.end(); ++it)
		{
			BNode n(*it);
			nodes.push_back(n);
		}
	}

	/**
	 * \brief Dodanie krawêdzi
	 * \param src Indeks wêz³a Ÿród³owego
	 * \param dest Indeks wêz³a docelowego
	 */
	void AddEdge(const unsigned int src, const unsigned int dest)
	{
		if (src > nodes.size() || dest > nodes.size())
		{
			throw invalid_argument("Invalid node index");
		}
		if (src == dest) return;
		const unsigned int first_tot_states = nodes[src].no_states;
		nodes[src].adj.push_back(dest);
		nodes[dest].conds.AddVariable(first_tot_states);
	}

	/**
	 * \brief Operator indeksowania
	 * \param index Indeks wêz³a
	 * \return Wêze³ o podanym indeksie
	 */
	BNode& operator[](const unsigned int index)
	{
		if (index >= nodes.size()) throw invalid_argument("Invalid node index");
		return nodes[index];
	}

	/**
	 * \brief Pobieranie wêz³ów powi¹zanych z wêz³em krawêdziami wejœciowymi
	 * \param index Indeks wêz³a
	 * \return Lista wêz³ów wejœciowych
	 */
	list<unsigned int> ReturnInEdges(unsigned int index)
	{
		list<unsigned int> temp_list;
		unsigned int counter = 0;
		for (auto it = nodes.begin(); it != nodes.end(); ++it)
		{
			if ((*it).IsInAdjacencyList(index) == true) temp_list.push_back(counter);
			counter++;
		}
		return temp_list;
	}

	/**
	 * \brief Pobieranie wêz³ów powi¹zanych z wêz³em krawêdziami wyjœciowymi
	 * \param index Indeks wêz³a
	 * \return Lista wêz³ów wyjœciowych
	 */
	list<unsigned int> ReturnOutEdges(unsigned int index) const
	{
		list<unsigned int> temp_list = nodes[index].adj;
		return temp_list;
	}

	/**
	 * \brief Sprawdzenie czy wêze³ zosta³ odwiedzony
	 * \param index Indeks wêz³a
	 * \param closed Mapa wêz³ów odwiedzonych
	 * \return Prawda, jeœli wêze³ zosta³ odwiedzony
	 */
	bool isVisited(pair<unsigned, unsigned> index, map<pair<unsigned, unsigned>, bool>* closed)
	{
		return closed->contains(index);
	}

	/**
	 * \brief Przechodzenie po wêz³ach w g³¹b
	 * \param node Wêze³
	 * \param v Wektor wêz³ów
	 * \param closed Mapa wêz³ów odwiedzonych
	 * \param probability Prawdopodobieñstwo
	 * \return Prawdopodobieñstwo
	 */
	double traverse(pair<unsigned, unsigned>& node, vector<pair<unsigned, unsigned>>& v, map<pair<unsigned, unsigned>, bool>* closed, double probability)
	{
		list<unsigned> parents = ReturnInEdges(node.first);
		if (parents.empty())
		{
			double prob = nodes[node.first].conds.GetProbability(node.second, {});
			closed->insert({ {node}, true });
			if constexpr (DEBUG)
				cout << prob << endl;
			return prob;
		}
		vector<pair<unsigned, unsigned>> parents_vector;
		for (auto& p : parents)
		{
			auto nest = ranges::find(v, p, &pair<unsigned, unsigned>::first);
			if (nest != v.end())
			{
				if (isVisited(*nest, closed))
				{
					double prob = nodes[node.first].conds.conditionalMap[{nest->second}][node.second];
					closed->insert({ { node }, true });
					parents_vector.push_back(*nest);
				}
				else
					probability *= traverse(*nest, v, closed, probability);
			}
		}
		if (!parents_vector.empty())
		{
			vector<unsigned> values;
			ranges::transform(parents_vector,
				back_inserter(values),
				[](auto const& pair) { return pair.second; });
			double prob = nodes[node.first].conds.conditionalMap[values][node.second];
			if constexpr (DEBUG)
				cout << prob << endl;
			return prob;
		}
		else
		{
			vector<unsigned> values;
			for (auto& p : parents)
			{
				values.push_back(ranges::find(v, p, &pair<unsigned, unsigned>::first)->second);
			}
			closed->insert({ { node }, true });
			double prob = nodes[node.first].conds.conditionalMap[values][node.second];
			if constexpr (DEBUG)
				cout << prob << endl;
			probability *= prob;
		}
		return probability;
	}

	/**
	 * \brief Funkcja wywo³uj¹ca przechodzenie po wêz³ach w g³¹b
	 * \param v Wektor wêz³ów
	 * \return Mapa œcie¿ki na prawdopodobieñstwo
	 */
	map<vector<pair<unsigned, unsigned>>, double> CalculateProbability(const vector<pair<unsigned, unsigned>>& v)
	{
		vector<vector<pair<unsigned, unsigned>>> paths = GeneratePaths(v);
		vector<double> probabilities(paths.size(), 1);
		map<vector<pair<unsigned, unsigned>>, double> probMap;
		const list<unsigned> leaves = ReturnLeafList();

		for (int i = 0; i < paths.size(); ++i)
		{
			map<pair<unsigned, unsigned>, bool> closed;
			for (auto& leaf : leaves)
			{
				auto n = ranges::find(paths[i], leaf, &pair<unsigned, unsigned>::first);
				if (n != paths[i].end())
					probabilities[i] *= traverse(*n, paths[i], &closed, probabilities[i]);
			}
			probMap.insert({ paths[i], probabilities[i] });
		}
		return probMap;
	}

	/**
	 * \brief Generowanie œcie¿ek dla przechodzenia po wêz³ach w g³¹b
	 * \param v Wektor wêz³ów
	 * \return Wektor œcie¿ek
	 */
	vector<vector<pair<unsigned, unsigned>>> GeneratePaths(vector<pair<unsigned, unsigned>> v)
	{

		vector<pair<unsigned, unsigned>> path;
		vector<pair<unsigned, unsigned>> diff;

		if (v.size() == nodes.size())
			return { v };

		for (int i = 0; i < nodes.size(); ++i)
		{
			if (auto tmp = ranges::find(v, i, &pair<unsigned, unsigned>::first); tmp == v.end())
			{
				diff.emplace_back(i, nodes[i].no_states);
			}
		}

		int its = 1;
		for (const auto& val : diff | views::values)
		{
			its *= val;
		}

		vector paths(its, v);

		for (size_t i = 0; i < its; i++)
		{
			auto temp = i;
			for (const auto& [fst, snd] : diff)
			{
				auto index = temp % snd;
				temp /= snd;
				paths[i].emplace_back(fst, index);
			}
		}

		return paths;
	}

	/**
	 * \brief Sprawdzenie, czy wêze³ jest korzeniem
	 * \param index Indeks wêz³a
	 * \return Prawda, jeœli wêze³ jest korzeniem
	 */
	bool IsRoot(const unsigned int index)
	{
		const auto out_list = ReturnOutEdges(index);
		if (const auto in_list = ReturnInEdges(index); in_list.empty() && !out_list.empty()) return true;
		else return false;
	}

	/**
	 * \brief Sprawdznie, czy wêze³ jest liœciem
	 * \param index Indeks wêz³a
	 * \return Prawda, jeœli wêze³ jest liœciem
	 */
	bool IsLeaf(const unsigned int index)
	{
		const auto out_list = ReturnOutEdges(index);
		if (const auto in_list = ReturnInEdges(index); !in_list.empty() && out_list.empty()) return true;
		else return false;
	}

	/**
	 * \brief Zwrócenie listy korzeni
	 * \return Lista korzeni
	 */
	list<unsigned int> ReturnRootList()
	{
		list<unsigned int> list_to_return;
		for (unsigned int i = 0; i < nodes.size(); i++)
		{
			if (IsRoot(i) == true) list_to_return.push_back(i);
		}
		return list_to_return;
	}

	/**
	 * \brief Zwrócenie listy liœci
	 * \return Lista liœci
	 */
	list<unsigned int> ReturnLeafList()
	{
		list<unsigned int> list_to_return;
		for (unsigned int i = 0; i < nodes.size(); i++)
		{
			if (IsLeaf(i) == true) list_to_return.push_back(i);
		}
		return list_to_return;
	}

	/**
	 * \brief Obliczenie wspólnego rozk³adu prawdopodobieñstwa
	 * \param v Wektor wêz³ów
	 * \return Prawdopodobieñstwo
	 */
	double GetJointProbability(const vector<pair<unsigned, unsigned>>& v)
	{
		const auto probs = CalculateProbability(v);
		if (v.size() == nodes.size())
		{
			return probs.begin()->second;
		}

		vector<double> values;
		ranges::transform(probs,
		                  back_inserter(values),
		                  [](const map<vector<pair<unsigned, unsigned>>, double>::value_type &val) { return val.second; });

		if constexpr (DEBUG)
		{
			cout << endl << "Probabilities: " << endl;
			for (const auto& val : values)
			{
				cout << val << endl;
			}
		}

		const double sum = accumulate(values.begin(), values.end(), 0.0);
		return sum;
	}

	/**
	 * \brief Obliczenie prawdopodobieñstwa Bayesa
	 * \param p Wektor wêz³ów do przewidzenia
	 * \param v Wektor wêz³ów
	 * \return Prawdopodobieñstwo
	 */
	double PredictProbability(const vector<pair<unsigned, unsigned>>& p, const vector<pair<unsigned, unsigned>>& v)
	{
		vector<pair<unsigned, unsigned>> sum;
		sum.reserve(p.size() + v.size());
		sum.insert(sum.end(), p.begin(), p.end());
		sum.insert(sum.end(), v.begin(), v.end());

		const auto up = CalculateProbability(sum);
		const auto down = CalculateProbability(v);

		vector<double> nominator;
		vector<double> denominator;
		
		ranges::transform(up,
			back_inserter(nominator),
			[](const map<vector<pair<unsigned, unsigned>>, double>::value_type& val1) { return val1.second; });

		ranges::transform(down,
			back_inserter(denominator),
			[](const map<vector<pair<unsigned, unsigned>>, double>::value_type& val2) { return val2.second; });

		if constexpr (DEBUG)
		{
			cout << endl << "Nominator: " << endl;
			for (const auto& val : nominator)
			{
				cout << val << endl;
			}

			cout << endl << "Denominator: " << endl;
			for (const auto& val : denominator)
			{
				cout << val << endl;
			}
		}
		
		const double s1 = accumulate(nominator.begin(), nominator.end(), 0.0);
		const double s2 = accumulate(denominator.begin(), denominator.end(), 0.0);
		return s1 / s2;
	}

private:
	/**
	 * \brief Wektor wêz³ów
	 */
	vector<BNode> nodes;
};


int main()
{
	// Ex1

	enum node
	{
		RAIN = 0,
		SPRINKLER = 1,
		GRASS_WET = 2
	};

	Bayes net({ 2, 2, 2 });

	net.AddEdge(RAIN, SPRINKLER);
	net.AddEdge(RAIN, GRASS_WET);
	net.AddEdge(SPRINKLER, GRASS_WET);

	net[RAIN].conds.SetProbabilities({}, { 0.8, 0.2 });

	net[SPRINKLER].conds.SetProbabilities({ false }, { 0.6, 0.4 });
	net[SPRINKLER].conds.SetProbabilities({ true }, { 0.99, 0.01 });

	net[GRASS_WET].conds.SetProbabilities({ false, false }, { 1.0, 0.0 });
	net[GRASS_WET].conds.SetProbabilities({ false, true }, { 0.1, 0.9 });
	net[GRASS_WET].conds.SetProbabilities({ true, false }, { 0.2, 0.8 });
	net[GRASS_WET].conds.SetProbabilities({ true, true }, { 0.01, 0.99 });
	

	//double out = net.GetJointProbability({{GRASS_WET, true}, {SPRINKLER, false}, {RAIN, true} });
	double out = net.PredictProbability({ {RAIN, true} }, { {GRASS_WET, true} });
	cout << endl << out << endl;

	// Ex2
	//
	//enum node
	//{
	//	BURGLARY,
	//	EARTHQUAKE,
	//	ALARM,
	//	JOHN,
	//	MARY
	//};

	//Bayes net({ 2, 2, 2, 2, 2 });

	//net.AddEdge(BURGLARY, ALARM);
	//net.AddEdge(EARTHQUAKE, ALARM);
	//net.AddEdge(ALARM, JOHN);
	//net.AddEdge(ALARM, MARY);

	//net[BURGLARY].conds.SetProbabilities({}, { 0.999, 0.001 });
	//net[EARTHQUAKE].conds.SetProbabilities({}, { 0.998, 0.002 });

	//net[ALARM].conds.SetProbabilities({ false, false }, { 0.999, 0.001 });
	//net[ALARM].conds.SetProbabilities({ false, true }, { 0.06, 0.94 });
	//net[ALARM].conds.SetProbabilities({ true, false }, { 0.71, 0.29 });
	//net[ALARM].conds.SetProbabilities({ true, true }, { 0.05, 0.95 });

	//net[JOHN].conds.SetProbabilities({ false }, { 0.95, 0.05 });
	//net[JOHN].conds.SetProbabilities({ true }, { 0.10, 0.90 });

	//net[MARY].conds.SetProbabilities({ false }, { 0.99, 0.01 });
	//net[MARY].conds.SetProbabilities({ true }, { 0.30, 0.70 });


	////double out = net.GetJointProbability({{JOHN, true}, {MARY, true}, {ALARM, true}, {EARTHQUAKE, false}, {BURGLARY, false} });
	//double out = net.GetJointProbability({ {JOHN, true} });
	//cout << out << endl;

	// Ex3

	//enum node
	//{
	//	INCOME,
	//	DEPOSIT,
	//	PAYMENT,
	//	SECURITY,
	//	HOUSING
	//};

	//Bayes net({ 2, 2, 2, 2, 2 });

	//net.AddEdge(INCOME, DEPOSIT);
	//net.AddEdge(INCOME, PAYMENT);
	//net.AddEdge(DEPOSIT, PAYMENT);
	//net.AddEdge(PAYMENT, SECURITY);
	//net.AddEdge(HOUSING, SECURITY);

	//net[INCOME].conds.SetProbabilities({}, { 0.7, 0.3 });
	//net[HOUSING].conds.SetProbabilities({}, { 0.65, 0.35 });

	//net[DEPOSIT].conds.SetProbabilities({ false }, { 0.4, 0.6 });
	//net[DEPOSIT].conds.SetProbabilities({ true }, { 0.9, 0.1 });

	//net[PAYMENT].conds.SetProbabilities({ false, false }, { 0.4, 0.6 });
	//net[PAYMENT].conds.SetProbabilities({ false, true }, { 0.55, 0.45 });
	//net[PAYMENT].conds.SetProbabilities({ true, false }, { 0.5, 0.5 });
	//net[PAYMENT].conds.SetProbabilities({ true, true }, { 0.95, 0.05 });

	//net[SECURITY].conds.SetProbabilities({ false, false }, { 0.69, 0.31 });
	//net[SECURITY].conds.SetProbabilities({ false, true }, { 0.25, 0.75 });
	//net[SECURITY].conds.SetProbabilities({ true, false }, { 0.5, 0.5 });
	//net[SECURITY].conds.SetProbabilities({ true, true }, { 0.99, 0.01 });

	////double out = net.GetJointProbability({ {INCOME, true}, {DEPOSIT, false}, { SECURITY, false }, {HOUSING, true} });
	//double out = net.PredictProbability({ {PAYMENT, false} }, { { INCOME, true }, { DEPOSIT, false }, { SECURITY, false }, { HOUSING, true } });
	//cout << out << endl;

	// Ex 4

	//enum node
	//{
	//	WEATHER,
	//	FREE_TIME,
	//	HUMOUR,
	//	OUTDOORS,
	//	INDOORS
	//};

	//enum weather
	//{
	//	SUNNY,
	//	CLOUDY,
	//	RAINY,
	//	WINDY
	//};

	//enum humour
	//{
	//	EXTATIC,
	//	GOOD,
	//	SAD
	//};

	//enum outdoors
	//{
	//	WALK,
	//	POOL,
	//	BIKE
	//};

	//enum indoors
	//{
	//	COMPUTER,
	//	BOOK,
	//	COOK
	//};

	//Bayes net({ 4, 2, 3, 3, 3 });

	//net.AddEdge(WEATHER, HUMOUR);
	//net.AddEdge(FREE_TIME, HUMOUR);
	//net.AddEdge(HUMOUR, OUTDOORS);
	//net.AddEdge(HUMOUR, INDOORS);

	//net[WEATHER].conds.SetProbabilities({}, { 0.25, 0.25, 0.25, 0.25 });

	//net[FREE_TIME].conds.SetProbabilities({}, { 0.4, 0.6 });
	//
	//net[HUMOUR].conds.SetProbabilities({ SUNNY, false }, { 0.3, 0.3, 0.4 });
	//net[HUMOUR].conds.SetProbabilities({ SUNNY, true }, { 0.2, 0.4, 0.4 });
	//net[HUMOUR].conds.SetProbabilities({ CLOUDY, false }, { 0.1, 0.3, 0.6 });
	//net[HUMOUR].conds.SetProbabilities({ CLOUDY, true }, { 0.05, 0.35, 0.6 });
	//net[HUMOUR].conds.SetProbabilities({ RAINY, false }, { 0.4, 0.4, 0.2 });
	//net[HUMOUR].conds.SetProbabilities({ RAINY, true }, { 0.2, 0.5, 0.3 });
	//net[HUMOUR].conds.SetProbabilities({ WINDY, false }, { 0.6, 0.35, 0.05 });
	//net[HUMOUR].conds.SetProbabilities({ WINDY, true }, { 0.3, 0.4, 0.3 });

	//net[OUTDOORS].conds.SetProbabilities({ EXTATIC }, { 0.1, 0.2, 0.7 });
	//net[OUTDOORS].conds.SetProbabilities({ GOOD }, { 0.2, 0.6, 0.2 });
	//net[OUTDOORS].conds.SetProbabilities({ SAD }, { 0.5, 0.4, 0.1 });
	//
	//net[INDOORS].conds.SetProbabilities({ EXTATIC }, { 0.1, 0.3, 0.6 });
	//net[INDOORS].conds.SetProbabilities({ GOOD }, { 0.7, 0.2, 0.1 });
	//net[INDOORS].conds.SetProbabilities({ SAD }, { 0.3, 0.4, 0.3 });

	////double out = net.GetJointProbability({ {WEATHER, WINDY}, {FREE_TIME, false}, {OUTDOORS, WALK}, {INDOORS, BOOK}, {HUMOUR, SAD } });
	////double out = net.PredictProbability({{HUMOUR, EXTATIC}}, {{WEATHER, SUNNY}, {FREE_TIME, false}});
	//double out = net.GetJointProbability({ {WEATHER, SUNNY} });
	//cout << endl << out << endl;
}
