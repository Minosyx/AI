#define _USE_MATH_DEFINES
#define MAX_IT 20000
#include <iostream>
#include <cmath>
#include <random>

using namespace std;

using Function = double (*)(double);

/// <summary>
/// Generowanie rzeczywistych liczb losowych
/// </summary>
/// <param name="min">Minimalna warto�� przedzia�u</param>
/// <param name="max">Maksymalna warto�� przedzia�u</param>
/// <returns>Losowa liczba rzeczywista z przedzia�u</returns>
double generateRandomDouble(const double min, const double max)
{
	static bool is_seeded = false;
	static std::mt19937 generator;

	if (!is_seeded) {
		std::random_device rd;
		generator.seed(rd());
		is_seeded = true;
	}

	std::uniform_real_distribution<double> distribution(min, max);
	return distribution(generator);
}

/// <summary>
/// Generowanie ca�kowitych liczb losowych
/// </summary>
/// <param name="min">Minimalna warto�� przedzia�u</param>
/// <param name="max">Maksymalna warto�� przedzia�u</param>
/// <returns>Losowa liczba ca�kowita z przedzia�u</returns>
int generateRandomInt(const int min, const int max)
{
	static bool is_seeded = false;
	static std::mt19937 generator;

	if (!is_seeded) {
		std::random_device rd;
		generator.seed(rd());
		is_seeded = true;
	}
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(generator);
}


double f(double x)
{
	return 3 * sin(M_PI * x / 5) + sin(M_PI * x);
}

/// <summary>
/// Wsp�rz�dne kartezja�skie
/// </summary>
struct Value
{
	/// <summary>
	/// Argument
	/// </summary>
	double x;
	
	/// <summary>
	/// Warto��
	/// </summary>
	double y;
};

/// <summary>
/// Wy�wietlanie wsp�rz�dnych x i y
/// </summary>
/// <param name="x">Argument</param>
/// <param name="y">Warto��</param>
void printValues(double x, double y)
{
	cout << "f(" << x << ") = " << y << endl;
}

/// <summary>
/// Algorytm symulowanego wy�arzania
/// </summary>
/// <param name="f">Funkcja</param>
/// <param name="start">Pocz�tek zakresu</param>
/// <param name="end">Koniec zakresu</param>
/// <param name="T">Temperatura</param>
/// <param name="alfa">Wsp�czynnik alfa</param>
/// <param name="epoki">Liczba epok</param>
/// <param name="proby">Liczba pr�b w epoce</param>
/// <returns>Struktura przechowuj�ca wsp�rz�dne x i y wyniku</returns>
Value ASW(Function f, double start, double end, double T, double alfa, int epoki, int proby)
{
	double x0 = generateRandomDouble(start, end);
	double f0 = f(x0);
	double x1, f1;

	printValues(x0, f0);

	for (int i = 0; i < epoki; ++i)
	{
		for (int j = 0; j < proby; ++j)
		{
			x1 = generateRandomDouble(x0 - 2 * T < start ? start : x0 - 2 * T, x0 + 2 * T > end ? end : x0 + 2 * T);
			f1 = f(x1);

			printValues(x1, f1);

			if (f1 > f0) 
			{
				x0 = x1;
				f0 = f1;
			}
			else
			{
				double prob = exp((f1 - f0) / T);
				double random = generateRandomDouble(0, 1);

				cout << "Prawdopodobienstwo: " << prob << " > " << random << endl << endl;

				if (random < prob)
				{
					x0 = x1;
					f0 = f1;
				}
			}
		}
		T = T * alfa;
	}
	return {x0, f0};
}

/// <summary>
/// Lista kraw�dzi grafu
/// </summary>
struct ListNode {
	/// <summary>
	/// Wierzcho�ek docelowy
	/// </summary>
	int dest;

	/// <summary>
	/// Wierzcho�ek �r�d�owy
	/// </summary>
	int src;

	/// <summary>
	/// Waga
	/// </summary>
	int weight;

	/// <summary>
	/// Wska�nik do kolejnej kraw�dzi listy
	/// </summary>
	ListNode* next;
};

/// <summary>
/// Graf
/// </summary>
struct Graph {
	/// <summary>
	/// Ilo�� wierzcho�k�w
	/// </summary>
	int vertices;

