#include <map>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

/**
* \brief Wêze³ grafu
*/
struct node
{
    /**
     * \brief Etykieta
     */
    string label;

    /**
     * \brief Prawdopodobieñstwo
     */
     possibility_table;

    /**
     * \brief Zale¿noœæ prawdopodobieñstwa
     */
    vector<node*> dependencies;

    /**
     * \brief Przodkowie
     */
    int ancestors;

    /**
     * \brief Komparator dla mapy
     * \param other Inny wêze³
     * \return Czy aktualny jest mniejszy od drugiego
     */
    bool operator<(const node& other) const
    {
        return (*this).label < other.label;
    }

    node(string label)
        : label(std::move(label)), possibility_table({ 1, 2 }), ancestors(0)
    {}
};

/// <summary>
/// KrawêdŸ grafu
/// </summary>
struct edge
{
    /**
     * \brief Wêze³ docelowy
     */
    node* destination;
};

class graph
{
private:
    /// <summary>
    /// Czy graf jest skierowany?
    /// </summary>
    bool directed;

    /**
     * \brief Iloœæ wierzcho³ków
     */
    int nodes;

    /**
     * \brief Mapa wêz³ów na wektor krawêdzi
     */
    map<node, vector<edge>> node_map;
public:
    explicit graph(const bool directed)
        : directed(directed), nodes(0)
    {}

    /**
     * \brief Dodanie krawêdzi
     * \param src Wêze³ Ÿród³owy
     * \param dest Wêze³ docelowy
     */
    void add_edge(node* src, node* dest)
    {
        const edge e{ dest };
        const auto it = node_map.find(*src);
        if (it == node_map.end())
        {
            vector<edge> ve;
            ve.push_back(e);
            node_map[*src] = ve;
            dest->ancestors++;
            nodes++;
        }
        else
        {
            dest->ancestors++;
            node_map[*src].push_back(e);
            nodes += 2;
        }
        dest->dependencies.push_back(src);
        if (dest->ancestors == 2)
        {
            dest->possibility_table = dynamic_array({ 2, 2 });
        }
        else if (dest->ancestors > 2)
        {
            vector<int> tmp = dest->possibility_table.shape();
            tmp.push_back(2);
            dest->possibility_table = dynamic_array(tmp);
        }

        if (!directed)
        {
            const edge re{ src };
            const auto rit = node_map.find(*dest);
            if (it == node_map.end())
            {
                vector<edge> ve;
                ve.push_back(re);
                node_map[*dest] = ve;
                src->ancestors++;
            }
            else
                node_map[*dest].push_back(re);

            if (src->ancestors == 2)
            {
                src->possibility_table = dynamic_array({ 2, 2 });
            }
            else if (src->ancestors > 2)
            {
                vector<int> tmp = dest->possibility_table.shape();
                tmp.push_back(2);
                src->possibility_table = dynamic_array(tmp);
            }
            src->dependencies.push_back(dest);

        }
    }

    const node* find_node_by_label(const string& label) const
    {
        for (auto& n : node_map)
        {
            if (n.first.label == label)
                return &n.first;
            for (auto& i : n.second)
            {
                if (i.destination->label == label)
                    return i.destination;
            }
        }
        return nullptr;
    }

    void traverse(const node* nd, const vector<string>& v, vector<string>* closed, double* poss)
    {
        vector<string> dep;
        for (auto& i : v)
        {
            if (!nd->dependencies.empty())
                for (auto& n : nd->dependencies)
                {
                    if (i.find(n->label) != std::string::npos)
                    {
                        dep.push_back(i);
                    }
                }
            else
                if (i.find(nd->label) != std::string::npos)
                {
                    dep.push_back(i);
                }
        }
        if (find(closed->begin(), closed->end(), nd->label) == closed->end())
            closed->push_back(nd->label);
        for (auto& it : nd->possibility_table.data())
        {
            bool found = true;
            for (auto& i : dep)
            {
                if (it.first.find(i) == std::string::npos)
                {
                    found = false;
                    break;
                }
            }
            if (!dep.empty() && found)
                *poss *= it.second;
            else if (dep.empty())
                *poss *= it.second;

        }
        for (auto& s : nd->dependencies)
        {
            bool found = false;
            for (auto& c : *closed)
            {
                if (c.find(s->label) != std::string::npos)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                traverse(s, v, closed, poss);
        }
    }

    double check_possibility(const vector<string>& v)
    {
        vector<string> closed;

        if (v.size() > nodes)
        {
            throw std::invalid_argument("graph::check_possibility(): "
                "vector size is greater than number of nodes.");
        }

        double poss = 1;
        for (auto& n : v)
        {
            string parsing = n;
            parsing.erase(remove(parsing.begin(), parsing.end(), '+'), parsing.end());
            parsing.erase(remove(parsing.begin(), parsing.end(), '-'), parsing.end());
            if (find(closed.begin(), closed.end(), parsing) != closed.end())
                continue;
            const node* tmp = find_node_by_label(parsing);
            if (tmp == nullptr)
                throw std::invalid_argument("graph::check_possibility(): "
                    "node with label " + n + " not found.");
            traverse(tmp, v, &closed, &poss);
        }

        return poss;
    }
};