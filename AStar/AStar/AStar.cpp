#define _USE_MATH_DEFINES

#include <iostream>
#include <algorithm>
#include <map>
#include <queue>
#include <vector>
#include <cmath>

using namespace std;

/**
 * \brief Wspó³rzêdne geograficzne
 */
struct Coordinates
{
	/**
	 * \brief Szerokoœæ geograficzna
	 */
	double lat;

	/**
	 * \brief Wysokoœæ geograficzna
	 */
	double lon;
};

/// <summary>
/// Lista krawêdzi grafu
/// </summary>
struct ListNode
{
    /// <summary>
    /// Wierzcho³ek docelowy
    /// </summary>
    int dest;

    /// <summary>
    /// Wierzcho³ek Ÿród³owy
    /// </summary>
    int src;

    /// <summary>
    /// Waga
    /// </summary>
    int weight;

    /// <summary>
    /// Etykieta Ÿród³a
    /// </summary>
    string slabel;

    /// <summary>
    /// Etykieta celu
    /// </summary>
    string dlabel;

    /// <summary>
    /// WskaŸnik do kolejnej krawêdzi listy
    /// </summary>
    ListNode* next;
};

/// <summary>
/// Graf
/// </summary>
struct Graph
{
    /// <summary>
    /// Iloœæ wierzcho³ków
    /// </summary>
    int vertices;

    /// <summary>
    /// Czy graf jest skierowany?
    /// </summary>
    bool directed;

    /// <summary>
    /// Tablica list krawêdzi dla wierzcho³ków
    /// </summary>
    ListNode** array;

    /// <summary>
    /// Czy u¿ywaæ etykiet
    /// </summary>
    bool useLabels;

    /// <summary>
    /// Etykiety
    /// </summary>
    const string* labels;
};

/// <summary>
/// Œcie¿ka
/// </summary>
struct Path
{
    /// <summary>
    /// Wektor z krawêdziami buduj¹cymi œcie¿kê
    /// </summary>
    vector<ListNode*> path;

    /// <summary>
    /// Sumaryczna waga krawêdzi œcie¿ki
    /// </summary>
    int distance;
};

#pragma region Kolejka_priorytetowa

/// <summary>
/// Kolejka priorytetowa
/// </summary>
struct Queue
{
	/**
	 * \brief Wartoœæ
	 */
	int node;

	/**
	 * \brief Priorytet
	 */
	double priority;

	/**
	 * \brief Waga
	 */
	int distance;

	/**
	 * \brief WskaŸnik do nastêpnego elementu
	 */
	Queue* next;
};

/**
 * \brief Tworzenie nowego elementu kolejki
 * \param node Wartoœæ
 * \param p Priorytet
 * \param dist Waga
 * \return WskaŸnik elementu kolejki
 */
Queue* createQ(int node, double p, int dist)
{
    Queue* new_Q = new Queue();
    if (!new_Q)
    {
        cout << "Blad alokacji pamieci!" << endl << endl;
        return nullptr;
    }
    new_Q->node = node;
    new_Q->priority = p;
    new_Q->distance = dist;
    new_Q->next = nullptr;
    return new_Q;
}

/**
 * \brief Dodawanie elementu do kolejki
 * \param q Kolejka
 * \param new_Q Element kolejki
 */
void add(Queue** q, Queue* new_Q)
{
    while (*q && (*q)->priority < new_Q->priority)
    {
        q = &(*q)->next;
    }
    new_Q->next = nullptr;
    if (*q)
        new_Q->next = *q;
    *q = new_Q;
}

/**
 * \brief Zwracanie elementu o najmnie¿szym priorytecie
 * \param q Kolejka
 * \return WskaŸnik do elementu kolejki
 */
Queue* getCheapest(Queue** q)
{
    if (!(*q))
        return nullptr;
    else
    {
        Queue* tmp = *q;
        *q = (*q)->next;
        return tmp;
    }
}

/**
 * \brief Podejrzenie elementu o najmnie¿szym priorytecie
 * \param q Kolejka
 * \return WskaŸnik do elementu kolejki
 */
Queue* lookCheapest(Queue** q)
{
    if (!(*q))
        return nullptr;
    else
    {
        return *q;
    }
}

/**
 * \brief Zmiana priorytetu elementu w kolejce
 * \param q Kolejka
 * \param el Element kolejki
 * \param newP Nowy priorytet
 */
