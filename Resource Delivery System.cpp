#include <iostream>
#include <ctime>
#include <climits> // Include for INT32_MAX using namespace std;
#include<string>
using namespace std;

#define NUM_ASSETS 11

struct Location
{
    int Id;
    string Name, Address;
    string Type;
} Locations[NUM_ASSETS] = {
    {0, "MAX", "Sector 63", "h"},
    {1, "APEX", "Sector 33", "h"},
    {2, "Jaypee", "Sector 128", "h"},
    {3, "Vivekananda", "Sector 19", "h"},
    {4, "Yatharth", "Sector 110", "h"},
    {5, "NMC", "Sector 30", "h"},
    {6, "Cosmos", "Sector 27", "h"},
    {7, "Apollo", "Sector 26", "h"},
    {8, "Nishchay", " Sector 4", " W "},
    {9, "Nitesh", "Sector 62", " W "},
    {10, "Narwani", "Sector 69", "w"}};

struct RoadNode
{
    int Destination;
    Location *Info;
    RoadNode *Next;
    int Traffic;
};

struct Connection
{
    int Source, Destination, Traffic;
};

class DeliveryGraph
{
public:
    RoadNode **Head;
    int NumLocations;

    RoadNode *GetRoadNode(int Destination, RoadNode *Head, int Traffic)
    {
        RoadNode *newNode = new RoadNode;
        newNode->Destination = Destination;
        newNode->Next = Head;
        newNode->Traffic = Traffic;
        newNode->Info = &Locations[Destination];
        return newNode;
    }

public:
    DeliveryGraph(Connection Connections[], int numConnections, int numLocations)
    {
        Head = new RoadNode *[numLocations]();
        this->NumLocations = numLocations;

        for (int i = 0; i < numLocations; i++)
        {
            Head[i] = nullptr;
        }

        for (unsigned i = 0; i < numConnections; i++)
        {
            int Source = Connections[i].Source;
            int Destination = Connections[i].Destination;
            int Traffic = Connections[i].Traffic;

            RoadNode *newNode = GetRoadNode(Destination, Head[Source], Traffic);
            Head[Source] = newNode;

            newNode = GetRoadNode(Source, Head[Destination], Traffic);
            Head[Destination] = newNode;
        }
    }

    void PrintRoadList(RoadNode *ptr)
    {
        while (ptr != nullptr)
        {
            cout << " Road to ";
            if (ptr->Info->Type == "h")
                cout << "hospital -";
            else
                cout << "warehouse -";

            cout << ptr->Info->Name << " has traffic level " << ptr->Traffic << "\n";
            ptr = ptr->Next;
        }
        cout << endl;
    }

    void PrintDeliveryGraph()
    {
        for (int i = 0; i < NumLocations; i++)
        {
            cout << "Starting from " << Locations[i].Name << ": \n";
            PrintRoadList(this->Head[i]);
        }
    }

    int FindDestinationId(string name, int oxygen, int ppe, int remdesivir)
    {
        for (int i = 0; i < NUM_ASSETS; i++)
        {
            if (Locations[i].Name == name)
            {
                return Locations[i].Id;
            }
        }
        return -1;
    }

    int GetNumVertices()
    {
        return NumLocations;
    }

    ~DeliveryGraph()
    {
        for (int i = 0; i < NumLocations; i++)
        {
            RoadNode *current = Head[i];
            while (current != nullptr)
            {
                RoadNode *temp = current;
                current = current->Next;
                delete temp;
            }
        }
        delete[] Head;
    }
};

int GenerateRandom(int min, int max)
{
    int random_variable = rand();
    return min + (random_variable % (max - min + 1));
}

struct MinRouteNode
{
    int v;
    int dist;
};

struct MinRouteHeap
{
    int size;
    int capacity;
    int *pos;
    MinRouteNode **array;
};

MinRouteNode *CreateMinRouteNode(int v, int dist)
{
    MinRouteNode *minRouteNode = new MinRouteNode;
    minRouteNode->v = v;
    minRouteNode->dist = dist;
    return minRouteNode;
}