	/// <summary>
	/// Czy graf jest skierowany?
	/// </summary>
	bool directed;

	/// <summary>
	/// Tablica list kraw�dzi dla wierzcho�k�w
	/// </summary>
	ListNode** array;
};

/// <summary>
/// �cie�ka
/// </summary>
struct Path
{
	/// <summary>
	/// Tablica z kraw�dziami buduj�cymi �cie�k�
	/// </summary>
	ListNode** path;

	/// <summary>
	/// Sumaryczna waga kraw�dzi �cie�ki
	/// </summary>
	int distance;
};

/// <summary>
/// Tworzenie kraw�dzi
/// </summary>
/// <param name="src">Wierzcho�ek �r�d�owy</param>
/// <param name="dest">Wierzcho�ek docelowy</param>
/// <param name="weight">Waga</param>
/// <returns>Utworzona kraw�d�</returns>
ListNode* makeListNode(int src, int dest, int weight) {
	ListNode* l = new ListNode();
	l->src = src;
	l->dest = dest;
	l->weight = weight;
	l->next = nullptr;
	return l;
}

/// <summary>
/// Tworzenie grafu
/// </summary>
/// <param name="vertices">Ilo�� wierzcho�k�w</param>
/// <param name="directed">Czy graf jest skierowany</param>
/// <returns>Graf</returns>
Graph* makeGraph(int vertices, bool directed) {
	Graph* g = new Graph();
	g->vertices = vertices;
	g->directed = directed;
	g->array = new ListNode * [vertices];

	for (int i = 0; i < vertices; i++) {
		g->array[i] = nullptr;
	}
	return g;
}

/// <summary>
/// Dodawanie kraw�dzi mi�dzy wierzcho�kami
/// </summary>
/// <param name="g">Graf</param>
/// <param name="src">Wierzcho�ek �r�d�owy</param>
/// <param name="dest">Wierzcho�ek docelowy</param>
/// <param name="weight">Waga</param>
void addEdge(Graph* g, int src, int dest, int weight) {
	ListNode* node = makeListNode(src, dest, weight);
	node->next = g->array[src];
	g->array[src] = node;

	if (!g->directed) {
		node = makeListNode(dest, src, weight);
		node->next = g->array[dest];
		g->array[dest] = node;
	}
}

/// <summary>
/// Tworzenie �cie�ki z tablicy wierzcho�k�w
/// </summary>
/// <param name="g">Graf</param>
/// <param name="path">Tablica wierzcho�k�w</param>
/// <returns>�cie�ka</returns>
Path createPath(Graph *g, int* path)
{
	ListNode** output = new ListNode * [g->vertices];
	int sum = 0;
	for (int i = 0; i < g->vertices; ++i)
	{
		ListNode* el = g->array[path[i]];
		while (el)
		{
			if (el->dest == path[i + 1])
				break;
			el = el->next;
		}
		if (el) 
		{
			output[i] = el;
			sum += el->weight;
		}
	}
	return { output, sum };
}


/// <summary>
/// Wy�wietlanie �cie�ki
/// </summary>
/// <param name="g">Graf</param>
/// <param name="res">Struktura �cie�ki</param>
void printPath(Graph* g, Path res)
{
	for (int i = 0; i < g->vertices; ++i)
	{
		cout << res.path[i]->src << " -> " << res.path[i]->dest << " : " << res.path[i]->weight << endl;
	}
	cout << "Distance: " << res.distance << endl << endl;
}

/// <summary>
/// Sprawdzenie czy dana �cie�ka by�a ju� wygenerowana
/// </summary>
/// <param name="history">Wektor z poprzednio wygenerowanymi �cie�kami</param>
/// <param name="path">Aktualnie wygenerowana �cie�ka</param>
/// <param name="vertices">Ilo�� wierzcho�k�w w �cie�ce</param>
/// <returns>Czy �cie�ka ju� wyst�pi�a</returns>
bool pathExists(vector<int*> history, int* path, int vertices) 
{
	for (int* p : history)
	{
		bool exists = true;
		for (int i = 0; i < vertices; ++i)
		{
			if (path[i] != p[i])
			{
				exists = false;
				break;
			}
		}
		if (exists) return true;
	}
	return false;
}

