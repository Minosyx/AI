#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <ranges>
#include <cstdlib>
#include <random>
#include <cfloat>

class Node;
using namespace std;

/**
 * \brief Kraw�d� grafu
 */
class Edge
{
public:
	/**
	 * \brief Odleg�o��
	 */
	double distance;

	/**
	 * \brief Ilo�� feromonu
	 */
	double pheromone;

	/**
	 * \brief Przyrost feromonu
	 */
	double delta_pheromone;

	/**
	 * \brief �r�d�o
	 */
	Node* source;

	/**
	 * \brief Cel
	 */
	Node* destination;

	/**
	 * \brief Prawdopodobie�stwo
	 */
	double probability;
};

/**
 * \brief W�ze� grafu
 */
class Node
{
public:
	/**
	 * \brief Etykieta
	 */
	string label;

	/**
	 * \brief Wektor kraw�dzi
	 */
	vector<Edge*> edges;

	/**
	 * \brief Destruktor
	 */
	~Node()
	{
		for (const auto e : edges)
			delete e;
	}

	/**
	 * \brief Sprawdzenie czy w�ze� w li�cie s�siedztwa
	 * \param node W�ze�
	 * \return Prawda, je�li w�ze� w li�cie s�siedztwa
	 */
	bool IsInAdjacencyList(Node* node)
	{
		return ranges::any_of(edges.begin(), edges.end(), [&](const Edge* edge) { return edge->destination == node; });
	}

	/**
	 * \brief Pobranie informacji o w�le
	 * \param dest W�ze� docelowy
	 * \return Para dystans - ilo�� feromon�w
	 */
	pair<double, double> GetInfo(Node* dest) const
	{
		for (const auto edge : edges)
		{
			if (edge->destination == dest)
			{
				return { edge->distance, edge->pheromone };
			}
		}
		return { -1, -1 };
	}

	/**
	 * \brief Zwr�cenie kraw�dzi do danego w�z�a
	 * \param dest W�ze� docelowy
	 * \return Kraw�d� powi�zana z w�z�ami
	 */
	Edge *GetEdge(Node* dest) const
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
	 * \brief Wektor w�z��w
	 */
	vector<Node*> nodes_;

	/**
	 * \brief Czy graf skierowany
	 */
	bool directed;

public:
	/**
	 * \brief Konstruktor
	 * \param nodes Wektor wska�nik�w do w�z��w
	 * \param directed Czy graf skierowany
	 */
	explicit Graph(const vector<Node*>& nodes, const bool directed = false)
		: nodes_(nodes), directed(directed)
	{}

	/**
	 * \brief Destruktor
	 */
	~Graph()
	= default;

	/**
	 * \brief Pobranie w�z��w
	 * \return Wektor w�z��w
	 */
	[[nodiscard]] vector<Node*> GetNodes() const
	{
		return nodes_;
	}

	/**
	 * \brief Pobranie ilo�ci w�z��w
	 * \return Ilo�� w�z��w
	 */
	[[nodiscard]] int CountNodes() const
	{
		return nodes_.size();
	}

	/**
	 * \brief Dodanie kraw�dzi do grafu
	 * \param source W�ze� �r�d�owy
	 * \param destination W�ze� docelowy
	 * \param distance Odleg�o��
	 * \param pheromone Ilo�� feromon�w
	 */
	void AddEdge(Node* source, Node* destination, const int distance, const double pheromone = 1e-15) const
	{
		Edge* edge = new Edge();
		edge->source = source;
		edge->destination = destination;
		edge->distance = distance;
		edge->pheromone = pheromone;
		edge->delta_pheromone = 0.0;
		edge->probability = 0.0;
		source->edges.push_back(edge);

		if (!directed)
		{
			Edge* edge_reverse = new Edge();
			edge_reverse->source = destination;
			edge_reverse->destination = source;
			edge_reverse->distance = distance;
			edge_reverse->pheromone = pheromone;
			edge_reverse->delta_pheromone = 0.0;
			edge_reverse->probability = 0.0;
			destination->edges.push_back(edge_reverse);
		}
	}
};

/**
 * \brief Algorytm mr�wkowy
 */
class AntAlgorithm
{
	/**
	 * \brief Graf
	 */
	Graph graph;

	/**
	 * \brief Wa�no�� �ladu feromonowego
	 */
	double alpha;

	/**
	 * \brief Wa�no�� widoczno�ci nast�pnego miasta
	 */
	double beta;

	/**
	 * \brief Wsp�czynnik parowania feromonu
	 */
	double rho;

	/**
	 * \brief Sta�a wp�ywaj�ca na przyrost feromonu
	 */
	double Q;

	/**
	 * \brief Ilo�� mr�wek
	 */
	int no_ants;

	/**
	 * \brief Miasto startowe
	 */
	int start_city;

	/**
	 * \brief D�ugo�� najlepszej trasy
	 */
	double best_length;

	/**
	 * \brief Najlepsza trasa
	 */
	vector<Node*> best_route;