MinRouteHeap *CreateMinRouteHeap(int capacity)
{
    MinRouteHeap *minRouteHeap = new MinRouteHeap;
    minRouteHeap->pos = new int[capacity];
    minRouteHeap->size = 0;
    minRouteHeap->capacity = capacity;
    minRouteHeap->array = new MinRouteNode *[capacity];
    return minRouteHeap;
}

void SwapMinRouteNode(MinRouteNode **a, MinRouteNode **b)
{
    MinRouteNode *t = *a;
    *a = *b;
    *b = t;
}

void MinRouteHeapify(MinRouteHeap *minRouteHeap, int idx)
{
    int smallest, left, right;
    smallest = idx;
    left = 2 * idx + 1;
    right = 2 * idx + 2;

    if (left < minRouteHeap->size && minRouteHeap->array[left]->dist < minRouteHeap->array[smallest]->dist)
        smallest = left;

    if (right < minRouteHeap->size && minRouteHeap->array[right]->dist<minRouteHeap->array[smallest] -> dist)
        smallest = right;

    if (smallest != idx)
    {
        MinRouteNode *smallestNode = minRouteHeap->array[smallest];
        MinRouteNode *idxNode = minRouteHeap->array[idx];

        minRouteHeap->pos[smallestNode->v] = idx;
        minRouteHeap->pos[idxNode->v] = smallest;

        SwapMinRouteNode(&minRouteHeap->array[smallest], &minRouteHeap->array[idx]);

        MinRouteHeapify(minRouteHeap, smallest);
    }
}

int IsMinRouteHeapEmpty(MinRouteHeap *minRouteHeap)
{
    return minRouteHeap->size == 0;
}

MinRouteNode *ExtractMinRoute(MinRouteHeap *minRouteHeap)
{
    if (IsMinRouteHeapEmpty(minRouteHeap))
        return NULL;

    MinRouteNode *root = minRouteHeap->array[0];

    MinRouteNode *lastNode = minRouteHeap->array[minRouteHeap->size - 1];
    minRouteHeap->array[0] = lastNode;

    minRouteHeap->pos[root->v] = minRouteHeap->size - 1;
    minRouteHeap->pos[lastNode->v] = 0;

    --minRouteHeap->size;
    MinRouteHeapify(minRouteHeap, 0);

    return root;
}

void DecreaseKey(MinRouteHeap *minRouteHeap, int v, int dist)
{
    int i = minRouteHeap->pos[v];

    minRouteHeap->array[i]->dist = dist;

    while (i && minRouteHeap->array[i]->dist < minRouteHeap->array[(i - 1) / 2]->dist)
    {
        minRouteHeap->pos[minRouteHeap->array[i]->v] = (i - 1) / 2;
        minRouteHeap->pos[minRouteHeap->array[(i - 1) / 2]->v] = i;

        SwapMinRouteNode(&minRouteHeap->array[i], &minRouteHeap->array[(i - 1) / 2]);

        i = (i - 1) / 2;
    }
}

bool IsInMinRouteHeap(MinRouteHeap *minRouteHeap, int v)
{
    return minRouteHeap->pos[v] < minRouteHeap->size;
}

struct EfficientWarehouse
{
    int Id;
    int dist;
};

void PrintShortestRoute(int dist[], int n, int src, int dest)
{
    cout << "Time taken from warehouse " << Locations[src].Name << " to reach hospital " << Locations[dest].Name << " is " << dist[dest] << " minutes." << endl;
}