/// <summary>
/// Wypisanie historii
/// </summary>
/// <param name="history">Wektor z histori� wygenerowanych �cie�ek</param>
/// <param name="vertices">Liczba wierzcho�k�w w �cie�ce</param>
void printHistory(vector<int*> history, int vertices)
{
	for (int* p : history)
	{
		for (int i = 0; i < vertices + 1; ++i)
		{
			cout << p[i] << " ";
		}
		cout << endl;
	}
}

/// <summary>
/// Algorytm symulowanego wy�arzania dla problemu komiwoja�era
/// </summary>
/// <param name="g">Graf</param>
/// <param name="T">Temperatura</param>
/// <param name="alfa">Wsp�czynnik alfa</param>
/// <param name="epoki">Liczba epok</param>
/// <param name="proby">Liczba pr�b w epoce</param>
/// <returns>�cie�ka</returns>
Path TSP_ASW(Graph *g, double T, double alfa, int epoki, int proby)
{
	int* path = new int[g->vertices + 1];
	int* pathCandidate = new int[g->vertices + 1];
	vector<int> tmp = vector<int>(g->vertices);
	vector<int*> history = vector<int*>();

	for (int v = 0; v < g->vertices; ++v)
	{
		tmp[v] = v;
	}

	for (int v = 0; v < g->vertices; ++v)
	{
		int num = generateRandomInt(0, tmp.size() - 1);
		path[v] = tmp.at(num);
		tmp.erase(tmp.begin() + num);
	}
	path[g->vertices] = path[0];

	Path p = createPath(g, path);
	printPath(g, p);

	int* historyEntry = new int[g->vertices + 1];
	copy_n(path, g->vertices + 1, pathCandidate);
	copy_n(path, g->vertices + 1, historyEntry);
	history.push_back(historyEntry);
	
	for (int i = 0; i < epoki; ++i)
	{
		for (int j = 0; j < proby; ++j)
		{
			int it = -1;
			do
			{
				++it;
				int first = generateRandomInt(0, g->vertices - 1), second;
				do
				{
					second = generateRandomInt(0, g->vertices - 1);
				} while (second == first);
				swap(pathCandidate[first], pathCandidate[second]);
				pathCandidate[g->vertices] = pathCandidate[0];
			} while (it < MAX_IT && pathExists(history, pathCandidate, g->vertices + 1));
			
			//cout << it << endl;

			Path candidate = createPath(g, pathCandidate);
			printPath(g, candidate);
			historyEntry = new int[g->vertices + 1];
			copy_n(pathCandidate, g->vertices + 1, historyEntry);
			history.push_back(historyEntry);

			if (candidate.distance < p.distance)
			{
				p = candidate;
				copy_n(pathCandidate, g->vertices + 1, path);
			}
			else
			{
				double prob = exp((p.distance - candidate.distance) / T);
				double random = generateRandomDouble(0, 1);

				cout << "Prawdopodobienstwo: " << prob << " > " << random << endl << endl;

				if (random < prob)
				{
					p = candidate;
					copy_n(pathCandidate, g->vertices + 1, path);
				}
				else
					copy_n(path, g->vertices + 1, pathCandidate);
			}
		}
		T = T * alfa;
	}
	printHistory(history, g->vertices);
	for (int* p : history)
	{
		delete[] p;
	}
	delete[] path;
	delete[] pathCandidate;
	return p;
}

int main()
{
	//Value result = ASW(f, 0, 10, 1, 0.9, 100, 20);
	//Value result = ASW(f, 0, 10, 1, 0.9, 5, 3);
	//cout << endl << "Wynik koncowy: " << endl;
	//printValues(result.x, result.y);

	const int V = 5;
	const int directed = false;
	Graph* graph = makeGraph(V, directed);

	addEdge(graph, 0, 1, 500);
	addEdge(graph, 0, 2, 200);
	addEdge(graph, 0, 3, 185);
	addEdge(graph, 0, 4, 205);
	addEdge(graph, 1, 2, 305);
	addEdge(graph, 1, 3, 360);
	addEdge(graph, 1, 4, 340);
	addEdge(graph, 2, 3, 320);
	addEdge(graph, 2, 4, 165);
	addEdge(graph, 3, 4, 302);

	Path p = TSP_ASW(graph, 100, 0.5, 5, 3);
	cout << endl << "Wynik koncowy: " << endl;
	printPath(graph, p);
	delete graph;
	delete[] p.path;
}