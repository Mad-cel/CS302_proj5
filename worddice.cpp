#include <cstdio>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

typedef enum {SOURCE, SINK, DICE, WORD} Node_Type;

class Edge {
  public:
	  Edge(class Node *, class Node *, int, int, int);
      int index;                      // The edge's index (to store in the edge map).
      class Node *from;               // The "from" node
      class Node *to;                 // The "to" node
      Edge *reverse;                  // The reverse edge, for processing the residual graph
      int original;                   // The original edge capacity.
      int residual;                   // The edge capacity in the residual graph.
};

class Node
{
  public:
      Node(int);
      int id;
      Node_Type type;
      vector <class Edge *> adj;      // The node's adjacency list in the residual graph.
      int visited;                    // A visited field for depth-first search.
      Edge *backedge;
      vector <bool> letters;          //store letters
};

class Graph
{
	public:
		vector <Node *> nodes;
		int min_nodes;			//numbers of dices
		int BFS();				//determine whether I can spell, return 1 if I can 	
		int canIspell();		//call BFS()
		void delete_halfgraph();//delete graph, erase edge between Word and dice, word and sink, delete word && sink
								//resize nodes to min_nodes 
		
};

Edge::Edge(class Node * f, class Node *t, int o, int r, int i)
{
	from = f;
	to = t;
	original = o;
	residual = r;
	index = i;
}

Node::Node(int a)
{
	id = a;
	letters.resize(26, 0);
	visited = -1;
	backedge = NULL;
}

int Graph::BFS(){}

int Graph::canIspell(){}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		cerr << "Not enough argument provided Idiot" << endl;
		return 0;
	}

	string dice_file = argv[1], word_file = argv[2], input;
	int id = 0;		//id mean the number of node at the moment as well as the index 
	ifstream dice_fin, word_fin;
	dice_fin.open(dice_file);

	//decleare source Node
	Node * source_node = new Node(id);
	id++;
	source_node->type = SOURCE;


	//read dice_file 
	if (!dice_fin.is_open())
	{
		cerr << "Check dice_File " << endl;
		return 0;
	}

	//create Graph
	Graph *g = new Graph();
	g->nodes.push_back(source_node);
	g->min_nodes = 0;
	//source index keep track of all edges to the sources
	int source_index = 0;


	//read all dice && make edges to the source node
	while (getline(dice_fin, input))
	{
		Node *dice_node = new Node(id);
		id++;
		dice_node->type = DICE;

		for (char &letter : input)
		{
			dice_node->letters[letter - 65] = 1;
			//cout << letter;
		}

		Edge * to_source = new Edge(dice_node, source_node, 0, 1, 0);
		Edge * from_source = new Edge(source_node, dice_node, 1, 0, source_index);
		//cout << to_source->original;
		source_index++;

		to_source->reverse = from_source;
		from_source->reverse = to_source;
		source_node->adj.push_back(from_source);
		g->nodes.push_back(dice_node);
		g->min_nodes++;

	}
	dice_fin.close();

}
