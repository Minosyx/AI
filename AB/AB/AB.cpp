#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

#define WIN 1000
#define	DRAW 0
#define LOSS -1000

#define P2 'O'
#define P1 'X'
#define EMPTY '-'

#define START_DEPTH 0

using namespace std;

/**
 * \brief Wêze³
 */
struct Node
{
	/**
	 * \brief Etykieta
	 */
	string label;

	/**
	 * \brief Wartoœæ
	 */
	int score;

	/**
	 * \brief Komparator dla mapy
	 * \param other Inny wêze³
	 * \return Czy aktualny jest mniejszy od drugiego
	 */
	bool operator<(const Node &other) const
    {
        return (*this).label < other.label;
    }
};

/// <summary>
/// KrawêdŸ grafu
/// </summary>
struct Edge
{
	/**
	 * \brief Odleg³oœæ
	 */
	int distance;

	/**
	 * \brief Wêze³ docelowy
	 */
	Node* destination;
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

    /**
     * \brief Mapa wêz³ów na wektor krawêdzi
     */
    map<Node, vector<Edge>> node_map;
};

/**
 * \brief Tworzenie wêz³a
 * \param label Etykieta
 * \param score Wartoœæ
 * \return Wêze³
 */
Node makeNode(const string &label, const int score = 0)
{
    return { label, score };
}

/**
 * \brief Dodanie krawêdzi
 * \param g Graf
 * \param src Wêze³ Ÿród³owy
 * \param dest Wêze³ docelowy
 * \param distance Odleg³oœæ
 */
void addEdge(Graph &g, Node *src, Node *dest, const int distance = 0)
{
	const Edge e { distance, dest };
	const auto it = g.node_map.find(*src);
	if (it == g.node_map.end())
    {
	    vector<Edge> ve;
	    ve.push_back(e);
        g.node_map[*src] = ve;
    }
    else
        g.node_map[*src].push_back(e);

    if (!g.directed)
    {
        const Edge re{ distance, src };
        const auto rit = g.node_map.find(*dest);
        if (it == g.node_map.end())
        {
            vector<Edge> ve;
            ve.push_back(re);
            g.node_map[*dest] = ve;
        }
        else
            g.node_map[*dest].push_back(re);
    }
}


/// <summary>
/// Tworzenie grafu
/// </summary>
/// <param name="vertices">Iloœæ wierzcho³ków</param>
/// <param name="directed">Czy graf jest skierowany</param>
/// <returns>Graf</returns>
Graph makeGraph(const int vertices, const bool directed)
{
    return {vertices, directed};
}

/**
 * \brief Algorytm alfa-beta
 * \param g Graf
 * \param node Wêze³
 * \param depth G³êbokoœæ
 * \param alpha Wartoœæ alfa
 * \param beta Wartoœæ beta
 * \param maxPlayer Gracz maksymalizuj¹cy
 * \return Wartoœæ koñcowa
 */
int AB(Graph &g, Node *node, const int depth, int alpha, int beta, bool maxPlayer)
{
    if (depth == 0 || !g.node_map.contains(*node))
        return (* node).score;

    //for (int i = 0; i < depth; ++i)
    //    cout << "\t";
    //cout << (*node).label << ": " << beta << endl;

    if (maxPlayer)
    {
	    const vector<Edge> e = g.node_map[*node];
        for (Edge ns : e)
        {
            int tmp = AB(g, ns.destination, depth - 1, alpha, beta, !maxPlayer);
            alpha = max(alpha, tmp);

			auto n = g.node_map.extract(*node);
            (*node).score = alpha;
            n.key() = *node;
            g.node_map.insert(move(n));

            if (alpha >= beta)
                break;
	    }
        return alpha;
    }
    const vector<Edge> e = g.node_map[*node];
    Node* N;
    pair<int, Node*> tmp;
    for (Edge ns : e)
    {
        int tmp = AB(g, ns.destination, depth - 1, alpha, beta, !maxPlayer);
        beta = min(beta, tmp);

        auto n = g.node_map.extract(*node);
        (*node).score = beta;
        n.key() = *node;
        g.node_map.insert(move(n));

	    if (alpha >= beta)
		    break;
    }
    return beta;
}

/**
 * \brief Wyœwietl planszê
 * \param board Plansza
 */
void printBoard(const vector<vector<char>>& board)
{
    cout << endl;
    for (int i = 0; i < board.size(); ++i)
    {
	    for (int j = 0; j < board.size(); ++j)
	    {
            cout << board[i][j];
            if (j < board.size() - 1)
                cout << " | ";
	    }
        cout << endl << "----------" << endl;
    }
}

/**
 * \brief Wyœwietl wynik gry
 * \param state Stan gry
 */
void printOutcome(const int state)
{
    if (WIN == state) { cout << "WYGRANA" << endl; }
    else if (DRAW == state) { cout << "REMIS" << endl; }
    else if (LOSS == state) { cout << "PRZEGRANA" << endl; }
}

/**
 * \brief Wygrywaj¹ce u³o¿enia
 */
