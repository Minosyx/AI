#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <numeric>
#include <ranges>
#include <random>
#include <set>

#define DEBUG false

class Node;
using namespace std;

/**
 * \brief KrawêdŸ grafu
 */
class Edge
{
public:
	/**
	 * \brief Dystans
	 */
	double distance;

	/**
	 * \brief Wêze³ Ÿród³owy
	 */
	Node* source;

	/**
	 * \brief Wêze³ docelowy
	 */
	Node* destination;
};

/**
 * \brief Wêze³ grafu
 */
class Node
{
public:
	/**
	 * \brief Etykieta
	 */
	string label;

	/**
	 * \brief Wektor krawêdzi
	 */
	vector<Edge*> edges;

	~Node()
	{
		for (const auto e : edges)
			delete e;
	}

	/**
	 * \brief Sprawdzenie czy wêze³ jest w liœcie s¹siedztwa
	 * \param node Wêze³
	 * \return Prawda, jeœli wêze³ jest w liœcie s¹siedztwa
	 */
	bool IsInAdjacencyList(Node* node)
	{
		return ranges::any_of(edges.begin(), edges.end(), [&](const Edge* edge) { return edge->destination == node; });
	}

	/**
	 * \brief Zwraca krawêdŸ do wêz³a
	 * \param dest Wêze³ docelowy
	 * \return KrawêdŸ do wêz³a docelowego
	 */
	Edge* GetEdge(Node* dest) const
	{
		for (const auto edge : edges)
		{
			if (edge->destination == dest)
			{
				return edge;
			}
		}
		return nullptr;
	}
};

/**
 * \brief Graf
 */
class Graph
{
	/**
	 * \brief Wektor wêz³ów
	 */
	vector<Node*> nodes_;

	/**
	 * \brief Czy graf skierowany
	 */
	bool directed;

public:
	/**
	 * \brief Konstruktor
	 * \param nodes Wektor wêz³ów
	 * \param directed Czy graf skierowany
	 */
	explicit Graph(const vector<Node*>& nodes, const bool directed = false)
		: nodes_(nodes), directed(directed)
	{
	}

	/**
	 * \brief Destruktor
	 */
	~Graph()
	= default;

	/**
	 * \brief Zwraca wektor wêz³ów
	 * \return Wektor wêz³ów
	 */
	[[nodiscard]] vector<Node*> GetNodes() const
	{
		return nodes_;
	}

	/**
	 * \brief Zwraca liczbê wêz³ów
	 * \return Liczba wêz³ów w grafie
	 */
	[[nodiscard]] int CountNodes() const
	{
		return nodes_.size();
	}

	/**
	 * \brief Dodanie krawêdzi pomiêdzy wêz³ami
	 * \param source Wêze³ Ÿród³owy
	 * \param destination Wêze³ docelowy
	 * \param distance Dystans
	 */
	void AddEdge(Node* source, Node* destination, const int distance) const
	{
		const auto edge = new Edge();
		edge->source = source;
		edge->destination = destination;
		edge->distance = distance;
		source->edges.push_back(edge);

		if (!directed)
		{
			const auto edge_reverse = new Edge();
			edge_reverse->source = destination;
			edge_reverse->destination = source;
			edge_reverse->distance = distance;
			destination->edges.push_back(edge_reverse);
		}
	}

	/**
	 * \brief Wydobycie krawêdzi z wektora wêz³ów
	 * \param nodes Wektor wêz³ów
	 * \return Wektor krawêdzi
	 */
	static vector<Edge*> ExtractEdges(const vector<Node*>& nodes)
	{
		vector<Edge*> edges;
		edges.reserve(nodes.size() - 1);
		for (int i = 0; i < nodes.size() - 1; ++i)
		{
			edges.push_back(nodes[i]->GetEdge(nodes[i + 1]));
		}
		return edges;
	}
};

/**
 * \brief Algorytm genetyczny
 */
class Genetic
{
	/**
	 * \brief Graf
	 */
	Graph* graph;

	/**
	 * \brief Rozmiar populacji
	 */
	int pop_size;

	/**
	 * \brief Œcie¿ki
	 */
	vector<vector<Node*>> routes;

	/**
	 * \brief Nowa generacja
	 */
	vector<vector<Node*>> new_gen;

	/**
	 * \brief Generator liczb losowych
	 */
	mt19937 rng;

	/**
	 * \brief Procent najlepszych osobników przechodz¹cych do nowej generacji
	 */
	int elitism;

	/**
	 * \brief Procent szansy na mutacjê
	 */
	int mutation;

	/**
	 * \brief Iloœæ generacji
	 */
	int no_gen;

