// wgraph.h (Directed or undirected graph with edge weights)

#include "graph.h"
#include<fstream>
#include<sstream>
#include<stdexcept>

class Weighted_Graph : public Graph                             // derived from Graph
{
public:
      explicit Weighted_Graph(size_t num_nodes, bool directed)  // constructor: 
          : Graph(num_nodes), _weight(), _directed(directed)    // empty graph with 
      {}                                                        // num_nodes vertices

      explicit Weighted_Graph(char const * filename, bool directed)  // constructor: 
          : Graph(), _weight(), _directed(directed)                  // read from file
      {                                   
            std::fstream file(filename);                             // open file 
            if (not file) throw std::runtime_error("Cannot open file.");

            NodeId num_nodes = 0;
            std::string line;
            std::getline(file, line);            // get first line of file
            std::stringstream ss(line);          // convert line to a stringstream for
            ss >> num_nodes;                     // which we can use >> as with cin
            if (not ss) throw std::runtime_error("Invalid file format.");

            for (size_t i = 0; i < num_nodes; ++i) add_node();

            while (std::getline(file, line)) 
            {
                  NodeId head = invalid_node;
                  NodeId tail = invalid_node;
                  std::stringstream ss(line);
                  ss >> tail;
                  if (not ss) throw std::runtime_error("Invalid file format.");
                  ss >> head;
                  if (not ss) throw std::runtime_error("Invalid file format.");
                  if (tail != head) add_edge(tail, head);
                  else throw std::runtime_error("Loops are not allowed.");
                  double new_weight = 0.0;
                  ss >> new_weight;
                  _weight.push_back(new_weight);
            }
      }

      void print() const                                       // print directed or
      {                                                        // undirected graph
            char bl = '{', br = '}';
            if (_directed) { bl = '('; br=')'; }  
            std::cout << "The graph contains vertices 0,...," 
                      << num_nodes() - 1 << " and the following edges:\n";
            for (EdgeId e = 0; e != num_edges(); ++e) 
            {
                  std::cout << bl << get_edge(e).get_tail() 
                            << "," << get_edge(e).get_head() 
                            << br << " with weight " << _weight[e] << "\n";
            }
      }

      class Iterator                                     // Iterator over incident or
      {                                                  // outgoing edges of a vertex
      public:
            EdgeId operator*() const  
            { 
                  if (_out) return _node.out_edges()[_index];
                       else return _node.in_edges()[_index];
            }

            EdgeId operator!=(Iterator const & other) const  
            { 
                  return ((_out != other._out) or (_index != other._index));
            }

            void operator++() 
            { 
                  ++_index; 
                  if (_out and (_index == _node.out_edges().size()) and not _directed)  
                  { 
                        _out = false;
                        _index = 0;
                  }
            }

      private:
            Iterator(Node const & node, bool first, bool directed) 
                : _node(node), _out(false), _index(0), _directed(directed)
            {
                  if (_directed or (first and (node.out_edges().size() > 0))) 
                  {
                        _out = true;
                  }
                  if (not first) 
                  {
                        if (directed) _index = node.out_edges().size();
                                 else _index = node.in_edges().size();
                  }
            }
            friend class Weighted_Graph;

            Node const & _node;
            bool _out;
            size_t _index;
            bool _directed;
      };

      Iterator first(NodeId node) const 
      { 
            return Iterator(get_node(node), true, _directed); 
      }
      Iterator last(NodeId node) const 
      { 
            return Iterator(get_node(node), false, _directed); 
      }

      NodeId other_endpoint(EdgeId e, NodeId v) const
      { 
            if (get_edge(e).get_tail() == v) 
                 return get_edge(e).get_head();
            else return get_edge(e).get_tail();
      }

      std::vector<double> _weight;                    // edge weights
      bool _directed;                                 // directed or undirected graph?
};

