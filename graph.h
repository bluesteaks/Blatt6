// graph.h (Declaration of class Graph)

#ifndef GRAPH_H                      // prevents to include declaration multiple times
#define GRAPH_H

#include <iostream>
#include <vector>

class Graph
{
public:

      typedef std::size_t NodeId;   // vertices are numbered 0,...,num_nodes()-1
      typedef std::size_t EdgeId;   // edges are numbered 0,...,num_edges()-1

      class Node
      {
      public:
            void add_incoming_edge(EdgeId);
            void add_outgoing_edge(EdgeId);

            std::vector<EdgeId> const & in_edges() const;     
            std::vector<EdgeId> const & out_edges() const;

      private:
            std::vector<EdgeId> _incoming;
            std::vector<EdgeId> _outgoing;
      };

      class Edge
      {
      public:
            Edge();
            Edge(NodeId tail, NodeId head);

            NodeId get_tail() const;
            NodeId get_head() const;

      private:
            NodeId _tail;
            NodeId _head;
      };

      explicit Graph(NodeId num_nodes);     // constructor cannot be called implicitly
      Graph();

      EdgeId add_edge(NodeId tail, NodeId head);
      NodeId add_node();

      NodeId num_nodes() const;
      EdgeId num_edges() const;

      Node & get_node(NodeId);
      Node const & get_node(NodeId) const;

      Edge & get_edge(EdgeId);
      Edge const & get_edge(EdgeId) const;

      static const EdgeId invalid_edge;
      static const NodeId invalid_node;

private:
      std::vector<Node> _nodes;
      std::vector<Edge> _edges;
};

void print_graph(Graph const &);            // print digraph to cout
Graph read_graph(char const * filename);    // read digraph from file

#endif