void changePriority(Queue** q, Queue** el, double newP)
{
    if (!(*q)->next)
    {
        (*q)->priority = newP;
        return;
    }
    (*el)->priority = newP;
    Queue* tmp = *el;
    *el = (*el)->next;
    if (*el && tmp->priority < (*el)->priority)
        add(el, tmp);
    else
        add(q, tmp);
}

/**
 * \brief Sprawdzenie czy kolejka jest pusta
 * \param q Kolejka
 * \return Informacja czy kolejka jest pusta
 */
bool emptyCheck(Queue* q)
{
    if (q)
        return false;
    else
        return true;
}

/**
 * \brief Wyœwietlanie zawartoœci kolejki
 * \param q Kolejka
 */
void printQueue(Queue* q)
{
    while (q)
    {
        cout << q->node << " -> [" << q->priority << "]" << "; ";
        q = q->next;
    }
    cout << endl;
}

/**
 * \brief Szukanie elementu o zadanej wartoœci w kolejce
 * \param q Kolejka
 * \param node Szukana wartoœæ
 * \return WskaŸnik do znalezionego elementu lub pusty wskaŸnik
 */
Queue** findEl(Queue** q, int node)
{
    if (!(*q))
        return nullptr;
    while (*q && (*q)->node != node)
    {
        q = &(*q)->next;
    }
    if (*q && (*q)->node == node)
        return q;
    else
        return nullptr;
}

/**
 * \brief Usuwanie elementu kolejki
 * \param el Element kolejki
 */
void del(Queue** el)
{
    Queue* p;
    if (*el == nullptr)
        return;
    p = *el;
    *el = (*el)->next;
}

/**
 * \brief Opró¿nianie kolejki
 * \param q Kolejka
 */
void emptyQueue(Queue** q)
{
    Queue* p = *q;
    while (p)
    {
        *q = (*q)->next;
        delete p;
        p = *q;
    }
}

#pragma endregion

/// <summary>
/// Tworzenie krawêdzi
/// </summary>
/// <param name="src">Wierzcho³ek Ÿród³owy</param>
/// <param name="dest">Wierzcho³ek docelowy</param>
/// <param name="weight">Waga</param>
/// <param name="slabel">Etykieta Ÿród³a</param>
/// <param name="dlabel">Etykieta celu</param>
/// <returns>Utworzona krawêdŸ</returns>
ListNode* makeListNode(const int src, const int dest, const int weight, const string &slabel = "", const string& dlabel = "")
{
    ListNode* l = new ListNode();
    l->src = src;
    l->dest = dest;
    l->weight = weight;
    l->next = nullptr;
    l->slabel = slabel;
    l->dlabel = dlabel;
    return l;
}

/// <summary>
/// Tworzenie grafu
/// </summary>
/// <param name="vertices">Iloœæ wierzcho³ków</param>
/// <param name="directed">Czy graf jest skierowany</param>
/// <param name="useLabels">Czy u¿yæ etykiet</param>
/// <param name="labels">Etykiety</param>
/// <returns>Graf</returns>
Graph* makeGraph(const int vertices, const bool directed, const bool useLabels = false, const string* labels = nullptr)
{
    Graph* g = new Graph();
    g->vertices = vertices;
    g->directed = directed;
    g->array = new ListNode * [vertices];
    g->useLabels = useLabels;
    g->labels = labels;

    for (int i = 0; i < vertices; i++)
    {
        g->array[i] = nullptr;
    }
    return g;
}

/// <summary>
/// Dodawanie krawêdzi miêdzy wierzcho³kami
/// </summary>
/// <param name="g">Graf</param>
/// <param name="src">Wierzcho³ek Ÿród³owy</param>
/// <param name="dest">Wierzcho³ek docelowy</param>
/// <param name="weight">Waga</param>
/// <param name="useLabels">Czy u¿yæ etykiet</param>
/// <param name="labels">Etykiety</param>
void addEdge(const Graph* g, const int src, const int dest, const int weight)
{

    ListNode* node = g->useLabels ? makeListNode(src, dest, weight, g->labels[src], g->labels[dest]) : makeListNode(src, dest, weight);
    node->next = g->array[src];
    g->array[src] = node;

    if (!g->directed)
    {
        node = g->useLabels ? makeListNode(dest, src, weight, g->labels[dest], g->labels[src]) : makeListNode(dest, src, weight);
        node->next = g->array[dest];
        g->array[dest] = node;
    }
}