EfficientWarehouse DijkstraAlgorithm(DeliveryGraph *graph, int src, int dest)
{
    int V = graph->GetNumVertices();
    int *dist = new int[V];

    MinRouteHeap *minRouteHeap = CreateMinRouteHeap(V);

    for (int v = 0; v < V; ++v)
    {
        dist[v] = INT32_MAX;
        minRouteHeap->array[v] = CreateMinRouteNode(v, dist[v]);
        minRouteHeap->pos[v] = v;
    }

    minRouteHeap->array[src] = CreateMinRouteNode(src, dist[src]);
    minRouteHeap->pos[src] = src;
    dist[src] = 0;
    DecreaseKey(minRouteHeap, src, dist[src]);

    minRouteHeap->size = V;

    while (!IsMinRouteHeapEmpty(minRouteHeap))
    {
        MinRouteNode *minRouteNode = ExtractMinRoute(minRouteHeap);
        int u = minRouteNode->v;

        RoadNode *pCrawl = graph->Head[u];

        while (pCrawl != NULL)
        {
            int v = pCrawl->Destination;

            if (IsInMinRouteHeap(minRouteHeap, v) && dist[u] != INT32_MAX && pCrawl->Traffic + dist[u] < dist[v])
            {
                dist[v] = dist[u] + pCrawl->Traffic;
                DecreaseKey(minRouteHeap, v, dist[v]);
            }

            pCrawl = pCrawl->Next;
        }
    }

    PrintShortestRoute(dist, V, src, dest);
    EfficientWarehouse w = {src, dist[dest]};
    delete[] dist;
    delete minRouteHeap;
    return w;
}

DeliveryGraph *GenerateDeliveryMap()
{
    Connection Connections[19] = {
        {0, 5}, {1, 5}, {1, 2}, {2, 3}, {2, 4}, {3, 5}, {3, 6}, {4, 7}, {4, 8}, {5, 9}, {5, 10}, {6, 5}, {7, 6}, {7, 10}, {8, 7}, {8, 0}, {9, 2}, {9, 3}, {10, 9}};

    for (int i = 0; i < sizeof(Connections) / sizeof(Connections[0]); i++)
    {
        Connections[i].Traffic = GenerateRandom(1, 10);
    }

    int NumLocations = 11;
    int NumConnections = 19;

    DeliveryGraph *Graph = new DeliveryGraph(Connections, NumConnections, NumLocations);
    return Graph;
}

int main()
{
    srand(time(nullptr));
    DeliveryGraph *DeliveryG = GenerateDeliveryMap();
    cout << " -------- Welcome to Covid-19 Resource Delivery System\n\n";
    cout << "For the purpose of this demonstration, we have considered 8 hospitals and 3 warehouses\n";
    cout << "Details of the above are as follows : \n";
    for (int i = 0; i < NUM_ASSETS; i++)
    {
        if (Locations[i].Type == "h")
        {
            cout << "Hospital : " << Locations[i].Name << ", " << Locations[i].Address << endl;
        }
        else
        {
            cout << "Warehouse : " << Locations[i].Name << ", " << Locations[i].Address << endl;
        }
    }
    cout << "\n";
    DeliveryG->PrintDeliveryGraph();
    cout << "Enter the name of hospital that requires resources: ";
    string hospital;
    cin >> hospital;

    int oxygen, ppe, remdesivir;
    cout << "Enter the required no of oxygen concentrators: ";
    cin >> oxygen;
    cout << "Enter the required no of vaccines: ";
    cin >> ppe;
    cout << "Enter the required no of remdesivir: ";
    cin >> remdesivir;

    int dest = DeliveryG->FindDestinationId(hospital, oxygen, ppe, remdesivir);
    cout << endl;

    if (dest == -1)
    {
        cout << "Invalid hospital name. Please enter a valid hospital name." << endl;
        return 0;
    }

    EfficientWarehouse fastest = {-1, INT32_MAX};
    for (int i = 8; i < 11; i++)
    {
        EfficientWarehouse temp = DijkstraAlgorithm(DeliveryG, Locations[i].Id, dest);
        if (temp.dist < fastest.dist)
            fastest = temp;
    }

    cout << "\nAmong these, the most efficient warehouse for delivering the required resources is: ";
    cout << Locations[fastest.Id].Name << endl;
    return 0;
}