	/**
	 * \brief Mapa prawdopodobieñstwa œcie¿ek
	 */
	multimap<double, vector<Node*>, greater<>> probability_map;

	/**
	 * \brief Generacja losowej œcie¿ki
	 * \return Losowo wygenerowana œcie¿ka
	 */
	vector<Node*> GenerateRoute()
	{
		vector<Node*> init = graph->GetNodes();
		ranges::shuffle(init, rng);
		return init;
	}

	/**
	 * \brief Zwraca ca³kowity dystans wszystkich œcie¿ek
	 * \return Ca³kowity dystans wszystkich œcie¿ek
	 */
	[[nodiscard]] int TotalDistance() const
	{
		int distance = 0;
		for (const auto& route : routes)
		{
			vector tmp = route;
			tmp.push_back(route[0]);
			vector<Edge*> edges = Graph::ExtractEdges(tmp);
			distance = accumulate(edges.begin(), edges.end(), distance, [](const int acc, const Edge* edge)
			{
				return acc + edge->distance;
			});
		}
		return distance;
	}

	/**
	 * \brief Zwraca dystans z wektora krawêdzi
	 * \param edges Wektor krawêdzi
	 * \return Dystans
	 */
	[[nodiscard]] int GetDistance(vector<Edge*> edges) const
	{
		return accumulate(edges.begin(), edges.end(), 0, [](const int acc, const Edge* edge)
		{
			return acc + edge->distance;
		});
	}

	/**
	 * \brief Funkcja dopasowania
	 */
	void Fitting()
	{
		const int total_distance = TotalDistance();

		for (const auto& route : routes)
		{
			vector tmp = route;
			tmp.push_back(route[0]);
			const vector<Edge*> edges = Graph::ExtractEdges(tmp);
			const double fitness = GetDistance(edges);
			probability_map.insert({ fitness / total_distance, route });
		}
	}

	/**
	 * \brief Elityzm
	 */
	void Elitism()
	{
		if (elitism == 0.0) return;
		const int max = pop_size * elitism / 100;
		for (auto it = probability_map.begin(); it != next(probability_map.begin(), max); ++it)
		{
			new_gen.push_back(it->second);
		}
	}

	/**
	 * \brief Wybranie rodzica na podstawie ruletki
	 * \param prob Prawdopodobieñstwo
	 * \return Rodzic
	 */
	vector<Node*> GetParent(const double prob)
	{
		auto it = probability_map.begin();
		double sum = it->first;
		while (sum < prob)
		{
			sum += it->first;
			++it;
		}
		return it->second;
	}

	/**
	 * \brief Generowanie potomstwa za pomoc¹ rekombinacji krawêdzi
	 * \param parent1 Rodzic pierwszy
	 * \param parent2 Rodzic drugi
	 */
	void GenerateChildren(const vector<Node*>& parent1, const vector<Node*>& parent2)
	{
		map<Node*, set<Node*>> neighbor_map;
		for (int i = 0; i < parent1.size(); ++i)
		{
			Node* left = parent1[(i - 1 + parent1.size()) % parent1.size()];
			Node* right = parent1[(i + 1) % parent1.size()];
			neighbor_map.insert({parent1[i], {left, right}});
		}

		for (int i = 0; i < parent2.size(); ++i)
		{
			Node* left = parent2[(i - 1 + parent2.size()) % parent2.size()];
			Node* right = parent2[(i + 1) % parent2.size()];
			neighbor_map[parent2[i]].insert(left);
			neighbor_map[parent2[i]].insert(right);
		}

		for (int i = 0; i < 2; ++i)
		{
			vector<Node*> child;
			auto map_copy = neighbor_map;
			const uniform_int_distribution<int> dist(0, 1);
			Node* X;
			if (dist(rng) == 0)
			{
				child.push_back(X = parent1[0]);
			}
			else
			{
				child.push_back(X = parent2[0]);
			}
			while (child.size() < parent1.size())
			{
				for (auto& val : map_copy | views::values)
				{
					val.erase(X);
				}

				if (map_copy[X].empty())
				{
					vector<Node*> diff;
					ranges::set_difference(parent1, child, back_inserter(diff));
					uniform_int_distribution<int> rand_node(0, diff.size() - 1);
					X = diff[rand_node(rng)];
				}
				else
				{
					set<Node*> neighbors = map_copy[X];
					vector<Node*> least_neighbors;
					int min_size = INT_MAX;
					for (auto& neighbor : neighbors)
					{
						if (map_copy[neighbor].size() == min_size)
						{
							least_neighbors.push_back(neighbor);
						}
						else if (map_copy[neighbor].size() < min_size)
						{
							min_size = map_copy[neighbor].size();
							least_neighbors.clear();
							least_neighbors.push_back(neighbor);
						}
					}
					if (least_neighbors.size() == 1)
					{
						X = least_neighbors[0];
					}
					else if (least_neighbors.size() > 1)
					{
						uniform_int_distribution<int> rand_node(0, least_neighbors.size() - 1);
						X = least_neighbors[rand_node(rng)];
					}
				}
				child.push_back(X);
			}
			new_gen.push_back(child);
		}
	}