/// <summary>
/// Tworzenie œcie¿ki z wektora wierzcho³ków
/// </summary>
/// <param name="g">Graf</param>
/// <param name="path">Wektor wierzcho³ków</param>
/// <returns>Œcie¿ka</returns>
Path createPath(const Graph* g, const vector<int> &path)
{
    vector<ListNode*> output;
    int sum = 0;
    for (int i = 0; i < path.size() - 1; ++i)
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
            output.push_back(el);
            sum += el->weight;
        }
    }
    return { output, sum };
}

/// <summary>
/// Wyœwietlanie œcie¿ki
/// </summary>
/// <param name="g">Graf</param>
/// <param name="res">Struktura œcie¿ki</param>
/// <param name="useLabels">Czy u¿yæ etykiet</param>
void printPath(Path const &res, const bool useLabels = false)
{
    for (const ListNode* p : res.path)
    {
        if (!useLabels)
			cout << p->src << " -> " << p->dest << " : " << p->weight << endl;
        else
            cout << p->slabel << " -> " << p->dlabel << " : " << p->weight << endl;
    }
    cout << "Distance: " << res.distance << endl << endl;
}

/**
 * \brief Obliczanie odleg³oœci w metrach na podstawie wspó³rzêdnych geograficznych
 * \param a Punkt a na mapie
 * \param b Punkt b na mapie
 * \return odleg³oœæ w metrach pomiêdzy punktami
 */
double haversine(const Coordinates &a, const Coordinates &b)
{
    const double R = 6371e3;
    const double fi1 = a.lat * M_PI / 180;
    const double fi2 = b.lat * M_PI / 180;
    const double dfi = (b.lat - a.lat) * M_PI / 180;
    const double dla = (b.lon - a.lon) * M_PI / 180;

    const double angle = sin(dfi / 2) * sin(dfi / 2) + cos(fi1) * cos(fi2) * sin(dla / 2) * sin(dla / 2);
    const double c = 2 * atan2(sqrt(angle), sqrt(1 - angle));
    return R * c;
}

/**
 * \brief Algorytm A* dla grafu
 * \param g Graf
 * \param heuristic Tablica z heurystyk¹
 * \param start Wêze³ startowy
 * \param goal Wêze³ koñcowy
 * \return Wyznaczona œcie¿ka
 */
Path AStarF(const Graph* g, const int* heuristic, const int start, const int goal)
{
    vector<int> path = vector<int>(g->vertices, -1);
    Queue* open = createQ(start, heuristic[start], 0);
    Queue* closed = nullptr;

    path[start] = start;
    while (!emptyCheck(open))
    {
        Queue* ch = getCheapest(&open);
        if (ch->node == goal)
        {
            vector<int> endPath;
            int n = goal;

            while (path[n] != n)
            {
                endPath.push_back(n);
                n = path[n];
            }

            emptyQueue(&open);
            emptyQueue(&closed);

            endPath.push_back(start);
            reverse(endPath.begin(), endPath.end());

            return createPath(g, endPath);
        }
        else
        {
            ListNode* it = g->array[ch->node];

            if (closed == nullptr)
                closed = createQ(ch->node, ch->priority, ch->distance);
            else
                add(&closed, ch);

            while (it)
            {
	            const int totalWeight = ch->distance + it->weight;
                Queue** fopen = findEl(&open, it->dest);
                Queue** fclosed = findEl(&closed, it->dest);
                if (fopen == nullptr && fclosed == nullptr)
                {
                    path[it->dest] = ch->node;
                    Queue* el = createQ(it->dest, heuristic[it->dest] + totalWeight, totalWeight);
                    add(&open, el);
                }
                else
                {
                    Queue** elem = fopen != nullptr ? fopen : fclosed;

                    if (totalWeight < (*elem)->distance)
                    {
                        (*elem)->distance = totalWeight;
                        if (fclosed)
                            changePriority(&closed, elem, totalWeight + heuristic[it->dest]);
                        else
                            changePriority(&open, elem, totalWeight + heuristic[it->dest]);

                        path[it->dest] = ch->node;

                        if (fclosed != nullptr)
                        {
                            Queue* tmp = *elem;
                            del(elem);
                            add(&open, tmp);
                        }
                    }
                }
                it = it->next;
            }
        }
    }
    return { vector<ListNode*>(), -1};
}

