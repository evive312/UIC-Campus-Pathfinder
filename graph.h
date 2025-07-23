// graph.h <Starter Code>
// < Your name >
//
// Basic graph class using adjacency matrix representation.  Currently
// limited to a graph with at most 100 vertices.
//
//
// Adam T Koehler, PhD
// University of Illinois Chicago
// CS 251, Spring 2023
//
// Project Original Variartion By:
// Joe Hummel, PhD
// University of Illinois at Chicago
//

#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <queue>
using namespace std;

template<typename VertexT, typename WeightT>
class graph {
 private:

  unordered_map<VertexT, unordered_map<VertexT, WeightT>> adjList;
  int numEdgs;


 public:
  //
  // constructor:
  //
  // Constructs an empty graph where n is the max # of vertices
  // you expect the graph to contain.
  //
  // NOTE: the graph is implemented using an adjacency matrix.
  // If n exceeds the dimensions of this matrix, an exception
  // will be thrown to let you know that this implementation
  // will not suffice.
  //
  graph() {
    this->adjList = {};
    this->numEdgs = 0;
  }
  //
  // NumVertices
  //
  // Returns the # of vertices currently in the graph.
  //
  int NumVertices() const {
    return(this->adjList.size());
  }

  //
  // NumEdges
  //
  // Returns the # of edges currently in the graph.
  //
  int NumEdges() const {
    return(this->numEdgs);
  }

  //
  // addVertex
  //
  // Adds the vertex v to the graph if there's room, and if so
  // returns true.  If the graph is full, or the vertex already
  // exists in the graph, then false is returned.
  //
  bool addVertex(VertexT v) {
    if (adjList.count(v) == 0)
    {
      adjList[v];
      return true;
    }
    return false;
  }


  //
  // addEdge
  //
  // Adds the edge (from, to, weight) to the graph, and returns
  // true.  If the vertices do not exist or for some reason the
  // graph is full, false is returned.
  //
  // NOTE: if the edge already exists, the existing edge weight
  // is overwritten with the new edge weight.
  //
  bool addEdge(VertexT from, VertexT to, WeightT weight) 
  {

    if(adjList.count(from) == 0 || adjList.count(to) == 0)
    {
      return false;
    }
    if(adjList.at(from).count(to) != 0)
    {
      adjList[from][to] = weight;
      return true;
    }
    adjList[from][to] = weight;
    numEdgs++;
    return true;
  }
  //
  // getWeight
  //
  // Returns the weight associated with a given edge.  If
  // the edge exists, the weight is returned via the reference
  // parameter and true is returned.  If the edge does not
  // exist, the weight parameter is unchanged and false is
  // returned.
  //
  bool getWeight(VertexT from, VertexT to, WeightT& weight) const {
    if(adjList.count(from) == 0 || adjList.count(to) == 0 || adjList.at(from).count(to) == 0)
    {
      return false;
    }
    weight = adjList.at(from).at(to);
    return true;
   }
  

  //
  // neighbors
  //
  // Returns a set containing the neighbors of v, i.e. all
  // vertices that can be reached from v along one edge.
  // Since a set is returned, the neighbors are returned in
  // sorted order; use foreach to iterate through the set.
  //
  set<VertexT> neighbors(VertexT v) const 
  {
    set<VertexT> hood;
    if(adjList.count(v) != 0)
    {
      for(auto& p : adjList.at(v))
      {
        hood.insert(p.first);
      }
    }
    return hood;
  }

  //
  // getVertices
  //
  // Returns a vector containing all the vertices currently in
  // the graph.
  //
  vector<VertexT> getVertices() const {
    vector<VertexT> vertVec;
    for(auto& p : adjList)
    {
      vertVec.push_back(p.first);
    }
    return vertVec;
  }

  //
  // dump
  //
  // Dumps the internal state of the graph for debugging purposes.
  //
  // Example:
  //    graph<string,int>  G(26);
  //    ...
  //    G.dump(cout);  // dump to console
  //
  void dump(ostream& output) const {
    output << "# of vertices: " << NumVertices() << endl;
    output << "# of edges: " << NumEdges() << endl;
    output << "**************************************************" << endl;
    for(auto v : adjList)
    {
      output << v.first << ": ";

      for(auto e : v.second)
      {
        output << "("<< v.first << "," << e.first << "," << e.second << ") ";
      }
      output << endl;
    }
  }
};