	/**
	 * \brief Mutacja dokonuj¹ca zamiany dwóch miast
	 */
	void Mutation()
	{
		const uniform_real_distribution<> dist(0.0, 100.0);
		for (auto& route : new_gen)
		{
			if (dist(rng) > mutation) continue;
			uniform_int_distribution<int> rand_node(0, route.size() - 1);
			const int i = rand_node(rng);
			int j;
			do
			{
				j = rand_node(rng);
			}
			while (i == j);

			swap(route[i], route[j]);
		}
	}

	/**
	 * \brief Selekcja rodziców do tworzenia nowej generacji
	 * \return Para rodziców
	 */
	pair<vector<Node*>, vector<Node*>> Selection()
	{
		const uniform_real_distribution<double> dist(0.0, 1.0);
		const double p1 = dist(rng);
		vector<Node*> parent1 = GetParent(p1);
		vector<Node*> parent2;
		do
		{
			const double p2 = dist(rng);
			parent2 = GetParent(p2);
		}
		while (parent1 == parent2);
		return {parent1, parent2};
	}

	/**
	 * \brief Tworzenie mapy odleg³oœci
	 * \return Mapa œcie¿ek na odleg³oœci
	 */
	[[nodiscard]] multimap<vector<Edge*>, int> CreateDistanceMap() const
	{
		multimap<vector<Edge*>, int> distance_map;
		for (auto& route : routes)
		{
			vector<Node*> tmp = route;
			tmp.push_back(route[0]);
			const auto edges = Graph::ExtractEdges(tmp);
			int distance = GetDistance(edges);
			distance_map.insert({edges, distance});
		}
		return distance_map;
	}

	/**
	 * \brief Wypisanie œcie¿ki wraz z dystansem
	 * \param edges Wektor krawêdzi
	 * \param distance Dystans
	 */
	void PrintOutcome(const vector<Edge*>& edges, const int distance = -1) const
	{
		cout << "ROUTE: " << endl;
		for (const auto& edge : edges)
		{
			cout << edge->source->label << " -> " << edge->destination->label << " : " << edge->distance << endl;
		}
		if (distance == -1)
			cout << "DISTANCE: " << GetDistance(edges) << endl << endl;
		else cout << "DISTANCE: " << distance << endl << endl;
	}

	/**
	 * \brief Tworzenie generacji pocz¹tkowej
	 */
	void Initialize()
	{
		routes.reserve(pop_size);
		for (int i = 0; i < pop_size; i++)
		{
			routes.push_back(GenerateRoute());
		}
	}
public:
	/**
	 * \brief Konstruktor
	 * \param graph Graf
	 * \param pop_size Rozmiar populacji
	 * \param no_gen Liczba generacji
	 * \param elitism Procent elityzmu
	 * \param mutation Procent mutacji
	 */
	explicit Genetic(Graph* graph, const int pop_size, const int no_gen, const int elitism, const int mutation)
		: graph(graph), pop_size(pop_size), rng(random_device()()), elitism(elitism),
		mutation(mutation), no_gen(no_gen)
	{
		Initialize();
	}

	/**
	 * \brief Optymalizacja
	 */
	void Optimize()
	{
		for (int i = 0; i < no_gen; i++)
		{
			Fitting();
			Elitism();
			while (new_gen.size() < pop_size)
			{
				auto [fst, snd] = Selection();
				GenerateChildren(fst, snd);
			}
			Mutation();
			routes = new_gen;
			new_gen.clear();
			if constexpr (DEBUG)
				PrintRoutes();
		}
	}

	/**
	 * \brief Wyœwietlenie tras
	 */
	void PrintRoutes() const
	{
		for (auto& route : routes)
		{
			vector<Node*> tmp = route;
			tmp.push_back(route[0]);
			const auto edges = Graph::ExtractEdges(tmp);
			PrintOutcome(edges);
		}
	}

	/**
	 * \brief Wyœwietlenie najlepszej trasy
	 */
	void PrintBestRoute() const
	{
		auto dist_map = CreateDistanceMap();
		const auto it = ranges::min_element(dist_map, [](const auto& lhs, const auto& rhs)
		{
			return lhs.second < rhs.second;
		});
		PrintOutcome(it->first, it->second);
	}
};