	/**
	 * \brief Wektor tras
	 */
	vector<vector<Node*>> routes;

	/**
	 * \brief Generator liczb losowych
	 */
	mt19937 rng;

public:
	/**
	 * \brief Konstruktor
	 * \param graph Graf
	 * \param alpha Wa�no�� �ladu feromonowego
	 * \param beta Wa�no�� widoczno�ci nast�pnego miasta
	 * \param rho Wsp�czynnik parowania feromonu
	 * \param q Sta�a wp�ywaj�ca na przyrost feromonu
	 * \param no_ants Ilo�� mr�wek
	 */
	AntAlgorithm(const Graph& graph, const double alpha, const double beta, const double rho, const double q, const int no_ants)
		: graph(graph),
		  alpha(alpha),
		  beta(beta),
		  rho(rho),
		  Q(q),
		  no_ants(no_ants), start_city(0), best_length(DBL_MAX), rng(random_device()())
	{
		routes.resize(no_ants, vector<Node*>());
	}

	/**
	 * \brief Destruktor
	 */
	~AntAlgorithm()
	= default;

	/**
	 * \brief Sprawdzenie czy w�ze� odwiedzony przez dan� mr�wk�
	 * \param node W�ze�
	 * \param ant_k Numer mr�wki
	 * \return Prawda, je�li w�ze� zosta� przez dan� mr�wk� odwiedzony
	 */
	bool IsVisited(Node* node, const int ant_k) const
	{
		return ranges::any_of(routes[ant_k].begin(), routes[ant_k].end(), [&](Node* n) { return n == node; });
	}

	/**
	 * \brief Pobranie prawdopodobie�stwa
	 * \param a W�ze� �r�d�owy
	 * \param b W�ze� docelowy
	 * \param ant_k Numer mr�wki
	 * \return Prawdopodobie�stwo przej�cia dan� kraw�dzi�
	 */
	double GetProbability(const Node* a, Node* b, const int ant_k) const
	{
		auto [dist, ph] = a->GetInfo(b);
		const double eta_ij = pow(1 / pow(dist, 2), beta);
		//const double eta_ij = pow(1 / dist, beta);
		const double tau_ij = pow(ph, alpha);

		double sum = 0;
		for (const auto& edge : a->edges)
		{
			if (!IsVisited(edge->destination, ant_k))
			{
				auto [d, p] = a->GetInfo(edge->destination);
				const double eta = pow(1 / pow(d, 2), beta);
				//const double eta = pow(1 / d, beta);
				const double tau = pow(p, alpha);
				sum += eta * tau;
			}
		}

		return tau_ij * eta_ij / sum;
	}

	/**
	 * \brief Pobranie ca��go pokonanego dystansu
	 * \param ant_k Numer mr�wki
	 * \return D�ugo�� trasy pokonanej przez mr�wk�
	 */
	[[nodiscard]] double TotalDistance(const int ant_k) const
	{
		vector<Edge*> edges;
		for (int i = 0; i < routes[ant_k].size() - 1; ++i)
		{
			const auto e = routes[ant_k][i]->GetEdge(routes[ant_k][static_cast<std::vector<Node*, std::allocator<Node*>>::size_type>(i) + 1]);
			edges.push_back(e);
		}
		return accumulate(edges.begin(), edges.end(), 0.0, [](const double sum, const Edge* e) { return sum + e->distance; });
	}

	/**
	 * \brief Aktualizacja feromon�w
	 */
	void UpdatePheromones() const
	{
		for (int i = 0; i < no_ants; i++)
		{
			const double rlength = TotalDistance(i);
			for (int j = 0; j < graph.CountNodes() - 1; j++)
			{
				Node* a = routes[i][j];
				Node* b = routes[i][static_cast<std::vector<Node*, std::allocator<Node*>>::size_type>(j) + 1];
				Edge * e = a->GetEdge(b);
				Edge * re = b->GetEdge(a);
				e->delta_pheromone += Q / rlength;
				re->delta_pheromone += Q / rlength;
			}
		}
		for (const auto &node : graph.GetNodes())
		{
			for (const auto &edge : node->edges)
			{
				edge->pheromone = (1 - rho) * edge->pheromone + edge->delta_pheromone;
				edge->delta_pheromone = 0;
			}
		}
	}

	/**
	 * \brief Wyb�r nast�pnego miasta dla danej mr�wki
	 * \param n Aktualne miasto
	 * \param ant_k Numer mr�wki
	 * \return W�ze� docelowy
	 */
	Node* City(const Node* n, const int ant_k)
	{
		const double xi = uniform_real_distribution<double>(0, 1)(rng);
		double sum = 0;
		Node* dest = nullptr;
		for (const auto& e : n->edges)
		{
			if (!IsVisited(e->destination, ant_k))
			{
				sum += e->probability;
				dest = e->destination;
				if (sum >= xi) break;
			}
		}
		return dest;
	}