/**
 * \brief Algorytm A* z odleg³oœci¹ Euklidesow¹
 * \param g Graf
 * \param heuristic Funkcja heurystyczna
 * \param cords Tablica koordynatów
 * \param start Punkt startowy
 * \param goal Punkt koñcowy
 * \return Œcie¿ka wynikowa
 */
Path AStarE(const Graph* g, double (*heuristic)(const Coordinates &, const Coordinates&), const Coordinates *cords, const int start, const int goal)
{
    vector<int> path = vector<int>(g->vertices, -1);
    Queue* open = createQ(start, heuristic(cords[start], cords[goal]) / 1000, 0);
    Queue* closed = nullptr;

    //cout << start << ": " << heuristic(cords[start], cords[goal]) / 1000 << " -> " << goal << endl;

    path[start] = start;
    while (!emptyCheck(open))
    {
        Queue* ch = getCheapest(&open);
        if (ch->node == goal)
        {
            vector<int> endPath;
            int n = goal;

            while (path[n] != n)
            {
                endPath.push_back(n);
                n = path[n];
            }

            emptyQueue(&open);
            emptyQueue(&closed);

            endPath.push_back(start);
            reverse(endPath.begin(), endPath.end());

            return createPath(g, endPath);
        }
        else
        {
            ListNode* it = g->array[ch->node];

            if (closed == nullptr)
                closed = createQ(ch->node, ch->priority, ch->distance);
            else
                add(&closed, ch);

            while (it)
            {
                const int totalWeight = ch->distance + it->weight;
                Queue** fopen = findEl(&open, it->dest);
                Queue** fclosed = findEl(&closed, it->dest);
                if (fopen == nullptr && fclosed == nullptr)
                {
                    path[it->dest] = ch->node;
                    Queue* el = createQ(it->dest, heuristic(cords[it->dest], cords[goal]) / 1000 + totalWeight, totalWeight);
                    //cout << it->dest << ": " << heuristic(cords[it->dest], cords[goal]) / 1000 << " -> " << goal << endl;
                    add(&open, el);
                }
                else
                {
                    Queue** elem = fopen != nullptr ? fopen : fclosed;

                    if (totalWeight < (*elem)->distance)
                    {
                        (*elem)->distance = totalWeight;
                        if (fclosed)
                            changePriority(&closed, elem, totalWeight + heuristic(cords[it->dest], cords[goal]) / 1000);
                        else
                            changePriority(&open, elem, totalWeight + heuristic(cords[it->dest], cords[goal]) / 1000);
                        //cout << it->dest << ": " << heuristic(cords[it->dest], cords[goal]) / 1000 << " -> " << goal << endl;

                        path[it->dest] = ch->node;

                        if (fclosed != nullptr)
                        {
                            Queue* tmp = *elem;
                            del(elem);
                            add(&open, tmp);
                        }
                    }
                }
                it = it->next;
            }
        }
    }
    return { vector<ListNode*>(), -1 };
}

/**
 * \brief Rozwi¹zanie uk³adanki
 */
struct Solution
{
	/**
	 * \brief U³o¿enie
	 */
	vector<vector<int>> placement;

	/**
	 * \brief Iloœæ wykonanych ruchów
	 */
	int moves;
};

/**
 * \brief Pozycja pustego klocka
 */
struct Position
{
	/**
	 * \brief Wiersz
	 */
	int row;

	/**
	 * \brief Kolumna
	 */
	int column;
};

/**
 * \brief Funkcja heurystyczna sprawdzaj¹ca iloœæ Ÿle umieszczonych klocków
 * \param current Aktualny stan uk³adanki
 * \param goal Rozwi¹zanie
 * \return Iloœæ Ÿle usytuowanych klocków
 */
int hamming(const vector<vector<int>> &current, const vector<vector<int>> &goal)
{
    int h = 0;
    for (int i = 0; i < current.size(); ++i)
    {
	    for (int j = 0; j < current[0].size(); ++j)
	    {
            if (current[i][j] == 0)
                continue;
            if (current[i][j] != goal[i][j])
                h++;
	    }
    }
    return h;
}

