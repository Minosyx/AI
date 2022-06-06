#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <regex>

using namespace std;

/// Sprawdzenie czy string jest liczb¹
/// \param s String
/// \return True jeœli string jest liczb¹
bool is_integer(const string& s)
{
    return regex_match(s, regex("[(-|+)|]?[0-9]+"));
}

/// Wêze³ drzewa decyzyjnego
struct Node
{
    /// Pytanie
    string question;

    /// Wektor wniosków
    vector<string> conclusion;

    /// Wektor proponowanych odpowiedzi
    vector<string> possible_answers;

    /// Wektor wêz³ów potomnych
    vector<Node*> children;

    /// Rodzic
    Node* parent;

    /// Konstruktor
    /// \param question Pytanie
    /// \param possible_answers Wektor proponowanych odpowiedzi
    explicit Node(string question, const vector<string>& possible_answers = {}) : question(std::move(question)),
	    possible_answers(possible_answers), parent(nullptr)
    {
    }

    /// Konstruktor
    /// \param question Pytanie
    /// \param conclusion Wektor wniosków
    /// \param possible_answers Wektor proponowanych odpowiedzi
    explicit Node(string question, const vector<string>& conclusion, const vector<string>& possible_answers = {}) :
	    question(std::move(question)), conclusion(conclusion), possible_answers(possible_answers), parent(nullptr)
    {
    }

    /// Konstruktor
    /// \param question Pytanie
    /// \param children Wektor wêz³ów potomnych
    Node(string question, const vector<Node*>& children) : question(std::move(question)), children(children)
    {
	    for (auto &child : children)
        {
            child->parent = this;
        }
    }

    /// Destruktor
    ~Node()
    {
        for (const auto &child : children)
        {
            delete child;
        }
    }

    /// Dodanie wêz³a potomnego
    /// \param question Pytanie
    /// \param conclusion Wektor wniosków
    /// \param possible_answers Wektor proponowanych odpowiedzi
    /// \param index Indeks wêz³a potomnego
    /// \return WskaŸnik na nowo utworzony wêze³
    Node* addChild(const string& question, const vector<string>& conclusion = {}, const vector<string>& possible_answers = {}, const int index = -1)
    {
        Node* child = new Node(question);
        child->parent = this;
        if (index == -1)
            children.push_back(child);
        else
            children.insert(children.begin() + index, child);
        child->possible_answers = possible_answers;
        child->conclusion = conclusion;
        return child;
    }

    /// Dodanie wêz³a potomnego
    /// \param child Wêze³ potomny
    /// \param index Indeks wêz³a potomnego
    void addChild(Node* child, const int index = -1)
    {
        child->parent = this;
        if (index == -1)
            children.push_back(child);
        else
            children.insert(children.begin() + index, child);
    }
};

/// Drzewo decyzyjne
class DecisionTree
{
private:
    using NodePtr = Node*;

    /// Wêze³ g³ówny
    NodePtr root;

    /// Wyœwietlenie proponowanych odpowiedzi
    /// \param node Wêze³
    static void printPossibleAnswers(NodePtr node)
    {
        if (node->children.size() > 2)
        {
            for (int i = 0; i < node->children.size(); i++)
            {
                cout << i + 1 << ". " << node->possible_answers[i] << endl;
            }
        }
    }

    /// Sprawdzenie odpowiedzi
    /// \param response OdpowiedŸ
    static void checkResponse(string& response)
    {
        while (response != "y" && response != "n")
        {
            cout << "Please enter y or n" << endl;
            cout << "Your choice: ";
            cin >> response;
        }
    }

    /// Sprawdzenie odpowiedzi liczbowej
    /// \param response OdpoiwedŸ
    /// \param size Iloœæ dostêpnych odpowiedzi
    static void checkResponseInt(string& response, int size)
    {
        while (!is_integer(response) || stoi(response) > size)
        {
            cout << "Please enter a number" << endl;
            cout << "Your choice: ";
            cin >> response;
        }
    }