	/**
	 * \brief Aktualizacja miasta startowgo
	 */
	void UpdateStartCity()
	{
		start_city = (start_city + 1) % graph.CountNodes();
	}

	/**
	 * \brief Generowanie trasy mr�wki
	 * \param ant_k Numer mr�wki
	 */
	void Route(int ant_k)
	{
		Node* start;
		Node* node = start = graph.GetNodes()[start_city];
		routes[ant_k].push_back(node);
		UpdateStartCity();
		while (routes[ant_k].size() < graph.CountNodes())
		{
			int count = 0;
			for (const auto &edge : node->edges)
			{
				if (!IsVisited(edge->destination, ant_k))
				{
					edge->probability = GetProbability(node, edge->destination, ant_k);
					count++;
				}
			}

			if (count == 0)
				return;

			node = City(node, ant_k);
			routes[ant_k].push_back(node);
		}

		routes[ant_k].push_back(start);
	}

	/**
	 * \brief Optymalizacja trasy
	 * \param iterations Ilo�� iteracji
	 */
	void Optimize(int iterations)
	{
		for (int it = 1; it <= iterations; it++)
		{
			//cout << "ITERATION " << it << " HAS STARTED!" << endl << endl;

			for (int k = 0; k < no_ants; k++)
			{
				//cout << " : ant " << k << " has been released!" << endl;

				Route(k);

				//cout << "  :: route done" << endl;
				const double rlength = TotalDistance(k);

				if (rlength < best_length)
				{
					best_route.clear();
					best_length = rlength;
					ranges::copy(routes[k], back_inserter(best_route));
				}
				PrintRoute();
				//cout << " : ant " << k << " has ended!" << endl;
			}
			//cout << endl << "updating PHEROMONES . . .";
			UpdatePheromones();
			//cout << " done!" << endl << endl;

			for (int i = 0; i < no_ants; i++)
			{
				routes[i].clear();
			}

			//cout << endl << "ITERATION " << it << " HAS ENDED!" << endl << endl;
		}
	}

	/**
	 * \brief Wy�wietlanie �cie�ki
	 */
	void PrintRoute() const
	{
		cout << "ROUTE: " << endl;
		for (int i = 0; i < best_route.size() - 1; i++)
		{
			const auto e = best_route[i]->GetEdge(best_route[static_cast<std::vector<Node*, std::allocator<Node*>>::size_type>(i) + 1]);
			cout << e->source->label << " -> " << e->destination->label << " : " << e->distance << endl;
		}
		cout << endl << "LENGTH: " << best_length << endl;
	}
};

int main()
{
	const int N = 6;
	vector<Node*> nodes;
	nodes.reserve(N);
	for (int i = 0; i < N; ++i)
	{
		nodes.push_back(new Node);
	}
	//nodes[0]->label = "1";
	//nodes[1]->label = "2";
	//nodes[2]->label = "3";
	//nodes[3]->label = "4";
	//nodes[4]->label = "5";

	const Graph g(nodes);

	// pdf

	//g.AddEdge(nodes[0], nodes[1], 38, 3);
	//g.AddEdge(nodes[0], nodes[2], 74, 2);
	//g.AddEdge(nodes[0], nodes[3], 59, 2);
	//g.AddEdge(nodes[0], nodes[4], 45, 2);
	//g.AddEdge(nodes[1], nodes[2], 46, 1);
	//g.AddEdge(nodes[1], nodes[3], 61, 1);
	//g.AddEdge(nodes[1], nodes[4], 72, 1);
	//g.AddEdge(nodes[2], nodes[3], 49, 2);
	//g.AddEdge(nodes[2], nodes[4], 85, 2);
	//g.AddEdge(nodes[3], nodes[4], 42, 1);

	// ex 1

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

	// ex 2

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

	// ex 4

	g.AddEdge(nodes[0], nodes[1], 12, 1);
	g.AddEdge(nodes[0], nodes[2], 29, 1);
	g.AddEdge(nodes[0], nodes[3], 22, 1);
	g.AddEdge(nodes[0], nodes[4], 13, 1);
	g.AddEdge(nodes[0], nodes[5], 24, 1);
	g.AddEdge(nodes[1], nodes[2], 19, 1);
	g.AddEdge(nodes[1], nodes[3], 3, 1);
	g.AddEdge(nodes[1], nodes[4], 25, 1);
	g.AddEdge(nodes[1], nodes[5], 6, 1);
	g.AddEdge(nodes[2], nodes[3], 21, 1);
	g.AddEdge(nodes[2], nodes[4], 23, 1);
	g.AddEdge(nodes[2], nodes[5], 28, 1);
	g.AddEdge(nodes[3], nodes[4], 4, 1);
	g.AddEdge(nodes[3], nodes[5], 5, 1);
	g.AddEdge(nodes[4], nodes[5], 16, 1);

	AntAlgorithm ant(g, 1, 1, 0.5, 5, 100);
	ant.Optimize(10);
	ant.PrintRoute();

	for (const auto &node : nodes)
	{
		delete node;
	}
}