/**
 * \brief Funkcja heurystyczna sprawdzaj¹ca odleg³oœæ od poprawnego miejsca
 * \param current Aktualny stan uk³adanki
 * \param goal Rozwi¹zanie
 * \return Odleg³oœæ od poprawnego miejsca
 */
int manhattan(const vector<vector<int>> &current, const vector<vector<int>> &goal)
{
    int distance = 0;
    const int size = current.size();
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (current[i][j] != 0)
                for (int k = 0; k < size; k++)
                    for (int l = 0; l < size; l++)
                        if (current[i][j] == goal[k][l])
                            distance += abs(i - k) + abs(j - l);
    return distance;
}

/**
 * \brief Sprawdzenie czy u³o¿enie jest rozwi¹zaniem
 * \param current Aktualny stan uk³adanki
 * \param goal Rozwi¹zanie
 * \return Informacja czy aktualny stan jest rozwi¹zaniem
 */
bool isGoal(const vector<vector<int>>& current, const vector<vector<int>>& goal)
{
	for (int i = 0; i < current.size(); ++i)
	{
        for (int j = 0; j < current[0].size(); ++j)
        {
            if (current[i][j] != goal[i][j])
                return false;
        }
	}
    return true;
}

/**
 * \brief Sprawdzenie wymiarów przekazanych uk³adanek
 * \param initial U³o¿enie pocz¹tkowe
 * \param goal Rozwi¹zanie
 * \return Informacja czy wymiary s¹ zgodne
 */
bool areEqual(const vector<vector<int>>& initial, const vector<vector<int>>& goal)
{
    bool equal = initial.size() == goal.size();
    if (!equal) return false;
    for (int i = 0; i < initial.size(); ++i)
    {
        if (initial[i].size() != goal[i].size() || initial[i].size() != initial.size())
            return false;
    }
    return true;
}

/**
 * \brief Sprawdzenie czy u³o¿enie by³o wczeœniej utworzone
 * \param visited Mapa odwiedzonych u³o¿eñ
 * \param current Aktualne u³o¿enie
 * \return Informacja czy u³o¿enie ju¿ wyst¹pi³o
 */
bool isVisited(map<vector<vector<int>>, bool>& visited, const vector<vector<int>>& current)
{
    return visited[current];
}

/**
 * \brief Sprawdzenie czy ruch pustego klocka jest dozwolony
 * \param dim Wymiar uk³adanki
 * \param x_pos Pozycja na osi x
 * \param y_pos Pozycja na osi y
 * \return Informacja czy ruch jest dozwolony
 */
bool isInBounds(const int dim, int x_pos, const int y_pos)
{
    return x_pos >= 0 && x_pos < dim && y_pos >= 0 && y_pos < dim;
}

/**
 * \brief Pobranie wektora stanów s¹siednich
 * \param current Stan aktualny
 * \param x_moveset Lista ruchów po osi x
 * \param y_moveset Lista ruchów po osi y
 * \return Wektor stanów s¹siednich
 */
vector<vector<vector<int>>> getNeighbors(vector<vector<int>> current, const vector<int> &x_moveset, const vector<int> &y_moveset)
{
    if (x_moveset.size() != y_moveset.size())
        throw "Niezgodne wymiary ruchow";

    Position n_pos{};
    for (int i = 0; i < current.size(); i++)
    {
        for (int j = 0; j < current[0].size(); j++)
        {
            if (current[i][j] == 0)
            {
                n_pos.row = i;
                n_pos.column = j;
                break;
            }
        }
    }

    vector<vector<vector<int>>> neighbors;

    for (int i = 0; i < x_moveset.size(); i++)
    {
	    const int x = n_pos.row;
	    const int y = n_pos.column;
        vector<vector<int>> vectors = current;
        if (isInBounds(current.size(), x + x_moveset[i], y + y_moveset[i]))
        {
            swap(vectors[x + x_moveset[i]][y + y_moveset[i]], vectors[x][y]);
            neighbors.push_back(vectors);
        }
    }

    return neighbors;
}

/**
 * \brief Stan przechowuj¹cy u³o¿enie i liczbê ruchów
 */
struct State
{
	/**
	 * \brief Aktualne po³o¿enie klocków
	 */
	vector<vector<int>> position;

	/**
	 * \brief Numer zag³êbienia
	 */
	int step;
};

/**
 * \brief Komparator stanów uk³adanki
 */
