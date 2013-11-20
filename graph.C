// graph.C (Implementation of class Graph)

#include <fstream>
#include <sstream>
#include <limits>
#include <stdexcept>

#include "graph.h"

Graph::EdgeId const Graph::invalid_edge = std::numeric_limits<EdgeId>::max();
Graph::NodeId const Graph::invalid_node = std::numeric_limits<NodeId>::max();

Graph::Graph()
{
}

Graph::Graph(NodeId num_nodes)
 : _nodes(num_nodes)
{
}

Graph::NodeId Graph::add_node() 
{
      _nodes.push_back(Node());
      return _nodes.size() - 1;
}

Graph::EdgeId Graph::add_edge(NodeId tail, NodeId head)
{
      if ((tail >= num_nodes()) or (head >= num_nodes()))
      {
         throw std::runtime_error("Edge cannot be added due to undefined endpoint.");
         return invalid_edge;
      }
      else
      {
      _edges.push_back(Edge(tail, head));
      EdgeId edge_id = _edges.size() - 1;
      _nodes[tail].add_outgoing_edge(edge_id);
      _nodes[head].add_incoming_edge(edge_id);
      return edge_id;
      }
}

void Graph::Node::add_incoming_edge(EdgeId edge)
{
      _incoming.push_back(edge);
}

void Graph::Node::add_outgoing_edge(EdgeId edge)
{
      _outgoing.push_back(edge);
}

Graph::Edge::Edge()
 : _tail(Graph::invalid_node),
   _head(Graph::invalid_node)
{
}

Graph::Edge::Edge(NodeId tail, NodeId head)
 : _tail(tail),
   _head(head)
{
}

Graph::NodeId Graph::Edge::get_tail() const 
{
      return _tail;
}

Graph::NodeId Graph::Edge::get_head() const 
{
      return _head;
}

std::vector<Graph::EdgeId> const & Graph::Node::in_edges() const
{
      return _incoming;
}

std::vector<Graph::EdgeId> const & Graph::Node::out_edges() const
{
      return _outgoing;
}

Graph::NodeId Graph::num_nodes() const
{
      return _nodes.size();
}

Graph::EdgeId Graph::num_edges() const
{
      return _edges.size();
}

Graph::Node & Graph::get_node(NodeId node)
{
      return _nodes[node];
}

Graph::Node const & Graph::get_node(NodeId node) const
{
      return _nodes[node];
}

Graph::Edge & Graph::get_edge(EdgeId edge)
{
      return _edges[edge];
}

Graph::Edge const & Graph::get_edge(EdgeId edge) const
{
      return _edges[edge];
}


void print_graph(Graph const & g) 
{
      for (Graph::NodeId v = 0; v != g.num_nodes(); ++v) 
      {
            std::cout << "The following edge(s) leave(s) vertex " << v << ":\n";
            for (Graph::EdgeId e = 0; e != g.get_node(v).out_edges().size(); ++e) 
            {
                  std::cout << g.get_edge(g.get_node(v).out_edges()[e]).get_tail() 
                            << " -> "
                            << g.get_edge(g.get_node(v).out_edges()[e]).get_head() 
                            << "\n";
            }
      }
}


Graph read_graph(char const * filename)
{
      std::fstream file(filename);             // open file 
      if (not file) 
      {
            throw std::runtime_error("Cannot open file.");
            return Graph();
      }

      Graph::NodeId num_nodes = 0;
      std::string line;
      std::getline(file, line);                // get first line of file
      std::stringstream ss(line);              // convert line to a stringstream
      ss >> num_nodes;                         // for which we can use << as with cin
      if (not ss) 
      {
            throw std::runtime_error("Invalid file format.");
            return Graph();
      }
      Graph graph(num_nodes);

      while (std::getline(file, line)) 
      {
            Graph::NodeId head = Graph::invalid_node, tail = Graph::invalid_node;
            std::stringstream ss(line);
            ss >> tail;
            if (not ss) 
            {
                  throw std::runtime_error("Invalid file format.");
                  return Graph();
            }
            ss >> head;
            if (not ss) 
            {
                  throw std::runtime_error("Invalid file format.");
                  return Graph();
            }
            if (tail != head) 
            {
                  graph.add_edge(tail, head);
            }
            else throw std::runtime_error("Invalid file format: loops not allowed.");
      }
      return graph;
}

