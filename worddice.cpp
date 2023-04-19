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
	  Edge(class Node *, class Node *, int, int);
      //int index;                      // The edge's index (to store in the edge map).
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
		void print_node();
		vector <Node *> nodes;
		int min_nodes;			//numbers of nodes include source, dice, words and sink
		int BFS();				//determine whether I can spell, return 1 if I can 	
		int canIspell();		//call BFS()
		void delete_halfgraph();//delete graph, erase edge between Word and dice, word and sink, delete word && sink
								//resize nodes to min_nodes 
};

void Graph::print_node()
{
	for (int i = 0; i < (int)nodes.size(); i++)
	{
		cout << "Node " << nodes[i]->id << ":";
		for (int x = 0; x < (int)nodes[i]->letters.size(); x++)
		{
			if (nodes[i]->letters[x])
				cout << (char)(x + 65);
		}	

		cout << endl;

		for (int j = 0; j < (int)nodes[i]->adj.size(); j++)
		{
			cout << nodes[i]->adj[j]->to->id << " ";
		}
		cout << endl;
	}
}

Edge::Edge(class Node * f, class Node *t, int o, int r)
{
	from = f;
	to = t;
	original = o;
	residual = r;
	//index = i;
}

Node::Node(int a)
{
	id = a;
	letters.resize(26, 0);
	visited = 0;
	backedge = NULL;
}

int Graph::BFS(){
	//BFS 
}

int Graph::canIspell(){
	//call BFS
}

void Graph::delete_halfgraph()
{

	vector <Node *>::iterator it = nodes.begin();
	int i = 0;
	while (i < (int)nodes.size())
	{

		//clear the edges for all Dice 
		if (nodes[i]->type == DICE)
		{
			nodes[i]->adj.clear();
			i++;			
		}
		 
		//delete Word/Sink Dice, remove it from nodes vector 
		else if (nodes[i]->type == WORD || nodes[i]->type == SINK)
		{
			delete nodes[i];
			nodes.erase(it + i, it + i + 1);
		}

		else
		{
			vector <Edge *>::iterator edge_it = nodes[i]->adj.begin();
			while (edge_it != nodes[i]->adj.end())
			{
				(*edge_it)->original = 1;
				(*edge_it)->residual = 0;
				(*edge_it)->reverse->original = 0;
				(*edge_it)->reverse->residual = 0;
				edge_it++;
			}
			i++;

		}
	}
	min_nodes = nodes.size() - 1;
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		cerr << "Not enough argument provided Idiot" << endl;
		return 0;
	}

	string dice_file = argv[1], word_file = argv[2], input;
	int id = 0;		//id mean the number of node at the moment as well as the index which include source, dice, word and sink
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

		//to source connect from dice to source, from source connect from source to dice
		Edge * to_source = new Edge(dice_node, source_node, 0, 1);
		Edge * from_source = new Edge(source_node, dice_node, 1, 0);
		//cout << to_source->original;
		source_index++;

		to_source->reverse = from_source;
		from_source->reverse = to_source;
		source_node->adj.push_back(from_source);
		g->nodes.push_back(dice_node);
		g->min_nodes++;

	}
	dice_fin.close();

	//read words 
	word_fin.open(word_file);
	if (!word_fin.is_open())
	{
		cerr << "Check word file " << endl;
		return 0;
	}
	int num_dice = g->min_nodes;

	while (getline(word_fin, input))
	{
		for (char &letter : input)
		{
			Node *word_node = new Node(id);
			id++;
			word_node->type = WORD;
			word_node->letters[letter - 65] = 1;

			int word_node_index = 0, dice_node_index = 1; 

			//starting from 1 to skip the source node
			for (int i = 1; i <= num_dice; i++)
			{	
				//For each letter in words, Loop through all Node, if the Nodes is dice AND contains that letter  
				if (g->nodes[i]->letters[letter - 65] == 1 && g->nodes[i]->type == DICE)
				{
					//create connection similiar to connection to source
					Edge * to_word = new Edge(g->nodes[i], word_node, 1, 0);
					word_node_index++;
					Edge * from_word = new Edge(word_node, g->nodes[i], 0, 1);
					dice_node_index++;

					to_word->reverse = from_word;
					from_word->reverse = to_word;

					g->nodes[i]->adj.push_back(to_word);
					word_node->adj.push_back(from_word);
				}
			}

			g->nodes.push_back(word_node);
			g->min_nodes++;
		
		}

		//create Sink node
		Node * sink = new Node(id);
		sink->type = SINK;
		id++;
		int sink_index = 0;

		for (int i = num_dice + 1; i <= g->min_nodes; i++)
		{
			//default case where the edges to sink index = 0
			Edge * to_sink = new Edge(g->nodes[i], sink, 1, 0);

			/*
			if (g->nodes[i]->adj.size() != 0)
			{
				to_sink->index = g->nodes[i]->adj[g->nodes[i]->adj.size() - 1]->index + 1;
			}
			*/

			Edge * from_sink = new Edge(sink, g->nodes[i], 0, 1);
			sink_index++;
			from_sink->reverse = to_sink;
			to_sink->reverse = from_sink;

			g->nodes[i]->adj.push_back(to_sink);
		}
		g->nodes.push_back(sink);
		g->min_nodes++;
			
		g->print_node();

		/*
		if (g->canIspell())
		{
			//print output 
		}
		else
			cout << "Cannot spell " << input << endl;
		*/
		g->delete_halfgraph();

		id = num_dice + 1;	

	}

	return 0;

}