vector<vector<pair<int, int>>> winStates
{
    { make_pair(0, 0), make_pair(0, 1), make_pair(0, 2) },
    { make_pair(1, 0), make_pair(1, 1), make_pair(1, 2) },
    { make_pair(2, 0), make_pair(2, 1), make_pair(2, 2) },

    { make_pair(0, 0), make_pair(1, 0), make_pair(2, 0) },
    { make_pair(0, 1), make_pair(1, 1), make_pair(2, 1) },
    { make_pair(0, 2), make_pair(1, 2), make_pair(2, 2) },

    { make_pair(0, 0), make_pair(1, 1), make_pair(2, 2) },
    { make_pair(2, 0), make_pair(1, 1), make_pair(0, 2) }
};

/**
 * \brief Pobiera dostêpne ruchy
 * \param board Plansza
 * \return Dostêpne ruchy
 */
vector<pair<int, int>> getMoves(const vector<vector<char>> &board)
{
    vector<pair<int, int>> legalMoves;
    for (int i = 0; i < board.size(); i++)
        for (int j = 0; j < board.size(); j++)
            if (board[i][j] != P2 && board[i][j] != P1)
                legalMoves.emplace_back(i, j);

    return legalMoves;
}

/**
 * \brief Sprawdzenie czy miejsce na planszy zajête
 * \param board Plansza
 * \param pos Pozycja
 * \return Czy miejsce zajête
 */
bool isOccupied(const vector<vector<char>> &board, const pair<int, int> pos)
{
	const vector<pair<int, int>> legalMoves = getMoves(board);

    for (const auto& legalMove : legalMoves)
        if (pos.first == legalMove.first && pos.second == legalMove.second)
            return false;

    return true;
}

/**
 * \brief Pobiera zajête pozycje
 * \param board Plansza
 * \param symbol Symbol gracza
 * \return Wektor zajêtych pozycji
 */
vector<pair<int, int>> getOccupied(const vector<vector<char>> &board, const char symbol)
{
    vector<pair<int, int>> occupiedPositions;

    for (int i = 0; i < board.size(); i++)
        for (int j = 0; j < board.size(); j++)
            if (symbol == board[i][j])
            	occupiedPositions.emplace_back(i, j);

    return occupiedPositions;
}

/**
 * \brief Sprawdzenie czy plansza pe³na
 * \param board Plansza
 * \return Czy plansza pe³na
 */
bool isBoardFull(const vector<vector<char>> &board)
{
	const vector<pair<int, int>> legalMoves = getMoves(board);

    if (legalMoves.empty())
        return true;
	return false;
}

/**
 * \brief Sprawdzenie czy gra jest wygrana
 * \param occupiedPositions Zajête pozycje
 * \return Czy gra wygrana
 */
bool isGameWon(const vector<pair<int, int>> &occupiedPositions)
{
    bool gameWon;

    for (auto winState : winStates)
    {
        gameWon = true;
        for (int j = 0; j < 3; j++)
        {
            if (!(find(begin(occupiedPositions), end(occupiedPositions), winState[j]) != end(occupiedPositions)))
            {
                gameWon = false;
                break;
            }
        }

        if (gameWon)
            break;
    }
    return gameWon;
}


/**
 * \brief Zmienia aktualny symbol na przeciwnika
 * \param symbol Aktualny symbol
 * \return Symbol przeciwnika
 */
char getOpponentSymbol(const char symbol)
{
    char opponentSymbol;
    if (symbol == P1)
        opponentSymbol = P2;
    else
        opponentSymbol = P1;

    return opponentSymbol;
}

/**
 * \brief Zwraca stan gry
 * \param board Plansza
 * \param symbol Symbol gracza
 * \return Stan gry
 */
int getBoardState(const vector<vector<char>> &board, const char symbol)
{
	const char opponentSymbol = getOpponentSymbol(symbol);

    vector<pair<int, int>> occupiedPositions = getOccupied(board, symbol);

    const bool isWon = isGameWon(occupiedPositions);

    if (isWon)
        return WIN;

    occupiedPositions = getOccupied(board, opponentSymbol);
    const bool isLost = isGameWon(occupiedPositions);

    if (isLost)
        return LOSS;

    const bool isFull = isBoardFull(board);
    if (isFull)
        return DRAW;
    return DRAW;

}

/**
 * \brief Algorytm alfa-beta dla gry kó³ko-krzy¿yk
 * \param board Plansza
 * \param symbol Symbol gracza
 * \param depth G³êbokoœæ
 * \param alpha Alfa
 * \param beta Beta
 * \return Para wynik-ruch
 */
