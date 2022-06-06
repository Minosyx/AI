#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

struct ListNode
{
    int dest;
    int weight;
    ListNode* next;
};

struct List
{
    ListNode* head;
};

struct Graph
{
    int vertices;
    List* array;
};

ListNode* makeListNode(int dest, int weight)
{
    ListNode* l = (ListNode*)malloc(sizeof(ListNode));
    l->dest = dest;
    l->weight = weight;
    l->next = NULL;
    return l;
}

Graph* makeGraph(int vertices)
{
    int i;
    Graph* g = (ListNode*)(sizeof(Graph));
    g->vertices = vertices;
    g->array = malloc(sizeof(List) * vertices);

    for (i = 0; i < vertices; i++)
    {
        g->array[i].head = NULL;
    }
    return g;
}

void addEdge(Graph* g, int src, int dest, int weight)
{
    ListNode* node = makeListNode(dest, weight);
    node->next = g->array[src].head;
    g->array[src].head = node;

    node = makeListNode(src, weight);
    node->next = g->array[dest].head;
    g->array[dest].head = node;
}

struct MinHeapNode
{
    int vertices;
    int dist;
};

struct MinHeap
{
    int size;
    int capacity;

    int* position;
    struct MinHeapNode** array;
};

MinHeapNode* makeMinHeapNode(int vertices, int dist)
{
    MinHeapNode* mhp = (MinHeapNode*)malloc(sizeof(MinHeapNode));
    mhp->vertices = vertices;
    mhp->dist = dist;
    return mhp;
}

MinHeap* makeMinHeap(int capacity)
{
    MinHeap* mh = (MinHeapNode*)malloc(sizeof(MinHeap));
    mh->position = malloc(capacity * sizeof(int));
    mh->size = 0;
    mh->capacity = capacity;
    mh->array = malloc(capacity * sizeof(MinHeapNode*));
    return mh;
}

void swap(MinHeapNode** x, MinHeapNode** y)
{
    MinHeapNode* tmp = *x;
    *x = *y;
    *y = tmp;
}

void minHeapify(MinHeap* mh, int idx)
{
    int smallest, left, right;
    smallest = idx;
    left = 2 * idx + 1;
    right = 2 * idx + 2;

    if (left < mh->size && mh->array[left]->dist < mh->array[smallest]->dist)
        smallest = left;

    if (right < mh->size && mh->array[right]->dist < mh->array[smallest]->dist)
        smallest = right;

    if (smallest != idx)
    {
        MinHeapNode* smallestNode = mh->array[smallest];
        MinHeapNode* idxNode = mh->array[idx];
        mh->position[smallestNode->vertices] = idx;
        mh->position[idxNode->vertices] = smallest;
        swap(&mh->array[smallest], &mh->array[idx]);
        minHeapify(mh, smallest);
    }
}

int isEmpty(MinHeap* mh)
{
    return mh->size == 0;
}

MinHeapNode* extractMin(MinHeap* mh)
{
    if (isEmpty(mh)) return NULL;

    MinHeapNode* root = mh->array[0];
    MinHeapNode* last = mh->array[mh->size - 1];
    mh->array[0] = last;

    mh->position[root->vertices] = mh->size - 1;
    mh->position[last->vertices] = 0;

    --mh->size;
    minHeapify(mh, 0);

    return root;
}

void decrKey(MinHeap* mh, int vertices, int dist)
{
    int i = mh->position[vertices];

    mh->array[i]->dist = dist;
    while (i && mh->array[i]->dist < mh->array[(i - 1) / 2]->dist)
    {
        mh->position[mh->array[i]->vertices] = (i - 1) / 2;
        mh->position[mh->array[(i - 1) / 2]->vertices] = i;
        swap(&mh->array[i], &mh->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

int isInMinHeap(MinHeap* mh, int vertices)
{
    if (mh->position[vertices] < mh->size)
        return 1;
    return 0;
}

void printPath(int* parent, int j)
{
    if (parent[j] == -1)
        return;

    printPath(parent, parent[j]);

    printf("%d ", j);
}

int printSolution(int* dist, int v, int* parent, int src)
{
    printf("Vertex\t Distance\tPath");
    for (int i = 0; i < v; i++)
    {
        printf("\n%d -> %d \t\t %d\t\t%d ", src, i, dist[i], src);
        printPath(parent, i);
    }
}

void dijkstra(Graph* g, int src)
{
    int vertices = g->vertices;
    int* dist =(int*) malloc(vertices * sizeof(int));
    int v;
    int* parent = (int*)malloc(vertices * sizeof(int));

    MinHeap* mh = makeMinHeap(vertices);

    for (v = 0; v < vertices; v++)
    {
        parent[v] = -1;
        dist[v] = INT_MAX;
        mh->array[v] = makeMinHeapNode(v, dist[v]);
        mh->position[v] = v;
    }

    // mh->array[src] = makeMinHeapNode(src, dist[src]);
    // mh->position[src] = src;
    dist[src] = 0;
    decrKey(mh, src, dist[src]);

    mh->size = v;

    while (!isEmpty(mh))
    {
        MinHeapNode* mhn = extractMin(mh);
        int tmp = mhn->vertices;

        ListNode* it = g->array[tmp].head;

        while (it != NULL)
        {
            v = it->dest;
            if (isInMinHeap(mh, v) && dist[tmp] != INT_MAX && it->weight + dist[tmp] < dist[v])
            {
                parent[v] = tmp;
                dist[v] = dist[tmp] + it->weight;
                decrKey(mh, v, dist[v]);
            }
            it = it->next;
        }
    }
    printSolution(dist, vertices, parent, src);
}

int main()
{
    int V = 7;
    Graph* graph = makeGraph(V);

    addEdge(graph, 0, 6, 5);
    addEdge(graph, 6, 4, 12);
    addEdge(graph, 6, 1, 2);
    addEdge(graph, 6, 3, 9);
    addEdge(graph, 1, 4, 8);
    addEdge(graph, 3, 5, 6);
    addEdge(graph, 4, 2, 3);

    dijkstra(graph, 5);
}