struct cmp
{
	/**
	 * \brief Rozwi¹zanie
	 */
	vector<vector<int>> goal;

	/**
	 * \brief Funkcja heurystyczna
	 */
	int (*heuristicFunction)(const vector<vector<int>>&, const vector<vector<int>>&);

	/**
	 * \brief Porównanie stanów
	 * \param a Pierwszy stan
	 * \param b Drugi stan
	 * \return Informacja czy stan pierwszy stan ma wiêcej b³êdnych pozycji klocków ni¿ stan drugi
	 */
	bool operator() (const State& a, const State &b) const
    {
	    const int a_estimation = heuristicFunction(a.position, goal) + a.step;
	    const int b_estimation = heuristicFunction(b.position, goal) + b.step;
        return a_estimation > b_estimation;
    }

	/**
	 * \brief Konstruktor komparatora
	 * \param goal Rozwi¹zanie uk³adanki
	 */
	cmp(const vector<vector<int>>& goal,
		int(* heuristic_function)(const vector<vector<int>>&, const vector<vector<int>>&))
		: goal(goal),
		  heuristicFunction(heuristic_function)
	{
	}
};


/**
 * \brief Wyœwietlenie historii uk³adanki
 * \param parent Mapa stanów poprzednich
 * \param s Stan koñcowy
 */
void printHistory(map<vector<vector<int>>, vector<vector<int>>> &parent, const vector<vector<int>> &s)
{
    if (parent.count(s))
        printHistory(parent, parent[s]);

    for (int i = 0; i < s.size(); i++)
    {
        for (int j = 0; j < s.size(); j++)
        {
            if (s[i][j] != 0)
                cout << s[i][j] << "\t";
            else
                cout << " \t";
        }
        cout << endl;
    }
    cout << endl;
}

/**
 * \brief Wyœwietlenie stanu koñcowego
 * \param s Rozwi¹zanie
 */
void printSolution(const Solution &s)
{
    for (int i = 0; i < s.placement.size(); i++)
    {
        for (int j = 0; j < s.placement.size(); j++)
        {
            if (s.placement[i][j] != 0)
                cout << s.placement[i][j] << "\t";
            else
                cout << " \t";
        }
        cout << endl;
    }
    cout << "Moves: " << s.moves;
    cout << endl;
}

/**
 * \brief Liczenie odwróceñ w wektorze
 * \param a Wektor ze sp³aszczonej matrycy
 * \return Iloœæ odwróceñ
 */
int countInversions(const vector<int> &a)
{
    int inv_count = 0;
    const int max = a.size();
    for (int i = 0; i < max - 1; i++)
        for (int j = i + 1; j < max; j++)
            if (a[j] && a[i] && a[i] > a[j])
                inv_count++;
    return inv_count;
}

/**
 * \brief Sprawdzenie czy uk³adanka ma rozwi¹zanie
 * \param a U³o¿enie
 * \return Parzystoœæ odwróceñ
 */
bool parity(const vector<vector<int>>& a, const vector<vector<int>>& b)
{
    Position a0;
    Position b0;
    vector<int> a_flatten;
    vector<int> b_flatten;
    for (int i = 0; i < a.size(); ++i)
        for (int j = 0; j < a.size(); ++j)
        {
            if (a[i][j] == 0)
                a0 = { i, j };
            if (b[i][j] == 0)
                b0 = { i, j };
            a_flatten.push_back(a[i][j]);
            b_flatten.push_back(b[i][j]);
        }

    const int a_inversions = countInversions(a_flatten);
    const int b_inversions = countInversions(b_flatten);

    if (a.size() % 2 == 0)
    {
        if (a0.row % 2 == b0.row % 2)
            return a_inversions % 2 == b_inversions % 2;
        return a_inversions % 2 != b_inversions % 2;
    }
    return a_inversions % 2 == b_inversions % 2;
}

/**
 * \brief Algorytm A* dla uk³adanek
 * \param initial Pozycja pocz¹tkowa uk³adanki
 * \param goal Pozycja koñcowa uk³adanki
 * \param heuristicFunction Funkcja heurystyczna
 * \return Rozwi¹zanie
 */
