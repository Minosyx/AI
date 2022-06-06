#include <iostream>
#include <vector>
#include <map>
#include <numeric>

using namespace std;

class dynamic_array
{
public:
	dynamic_array(const std::vector<int>& shape)
        : m_ndim(shape.size()), m_nelem(std::accumulate(shape.begin(), shape.end(), 1, multiplies<>())), m_shape(shape)
    {
        compute_strides();
        m_data.resize(m_nelem, { "", 0.00 });
    }

	dynamic_array(const dynamic_array& other)
		: m_ndim(other.m_ndim),
		  m_nelem(other.m_nelem),
		  m_shape(other.m_shape),
		  m_strides(other.m_strides),
		  m_data(other.m_data)
	{
	}

	dynamic_array(dynamic_array&& other) noexcept
		: m_ndim(other.m_ndim),
		  m_nelem(other.m_nelem),
		  m_shape(std::move(other.m_shape)),
		  m_strides(std::move(other.m_strides)),
		  m_data(std::move(other.m_data))
	{
	}

	dynamic_array& operator=(const dynamic_array& other)
	{
		if (this == &other)
			return *this;
		m_ndim = other.m_ndim;
		m_nelem = other.m_nelem;
		m_shape = other.m_shape;
		m_strides = other.m_strides;
		m_data = other.m_data;
		return *this;
	}

	dynamic_array& operator=(dynamic_array&& other) noexcept
	{
		if (this == &other)
			return *this;
		m_ndim = other.m_ndim;
		m_nelem = other.m_nelem;
		m_shape = std::move(other.m_shape);
		m_strides = std::move(other.m_strides);
		m_data = std::move(other.m_data);
		return *this;
	}

	~dynamic_array(){}

    const pair<string, double>& operator[](int i) const
    {
        return m_data.at(i);
    }

    pair<string, double>& operator[](int i)
    {
        return m_data.at(i);
    }

    const pair<string, double>& operator[](const std::vector<int>& indices) const
    {
	    const auto flat_index = std::inner_product(
            indices.begin(), indices.end(),
            m_strides.begin(), 0);
        return m_data.at(flat_index);
    }

    pair<string, double>& operator[](const std::vector<int>& indices)
    {
	    const auto flat_index = std::inner_product(
            indices.begin(), indices.end(),
            m_strides.begin(), 0);
        return m_data.at(flat_index);
    }

    void reshape(const std::vector<int>& new_shape)
    {
	    const auto new_nelem = std::accumulate(
            new_shape.begin(), new_shape.end(),
            1, multiplies<>());
        if (new_nelem != m_nelem)
        {
            throw std::invalid_argument("dynamic_array::reshape(): "
                "number of elements must not change.");
        }
        m_nelem = new_nelem;
        m_ndim = new_shape.size();
        m_shape = new_shape;
        compute_strides();
    }

    const std::vector<int>& shape() const
    {
        return m_shape;
    }

    const std::vector<int>& strides() const
    {
        return m_strides;
    }

    int ndim() const
    {
        return static_cast<int>(m_ndim);
    }

    int nelem() const
    {
        return m_nelem;
    }

    const vector<pair<string, double>>& data() const
	{
        return m_data;
    }

private:
    size_t m_ndim;
    int m_nelem;
    std::vector<int> m_shape;
    std::vector<int> m_strides;
    std::vector<pair<string, double>> m_data;

    void compute_strides()
    {
        m_strides.resize(m_ndim);
        m_strides.at(m_ndim - 1) = 1;
        partial_sum(m_shape.rbegin(),
            m_shape.rend() - 1,
            m_strides.rbegin() + 1,
            multiplies<>());
    }
};

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
    dynamic_array possibility_table;

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
    {
    }

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
	            for (auto &n : nd->dependencies)
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
        for (auto &s : nd->dependencies)
		{
            bool found = false;
            for (auto &c : *closed)
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

int main()
{
    enum states
    {
        TRUE,
        FALSE
    };

    //graph g(true);
    //node burglary = { "burglary" };
    //node earthquake = { "earthquake" };
    //node alarm = { "alarm" };
    //node john = { "john" };
    //node mary = { "mary" };
    //node susan = { "susan" };

    //burglary.possibility_table[TRUE] = { '+' + burglary.label,  0.001};
    //burglary.possibility_table[FALSE] = { '-' + burglary.label,  0.999};

    //earthquake.possibility_table[TRUE] = { '+' + earthquake.label,  0.002};
    //earthquake.possibility_table[FALSE] = { '-' + earthquake.label,  0.998 };

    //g.add_edge(&burglary, &alarm);
    //g.add_edge(&earthquake, &alarm);
	
    //alarm.possibility_table[{TRUE, TRUE}] = { '+' + alarm.dependencies[0]->label + '+' + alarm.dependencies[1]->label,  0.95};
    //alarm.possibility_table[{TRUE, FALSE}] = { '+' + alarm.dependencies[0]->label + '-' + alarm.dependencies[1]->label,  0.94};
    //alarm.possibility_table[{FALSE, TRUE}] = { '-' + alarm.dependencies[0]->label + '+' + alarm.dependencies[1]->label,  0.29};
    //alarm.possibility_table[{FALSE, FALSE}] = { '-' + alarm.dependencies[0]->label + '-' + alarm.dependencies[1]->label,  0.001};

    //g.add_edge(&alarm, &john);

    //john.possibility_table[TRUE] = { '+' + john.dependencies[0]->label,  0.9};
    //john.possibility_table[FALSE] = { '-' + john.dependencies[0]->label,  0.05};

    //g.add_edge(&alarm, &mary);
	
    //mary.possibility_table[TRUE] = { '+' + mary.dependencies[0]->label,  0.7};
    //mary.possibility_table[FALSE] = { '-' + mary.dependencies[0]->label,  0.01};

    //double poss = g.check_possibility({"john", "mary", "+alarm", "-burglary", "-earthquake"});
    ////double poss = g.check_possibility({ "john"});
    //cout << poss << endl;

    graph g(true);
    node rain = { "rain" };
    node grass_wet = { "grass_wet" };
    node sprinkler = { "sprinkler" };

    rain.possibility_table[TRUE] = { '+' + rain.label,  0.001 };
    rain.possibility_table[FALSE] = { '-' + rain.label,  0.999 };

    g.add_edge(&rain, &sprinkler);
    g.add_edge(&rain, &grass_wet);

    sprinkler.possibility_table[TRUE] = { '+' + sprinkler.dependencies[0]->label,  0.01 };
    sprinkler.possibility_table[FALSE] = { '+' + sprinkler.dependencies[0]->label,  0.4 };

    g.add_edge(&sprinkler, &grass_wet);

    grass_wet.possibility_table[{TRUE, TRUE}] = { '+' + grass_wet.dependencies[0]->label + '+' + grass_wet.dependencies[1]->label,  0.99 };
    grass_wet.possibility_table[{TRUE, FALSE}] = { '+' + grass_wet.dependencies[0]->label + '-' + grass_wet.dependencies[1]->label,  0.9 };
    grass_wet.possibility_table[{FALSE, TRUE}] = { '-' + grass_wet.dependencies[0]->label + '+' + grass_wet.dependencies[1]->label,  0.8 };
    grass_wet.possibility_table[{FALSE, FALSE}] = { '-' + grass_wet.dependencies[0]->label + '-' + grass_wet.dependencies[1]->label,  0.0 };

    double poss = g.check_possibility({ "grass_wet", "+sprinkler", "+rain", });
    //double poss = g.check_possibility({ "john"});
    cout << poss << endl;
}