pair<int, pair<int, int>> ABttt(vector<vector<char>> board, const char symbol, const int depth, int alpha, int beta)
{
    pair<int, int> bestMove(-1, -1);
    int bestScore = (symbol == P2) ? LOSS : WIN;

    if (isBoardFull(board) || DRAW != getBoardState(board, P2))
    {
        bestScore = getBoardState(board, P2);
        return make_pair(bestScore, bestMove);
    }

    const vector<pair<int, int>> legalMoves = getMoves(board);

    for (const auto &currMove : legalMoves)
    {
	    board[currMove.first][currMove.second] = symbol;
        if (symbol == P2)
        {
	        const int score = ABttt(board, P1, depth + 1, alpha, beta).first;
            if (bestScore < score)
            {
                bestScore = score - depth * 10;
                bestMove = currMove;

                alpha = max(alpha, bestScore);
                board[currMove.first][currMove.second] = EMPTY;
                if (alpha >= beta)
                    break;
            }

        }
        else
        {
	        const int score = ABttt(board, P2, depth + 1, alpha, beta).first;
            if (bestScore > score)
            {
                bestScore = score + depth * 10;
                bestMove = currMove;

                beta = min(beta, bestScore);
                board[currMove.first][currMove.second] = EMPTY;
                if (alpha >= beta)
                    break;
            }

        }
        board[currMove.first][currMove.second] = EMPTY;
    }

    return make_pair(bestScore, bestMove);
}

/**
 * \brief Sprawdzenie czy gra zakoñczona
 * \param board Plansza
 * \return Czy gra zakoñczona
 */
bool isFinished(const vector<vector<char>> &board)
{
    if (isBoardFull(board))
        return true;

    if (DRAW != getBoardState(board, P2))
        return true;

    return false;
}

int main()
{
    //Graph graph = makeGraph(15, true);
    //Node a = makeNode("A", 0);
    //Node b = makeNode("B", 0);
    //Node c = makeNode("C", 0);
    //Node d = makeNode("D", 0);
    //Node e = makeNode("E", 0);
    //Node f = makeNode("F", 0);
    //Node g = makeNode("G", 0);
    //Node h = makeNode("H", 3);
    //Node i = makeNode("I", 5);
    //Node j = makeNode("J", 6);
    //Node k = makeNode("K", 9);
    //Node l = makeNode("L", 1);
    //Node m = makeNode("M", 2);
    //Node n = makeNode("N", 0);
    //Node o = makeNode("O", -1);

    //addEdge(graph, a, b, 0);
    //addEdge(graph, a, c, 0);

    //addEdge(graph, b, d, 0);
    //addEdge(graph, b, e, 0);
    //addEdge(graph, c, f, 0);
    //addEdge(graph, c, g, 0);

    //addEdge(graph, d, h, 0);
    //addEdge(graph, d, i, 0);

    //addEdge(graph, e, j, 0);
    //addEdge(graph, e, k, 0);

    //addEdge(graph, f, l, 0);
    //addEdge(graph, f, m, 0);

    //addEdge(graph, g, n, 0);
    //addEdge(graph, g, o, 0);

    //cout << AB(graph, a, 3, INT_MIN, INT_MAX, true) << endl;

    //Graph g = makeGraph(22, true);
    //vector<Node> nodes;
    //nodes.reserve(22);
    //for (int i = 0; i < 22; ++i)
    //{
    //    string name(1, 65 + i);
    //    nodes.push_back(makeNode(name));
    //}

    //nodes[10].score = 4;
    //nodes[11].score = 6;
    //nodes[12].score = 7;
    //nodes[13].score = 9;
    //nodes[14].score = 1;
    //nodes[15].score = 2;
    //nodes[16].score = 0;
    //nodes[17].score = 1;
    //nodes[18].score = 8;
    //nodes[19].score = 1;
    //nodes[20].score = 9;
    //nodes[21].score = 2;

    //addEdge(g, &nodes[0], &nodes[1]);
    //addEdge(g, &nodes[0], &nodes[2]);
    //addEdge(g, &nodes[0], &nodes[3]);

    //int j = 4;
    //for (int i = 1; j < 22; ++i)
    //{
	   // for (int k = 0; k < 2 && j < 22; ++j, ++k)
	   // {
    //        addEdge(g, &nodes[i], &nodes[j]);
	   // }
    //}

    //int out = AB(g, &nodes[8], 3, INT_MIN, INT_MAX, true);
    //cout << out << endl;

    vector board(3, vector(3, EMPTY));
    cout << "Gracz = X\t Komputer = O" << endl << endl;

    printBoard(board);

    while (!isFinished(board))
    {
        int row, col;
        cout << "Wiersz: ";
        cin >> row;
        cout << "Kolumna: ";
        cin >> col;
        cout << endl << endl;

        if (isOccupied(board, make_pair(row, col)))
        {
            cout << "Pole (" << row << ", " << col << ") jest zajete. Sprobuj innego..." << endl;
            continue;
        }
        board[row][col] = P1;

        const pair<int, pair<int, int>> aiMove = ABttt(board, P2, START_DEPTH, LOSS, WIN);

        if (aiMove.second != pair(-1, -1))
			board[aiMove.second.first][aiMove.second.second] = P2;

        printBoard(board);
    }

    cout << "Koniec gry" << endl << endl;

    const int playerState = getBoardState(board, P1);

    cout << "Gracz: "; printOutcome(playerState);
}