Solution AStar8(const vector<vector<int>>& initial, const vector<vector<int>>& goal, int (*heuristicFunction)(const vector<vector<int>>&, const vector<vector<int>>&))
{
    if (!areEqual(initial, goal))
        throw "Zle wymiary poczatku i konca";

    if (!parity(initial, goal))
        throw "Przypadek bez rozwiazan";

    const vector<int> x = { 1,-1,0,0 };
    const vector<int> y = { 0,0,1,-1 };

    map<vector<vector<int>>, bool> visited;
    map<vector<vector<int>>, vector<vector<int>>> parent;

    const cmp stateCompare(goal, heuristicFunction);
    priority_queue<State, vector<State>, cmp> queue(stateCompare);
    queue.push(State{ initial, 0 });
    while (!queue.empty())
    {
        vector<vector<int>> cheapest = queue.top().position;
        const int step = queue.top().step;
        queue.pop();
        visited[cheapest] = true;
        if (cheapest == goal)
        {
            printHistory(parent, cheapest);
            return { cheapest, step };
        }
        vector<vector<vector<int>>> neighbors = getNeighbors(cheapest, x ,y);
        vector<vector<vector<int>>>::iterator it;
        for (it = neighbors.begin(); it != neighbors.end(); ++it)
        {
            vector<vector<int>> tmp = *it;
            if (!isVisited(visited, tmp))
            {
                parent.insert(pair<vector<vector<int>>, vector<vector<int>>>(tmp, cheapest));
                queue.push(State{ tmp, step + 1 });
            }
        }
    }
    return { vector<vector<int>>(), -1 };
}