    /// Przechodzenie drzewa w górê
    /// \param node Wskaznik na wêze³
    /// \return Prawda jeœli warunki zosta³y spe³nione
    bool ascendTree(NodePtr node){
	    constexpr bool result = true;
        if (node->parent != nullptr) {
	        string response;
	        const NodePtr current = node->parent;
            cout << current->question;
            if (current->children.size() <= 2)
                cout << " (y/n)";
            cout << endl;
            printPossibleAnswers(current);
            cout << "Your choice: ";
            cin >> response;
            if (current->children.size() <= 2) {
                checkResponse(response);
                if (response == "y")
                {
                    if (node == current->children[0])
                    {
                        if (!current->conclusion.empty())
                            cout << current->conclusion[0] << endl;
                        return ascendTree(current);
                    }
                	return false;
                }
                if (node == current->children[1]){
                    if (current->conclusion.size() > 1)
                        cout << current->conclusion[1] << endl;
                    return ascendTree(current);
                }
            	return false;
            }
            checkResponseInt(response, current->children.size());
	        const int index = stoi(response) - 1;
            if (node == current->children[index])
            {
                if (current->conclusion.size() > index + 1)
                    cout << current->conclusion[index] << endl;
                return ascendTree(current);
            }
        	return false;
        }
        return result;
    }

    /// Przechodzenie drzewa w g³¹b
    /// \param node Wskaznik na wêze³
    /// \return Prawda jeœli przewidywanie poprawne
    int descendTree(NodePtr node)
    {
	    int result = 0;

        if (node != nullptr)
        {
	        string response;
	        cout << node->question;
            if (node->children.size() <= 2)
                cout << " (y/n)";
            cout << endl;
            printPossibleAnswers(node);
            cout << "Your choice: ";
            cin >> response;
            if (node->children.size() <= 2)
            {
                checkResponse(response);
                if (node->children.empty())
                {
                    if (response == "y")
                    {
                        if (!node->conclusion.empty())
                            cout << node->conclusion[0] << endl;
                        result = 1;
                    }
                    else
                    {
                        if (node->conclusion.size() > 1)
                            cout << node->conclusion[1] << endl;
                        result = 0;
                    }
                }
                else
                {
                    if (response == "y")
                    {
                        if (!node->conclusion.empty())
                            cout << node->conclusion[0] << endl;
                        result = descendTree(node->children[0]);
                    }
                    else
                    {
                        if (node->conclusion.size() > 1)
                            cout << node->conclusion[1] << endl;
                        result = descendTree(node->children[1]);
                    }
                }
            }
            else
            {
                checkResponseInt(response, node->children.size());
                const int index = stoi(response) - 1;
                if (node->conclusion.size() > index + 1)
                    cout << node->conclusion[index] << endl;
                result = descendTree(node->children[index]);
            }

        }
        return result;
    }
public:
    /// Wrapper do przechodzenia drzewa w g³¹b
    int descend()
    {
        return descendTree(root);
    }

    /// Wrapper do przechodzenia drzewa w górê
    bool prove(NodePtr node)
    {
        return ascendTree(node);
    }

    /// Konstruktor
    DecisionTree() : root(nullptr)
    {
    }

    /// Konstruktor
    /// \param root Wskaznik na korzeñ drzewa
    explicit DecisionTree(NodePtr root) : root(root)
    {
    }

    /// Destruktor
    ~DecisionTree()
    {
        delete root;
    }
};

int main()
{
    Node* root = new Node("Does it live on land?", {}, { "Yes", "No", "Maybe" });
    Node* wings = root->addChild("Does it have wings?");
    Node* cold_blooded = root->addChild("Is it cold blooded?");
    Node* amphibian = root->addChild("Is its skin lumpy?");
    Node* toad = amphibian->addChild("Is it a toad?", { "It is indeed a toad" });
    Node* frog = amphibian->addChild("Is it a frog?");
    Node* duck = wings->addChild("Is it a duck?");
    Node* elephant = wings->addChild("Is it an elephant?");
    Node* shocking = cold_blooded->addChild("Is it shocking?");
    Node* electric_eel = shocking->addChild("Is it an electric eel?");
    Node* shark = shocking->addChild("Is it a shark?");
    Node* whale = cold_blooded->addChild("Is it a whale?");

    DecisionTree tree(root);
    //const int guessed = tree.descend();
    //if (guessed)
    //    cout << "\nCorrect guess\n";
    //else
    //    cout << "\nWrong guess\n";

    const bool proved = tree.prove(toad);
    cout << "Statement is " << (proved ? "true" : "false") << endl;
}