int main()
{
	int N = 6;
	vector<Node*> nodes;
	nodes.reserve(N);
	for (int i = 0; i < N; ++i)
	{
		nodes.push_back(new Node);
	}

	//nodes[0]->label = "0";
	//nodes[1]->label = "1";
	//nodes[2]->label = "2";
	//nodes[3]->label = "3";
	//nodes[4]->label = "4";
	
	Graph g(nodes);

	// pdf

	//g.AddEdge(nodes[0], nodes[1], 1);
	//g.AddEdge(nodes[0], nodes[2], 3);
	//g.AddEdge(nodes[0], nodes[3], 4);
	//g.AddEdge(nodes[0], nodes[4], 5);
	//g.AddEdge(nodes[1], nodes[2], 1);
	//g.AddEdge(nodes[1], nodes[3], 4);
	//g.AddEdge(nodes[1], nodes[4], 8);
	//g.AddEdge(nodes[2], nodes[3], 5);
	//g.AddEdge(nodes[2], nodes[4], 1);
	//g.AddEdge(nodes[3], nodes[4], 2);

	//nodes[0]->label = "1";
	//nodes[1]->label = "2";
	//nodes[2]->label = "3";
	//nodes[3]->label = "4";
	//nodes[4]->label = "5";

	// pdf

	//g.AddEdge(nodes[0], nodes[1], 38);
	//g.AddEdge(nodes[0], nodes[2], 74);
	//g.AddEdge(nodes[0], nodes[3], 59);
	//g.AddEdge(nodes[0], nodes[4], 45);
	//g.AddEdge(nodes[1], nodes[2], 46);
	//g.AddEdge(nodes[1], nodes[3], 61);
	//g.AddEdge(nodes[1], nodes[4], 72);
	//g.AddEdge(nodes[2], nodes[3], 49);
	//g.AddEdge(nodes[2], nodes[4], 85);
	//g.AddEdge(nodes[3], nodes[4], 42);


	nodes[0]->label = "A";
	nodes[1]->label = "B";
	nodes[2]->label = "C";
	nodes[3]->label = "D";
	nodes[4]->label = "E";
	nodes[5]->label = "F";

	//g.AddEdge(nodes[0], nodes[1], 12);
	//g.AddEdge(nodes[0], nodes[2], 10);
	//g.AddEdge(nodes[0], nodes[3], 19);
	//g.AddEdge(nodes[0], nodes[4], 8);
	//g.AddEdge(nodes[1], nodes[2], 3);
	//g.AddEdge(nodes[1], nodes[3], 7);
	//g.AddEdge(nodes[1], nodes[4], 2);
	//g.AddEdge(nodes[2], nodes[3], 6);
	//g.AddEdge(nodes[2], nodes[4], 20);
	//g.AddEdge(nodes[3], nodes[4], 4);

	//g.AddEdge(nodes[0], nodes[1], 14);
	//g.AddEdge(nodes[0], nodes[2], 15);
	//g.AddEdge(nodes[0], nodes[3], 4);
	//g.AddEdge(nodes[0], nodes[4], 9);
	//g.AddEdge(nodes[1], nodes[2], 18);
	//g.AddEdge(nodes[1], nodes[3], 5);
	//g.AddEdge(nodes[1], nodes[4], 13);
	//g.AddEdge(nodes[2], nodes[3], 19);
	//g.AddEdge(nodes[2], nodes[4], 10);
	//g.AddEdge(nodes[3], nodes[4], 12);

	g.AddEdge(nodes[0], nodes[1], 12);
	g.AddEdge(nodes[0], nodes[2], 29);
	g.AddEdge(nodes[0], nodes[3], 22);
	g.AddEdge(nodes[0], nodes[4], 13);
	g.AddEdge(nodes[0], nodes[5], 24);
	g.AddEdge(nodes[1], nodes[2], 19);
	g.AddEdge(nodes[1], nodes[3], 3);
	g.AddEdge(nodes[1], nodes[4], 25);
	g.AddEdge(nodes[1], nodes[5], 6);
	g.AddEdge(nodes[2], nodes[3], 21);
	g.AddEdge(nodes[2], nodes[4], 23);
	g.AddEdge(nodes[2], nodes[5], 28);
	g.AddEdge(nodes[3], nodes[4], 4);
	g.AddEdge(nodes[3], nodes[5], 5);
	g.AddEdge(nodes[4], nodes[5], 16);

	Genetic gen = Genetic(&g, 5000, 20, 10, 2);
	gen.Optimize();
	gen.PrintBestRoute();
}