int main()
{
	int V = 6;
	bool directed = true;
    const string labels[] = { "S", "A", "B", "C", "D", "G" };
	Graph* g = makeGraph(V, directed, true, labels);

	addEdge(g, 0, 1, 1); // S -> A
	addEdge(g, 0, 5, 10); // S -> G
	addEdge(g, 1, 2, 2); // A -> B
	addEdge(g, 1, 3, 1); // A -> C
	addEdge(g, 2, 4, 5); // B -> D
	addEdge(g, 3, 4, 3); // C -> D
	addEdge(g, 3, 5, 4); // C -> G
	addEdge(g, 4, 5, 2); // D -> G

    const int heuristic[] = { 5, 3, 4, 2, 6, 0 };

    Path p = AStarF(g, heuristic, 0, 5);
    printPath(p, true);

    //int V = 7;
    //bool directed = true;
    //Graph* g = makeGraph(V, directed);

    //addEdge(g, 0, 3, 2);
    //addEdge(g, 0, 2, 5);
    //addEdge(g, 0, 1, 1);
    //addEdge(g, 1, 4, 7);
    //addEdge(g, 2, 5, 4);
    //addEdge(g, 3, 2, 1);
    //addEdge(g, 3, 4, 6);
    //addEdge(g, 4, 6, 3);
    //addEdge(g, 5, 6, 3);
    //addEdge(g, 5, 4, 1);

    //int* heuristic = new int[] { 3, 2, 2, 2, 1, 1, 0 };

    //Path p = AStarF(g, heuristic, 0, 6);

	//const int V = 5;
	//const bool directed = true;
 //   Graph* g = makeGraph(V, directed);
 //   addEdge(g, 0, 2, 4);
 //   addEdge(g, 0, 1, 1);
 //   addEdge(g, 1, 4, 12);
 //   addEdge(g, 1, 3, 5);
 //   addEdge(g, 1, 2, 2);
 //   addEdge(g, 2, 3, 2);
 //   addEdge(g, 3, 4, 3);

	//const int* heuristic = new int[] { 7, 6, 2, 1, 0 };
	//const Path p = AStarF(g, heuristic, 0, 4);


    //const int V = 10;
    //const bool directed = false;
    //const string labels[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"};
    //Graph* g = makeGraph(V, directed, true, labels);
    //addEdge(g, 0, 1, 6);
    //addEdge(g, 0, 5, 3);
    //addEdge(g, 1, 2, 3);
    //addEdge(g, 1, 3, 2);
    //addEdge(g, 2, 3, 1);
    //addEdge(g, 2, 4, 5);
    //addEdge(g, 3, 4, 8);
    //addEdge(g, 4, 9, 5);
    //addEdge(g, 4, 8, 5);
    //addEdge(g, 9, 8, 3);
    //addEdge(g, 8, 6, 3);
    //addEdge(g, 8, 7, 2);
    //addEdge(g, 6, 5, 1);
    //addEdge(g, 7, 5, 7);

    //const int* heuristic = new int[] { 10, 8, 5, 7, 3, 6, 5, 3, 1, 0 };
    //const Path p = AStarF(g, heuristic, 0, 9);

    //printPath(p, true);

 //   const int V = 14;
 //   const bool directed = false;
 //   const string labels[] = {"Gd", "Sz", "Ol", "By", "Wa", "Zg", "Po", "L", "Ki", "Lu", "Bi", "Wr", "Ka", "Kr"};
 //   Graph* g = makeGraph(V, directed, true, labels);


 //   //Gd 0
 //   addEdge(g, 0, 1, 362);
 //   addEdge(g, 0, 2, 170);
 //   addEdge(g, 0, 3, 165);
 //   addEdge(g, 0, 4, 377);

 //   //Sz 1
 //   addEdge(g, 1, 5, 217);
 //   addEdge(g, 1, 6, 235);

 //   //Ol 2
 //   addEdge(g, 2, 4, 212);

 //   //By 3
 //   addEdge(g, 3, 4, 270);
 //   addEdge(g, 3, 7, 227);

 //   //Wa 4
 //   addEdge(g, 4, 6, 319);
 //   addEdge(g, 4, 7, 137);
 //   addEdge(g, 4, 8, 178);
 //   addEdge(g, 4, 9, 166);
 //   addEdge(g, 4, 10, 193);

 //   //Zg 5
 //   addEdge(g, 5, 6, 152);
 //   addEdge(g, 5, 11, 156);

 //   //Po 6
 //   addEdge(g, 6, 11, 186);
 //   addEdge(g, 6, 7, 203);

 //   //L 7
 //   addEdge(g, 7, 11, 209);
 //   addEdge(g, 7, 12, 193);

 //   //Ki 8
 //   addEdge(g, 8, 13, 116);

 //   //Lu 9
 //   //Bi 10
 //   //Wr 11
 //   addEdge(g, 11, 12, 195);

 //   //Ka 12
 //   addEdge(g, 12, 13, 80);

 //   const int heuristics[] = { 546, 600, 468, 408, 282, 409, 378, 217, 115, 259, 470, 254, 75, 0 };

	//constexpr Coordinates cords[] = { {54.35205, 18.64637}, {53.42494, 14.55302}, {53.77995, 20.49416},
 //   {53.1235, 18.00762}, {52.22977, 21.01178}, {51.93548, 15.50643}, {52.40692, 16.92993},
 //   {51.75, 19.46667}, {50.87033, 20.62752}, {51.25, 22.56667}, {53.13333, 23.16433},
 //   {51.1, 17.03333}, {50.25841, 19.02754}, {50.06143, 19.93658} };

 //   const Path p = AStarF(g, heuristics, 0, 13);
 //   const Path p1 = AStarE(g, haversine, cords, 0, 13);
 //   printPath(p, true);
 //   printPath(p1, true);


    //try
    //{
    //    //const vector<vector<int>> init = { {0,8,7},{1,5,3},{6,4,2} };
    //    //const vector<vector<int>> goal = { {1,2,3},{4,5,6},{7,8,0} };

    //    //const vector<vector<int>> init = { {2,0,3},{6,5,8},{1,4,7} };
    //    //const vector<vector<int>> goal = { {1,2,3},{4,5,6},{7,8,0} };

    //    //const vector<vector<int>> init = { {1,2,3},{4,5,6},{8,7,0} };
    //    //const vector<vector<int>> goal = { {1,2,3},{8,0,4},{7,6,5} };

    //    //const vector<vector<int>> init = { {5,6,7},{4,0,8},{3,2,1} };
    //    //const vector<vector<int>> goal = { {1,2,3},{8,0,4},{7,6,5} };

    //    const vector<vector<int>> init = { {6,9,5,1},{14,0,10,11},{2,4,3,7},{15,12,13,8} };
    //    //const vector<vector<int>> init = { {1,2,3,4},{5,6,7,8},{9,10,11,12},{13,15,14,0} };
    //    //const vector<vector<int>> goal = { {1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,0} };
    //    const vector<vector<int>> goal = { {1,0,4,3},{5,10,7,8},{9,6,15,2},{13,14,12,11} };



    //    const Solution s = AStar8(init, goal, manhattan);
    //    printSolution(s);
    //} catch (const char * err)
    //{
    //    cout << err << endl;
    //}
}