#include <iostream>

using namespace std;

struct ListNode {
    int dest;
    int src;
    int weight;
    ListNode* next;
};

struct List {
    ListNode* head;
};

struct Graph {
    int vertices;
    bool directed;
    List* array;
};

struct Result 
{
    ListNode** nodes;
    int distance;
};

ListNode* makeListNode(int src, int dest, int weight) {
    ListNode* l = (ListNode *)malloc(sizeof(ListNode));
    l->src = src;
    l->dest = dest;
    l->weight = weight;
    l->next = NULL;
    return l;
}

Graph* makeGraph(int vertices, int directed) {
    int i;
    Graph* g = (Graph *)malloc(sizeof(Graph));
    g->vertices = vertices;
    g->directed = directed;
    g->array = (List *)malloc(sizeof(List) * vertices);

    for (i = 0; i < vertices; i++) {
        g->array[i].head = NULL;
    }
    return g;
}

void addEdge(Graph* g, int src, int dest, int weight) {
    ListNode* node = makeListNode(src, dest, weight);
    node->next = g->array[src].head;
    g->array[src].head = node;

    if (!g->directed) {
        node = makeListNode(dest, src, weight);
        node->next = g->array[dest].head;
        g->array[dest].head = node;
    }
}

Result nearestNeighbor(Graph *g)
{
    Result r;
    ListNode **output = (ListNode **)calloc(g->vertices, sizeof(ListNode *));
    ListNode **partialOutput = (ListNode **)calloc(g->vertices, sizeof(ListNode *));
    int minSum = INT_MAX;
    bool *visited = (bool *)calloc(g->vertices, sizeof(bool));
    for (int i = 0; i < g->vertices; ++i)
	{
        visited[i] = true;
        int sum = 0;
        ListNode *el = g->array[i].head;

        int it = 0;
        bool looped = true;

        for (int j = 0; j < g->vertices - 1; ++j)
        {
            ListNode* candidate;
            int weight = INT_MAX;
            while (el)
            {
                if (!visited[el->dest] && el->weight < weight)
                {
                    weight = el->weight;
                    candidate = el;
                }
                el = el->next;
            }

            if (weight == INT_MAX)
                looped = false;
            
            visited[candidate->dest] = true;
            sum += weight;
            el = g->array[candidate->dest].head;
            partialOutput[it++] = candidate;
        }
        bool found = false;
        while (looped && el)
        {
            if (el->dest == i)
            {
                found = true;
                sum += el->weight;
                partialOutput[it++] = el;
            }
            el = el->next;
        }
        if (looped && found && sum < minSum)
        {
            memcpy(output, partialOutput, g->vertices * sizeof(ListNode *));
            minSum = sum;
        }

        fill_n(visited, g->vertices, false);
        fill_n(partialOutput, g->vertices, nullptr);
    }
    free(partialOutput);
    free(visited);
    r.nodes = output;
    r.distance = minSum;
    return r;
}

void printSolution(int vertices, Result res)
{
    for (int i = 0; i < vertices; ++i)
    {
        cout << res.nodes[i]->src << " -> " << res.nodes[i]->dest << " : " << res.nodes[i]->weight << endl;
    }
    cout << "Distance: " << res.distance << endl;
}

int main()
{
    int V = 7;
    int directed = false;
    Graph* graph = makeGraph(V, directed);

    addEdge(graph, 0, 1, 10);
    addEdge(graph, 0, 3, 4);
    addEdge(graph, 0, 4, 1);
    addEdge(graph, 1, 2, 1);
    addEdge(graph, 1, 4, 3);
    addEdge(graph, 1, 5, 15);
    addEdge(graph, 2, 3, 1);
    addEdge(graph, 2, 4, 2);
    addEdge(graph, 3, 4, 3);
    addEdge(graph, 3, 5, 7);
    addEdge(graph, 3, 6, 5);
    addEdge(graph, 5, 6, 6);

    // addEdge(graph, 0, 1, 5);
    // // addEdge(graph, 0, 2, 26);
    // addEdge(graph, 0, 3, 4);
    // addEdge(graph, 0, 4, 2);
    // addEdge(graph, 1, 2, 7);
    // // addEdge(graph, 1, 3, 21);
    // addEdge(graph, 1, 4, 3);
    // addEdge(graph, 2, 3, 6);
    // addEdge(graph, 2, 4, 2);
    // addEdge(graph, 3, 4, 1);
    
    Result res = nearestNeighbor(graph);
    
    printSolution(graph->vertices, res);
    
    free(graph);
    free(res.nodes);
}