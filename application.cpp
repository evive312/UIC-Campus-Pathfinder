
// <Edgar Viveros>
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
// 
// References:
// TinyXML: https://github.com/leethomason/tinyxml2
// OpenStreetMap: https://www.openstreetmap.org
// OpenStreetMap docs:
//   https://wiki.openstreetmap.org/wiki/Main_Page
//   https://wiki.openstreetmap.org/wiki/Map_Features
//   https://wiki.openstreetmap.org/wiki/Node
//   https://wiki.openstreetmap.org/wiki/Way
//   https://wiki.openstreetmap.org/wiki/Relation
//

#include <iostream>
#include <iomanip>  /*setprecision*/
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <queue>
#include <utility>
#include <stack>

#include "tinyxml2.h"
#include "dist.h"
#include "graph.h"
#include "osm.h"


using namespace std;
using namespace tinyxml2;

//
// Implement your standard application here
//
struct prioritize  // you could also use a struct
{
public:
  bool operator()(const pair<long long, double>& p1, const pair<long long, double>& p2)const
  {
    return p1.second > p2.second; 
  }
};

void displayNode(map<long long, Coordinates>& Nodes, long long nodeID) 
{
  cout << " " <<nodeID << endl
      << " ( " << Nodes[nodeID].Lat << ", " << Nodes[nodeID].Lon << " )" << endl; 
}

void addNodesToGraph(map<long long, Coordinates>& Nodes, graph<long long, double>& G) 
{
  for(auto &it : Nodes) 
  {
    G.addVertex(it.first);
  }
}

void addFootwaysToGraph(map<long long, Coordinates>& Nodes, vector<FootwayInfo>& Footways, graph<long long, double>& G) 
{

  for(auto& fw : Footways) 
  {
    for(size_t i = 0; i < fw.Nodes.size()-1; i++) 
    {
      if(Nodes.count(fw.Nodes[i])!=0 && Nodes.count(fw.Nodes[i+1])!=0)
      {
        long long first = fw.Nodes[i];
        long long second = fw.Nodes[i+1];
        double dist = distBetween2Points(Nodes[first].Lat, Nodes[first].Lon, Nodes[second].Lat, Nodes[second].Lon);
        G.addEdge(fw.Nodes[i], fw.Nodes[i+1], dist);
        dist = distBetween2Points(Nodes[second].Lat, Nodes[second].Lon, Nodes[first].Lat, Nodes[first].Lon);
        G.addEdge(fw.Nodes[i+1], fw.Nodes[i], dist);
      }
    }
  }
}

// A helper function to search for a building by partial name or abbreviation
BuildingInfo searchBuilding(vector<BuildingInfo>& Buildings, string buildingName, bool& found) 
{
  BuildingInfo building;

  for(auto& b : Buildings) 
  {
    if(b.Abbrev == buildingName){
      building = b;
      found = true;
      break;
    }
    if(b.Fullname.find(buildingName) != string::npos)
    {
      building = b;
      found = true;
      break;
    }
  }
  return building;
}

long long closestNode(map<long long, Coordinates>& Nodes, vector<FootwayInfo>& Footways, BuildingInfo& source)
{
  double lat, lon;
  double min = INFINITY;
  double dist = 0;
  long long closest;
  for(auto& fw : Footways)
  {
    for(auto& fwN: fw.Nodes)
    {
      if(Nodes.count(fwN)!=0)
      {
        lat = Nodes[fwN].Lat;
        lon = Nodes[fwN].Lon;
        dist = distBetween2Points(lat, lon, source.Coords.Lat, source.Coords.Lon);
        if(dist < min)
        {
          min = dist;
          closest = fwN;
        }
      }
    }
  }
  return closest;
}

vector<long long> getPath(map<long long, long long>& predecessors, long long& endV)
{
  vector<long long> path;
  stack<long long> s;
  long long currentV; 
  currentV= endV;
  while(currentV != 0)
  {
    s.push(currentV);
    currentV = predecessors[currentV];
  }
  while(!s.empty())
  {
    path.push_back(s.top());
    s.pop();
  }
  return path;
}

void findDestination(vector<BuildingInfo>& Buildings, BuildingInfo& source1, BuildingInfo& source2, BuildingInfo& dest, set<string>& visited)
{
  BuildingInfo b1;
  b1.Coords = centerBetween2Points(source1.Coords.Lat, source1.Coords.Lon, source2.Coords.Lat, source2.Coords.Lon);

  double dist = 0;
  double Min_dist = INFINITY;
  for(auto& b : Buildings)
  {
    if(visited.count(b.Fullname) == 0)
    {
      dist = distBetween2Points(b1.Coords.Lat, b1.Coords.Lon, b.Coords.Lat, b.Coords.Lon);
      if(dist < Min_dist)
      {
        Min_dist = dist;
        dest = b;
      }
    }
  }
}

string Dijkstra(long long& startV, long long& endV,long long startV2, double& dist,graph<long long, double>& G)
{
  vector<long long> vertices = G.getVertices();

  priority_queue<pair<long long, double>, vector<pair<long long, double>>, prioritize>unvisited;
  set<long long> visited;

  map<long long, double> distances;
  map<long long, long long> predecessors;
  for(auto& currentV : vertices)
  {
    distances[currentV] = INFINITY;
    predecessors[currentV] = 0;
    unvisited.push(make_pair(currentV, INFINITY));
  }

  distances[startV] = 0;
  unvisited.push({startV, 0});

  while(!unvisited.empty())
  {
    pair<long long, double> currentV = unvisited.top();
    unvisited.pop();
    if(distances[currentV.first] == INFINITY)
    {
      break;
    }
    else if(visited.count(currentV.first))
    {
      continue;
    }
    else
    {
      visited.insert(currentV.first);
    }

    set<long long> hood = G.neighbors(currentV.first);
    for(auto& adjV : hood)
    {
      double edgeWeight = 0; 
      G.getWeight(currentV.first, adjV, edgeWeight);
      double alt = distances[currentV.first] + edgeWeight;
      if(alt < distances[adjV])
      {
        distances[adjV] = alt;
        predecessors[adjV] = currentV.first;
        unvisited.push({adjV, alt});
      }
    }
  }

  vector<long long> path = getPath(predecessors, endV);
  string pathString = "";
  for(size_t p = 0; p < path.size(); p++)
  {
    pathString += to_string(path[p]) + "->";
  }
  if(distances[startV2] >= INFINITY)
  {
    return "0";
  }
  if(distances[endV] >= INFINITY)
  {
    return "-1";
  }
  // dist = 0;
  // for(auto& p1: path)
  // {
  //   dist += distances[p1];
  // }


  return pathString;
}

void application(
    map<long long, Coordinates>& Nodes, vector<FootwayInfo>& Footways,
    vector<BuildingInfo>& Buildings, graph<long long, double>& G) 
{
  double d1, d2;
  string person1Building, person2Building;
  bool found1 = false;
  bool found2 = false;
  BuildingInfo source1, source2, dest;
  set<string> unreachable;

  
  addNodesToGraph(Nodes, G);
  addFootwaysToGraph(Nodes, Footways, G);

  int numVertices = G.NumVertices();
  int numEdges = G.NumEdges();

  cout << "# of nodes: " << Nodes.size() << endl;
  cout << "# of footways: " << Footways.size() << endl;
  cout << "# of buildings: " << Buildings.size() << endl;
  cout << "# of vertices: " << numVertices << endl;
  cout << "# of edges: " << numEdges << endl;

  cout << endl;
  cout << "Enter person 1's building (partial name or abbreviation), or #> ";
  getline(cin, person1Building);

  while (person1Building != "#") 
  {
 
    cout << "Enter person 2's building (partial name or abbreviation)> ";
    getline(cin, person2Building);

    

    source1 = searchBuilding(Buildings, person1Building, found1);
    source2 = searchBuilding(Buildings, person2Building, found2);
    if(!found1)
    {
      cout << "Person 1's building not found" << endl;
      cout << endl;
      cout << "Enter person 1's building (partial name or abbreviation), or #> ";
      getline(cin, person1Building);

      found2 = false;
      continue;
    }
    if(!found2)
    {
      cout << "Person 2's building not found" << endl;
      cout << endl;
      cout << "Enter person 1's building (partial name or abbreviation), or #> ";
      getline(cin, person1Building);

      continue;
    }

    else if(found1 && found2)
    {
    
      cout <<  "Person 1's point:" << endl
          << " " << source1.Fullname << endl
          << " " <<  "(" << source1.Coords.Lat << "," << source1.Coords.Lon << ")"<< endl;
    

    
      cout <<  "Person 2's point:" << endl
          << " " << source2.Fullname << endl
          << " " <<  "(" << source2.Coords.Lat << "," << source2.Coords.Lon << ")"<< endl;
    }


    if(found1 && found2)
    {
      findDestination(Buildings, source1, source2, dest, unreachable);
    }

      cout << "Destination Building:" << endl
          << dest.Fullname << endl
          << "(" << dest.Coords.Lat << "," << dest.Coords.Lon << ")"<< endl;
      long long src = closestNode(Nodes, Footways, source1);
      long long src2 = closestNode(Nodes, Footways, source2);
      long long end = closestNode(Nodes, Footways, dest);
      cout << endl;
      cout << "Nearest P1 node:" << endl;
      displayNode(Nodes, src);
      cout << "Nearest P2 node:" << endl;
      displayNode(Nodes, src2);
      cout << "Nearest destination node:" << endl;
      displayNode(Nodes, end);
      string test = Dijkstra(src,end,src2,d1, G);
      string test2 = Dijkstra(src2,end,src,d2, G);
      G.getWeight(src, end, d1);
      G.getWeight(src2, end, d2);
      if(test == "0" || test2 == "0")
      {
        cout << "Sorry, destination unreachable." << endl;
        cout << "Enter person 1's building (partial name or abbreviation), or #> ";
        getline(cin, person1Building);
        continue;
      }
      else if(test == "-1" || test2 == "-1"){
        while(test == "-1" || test2 == "-1")
        {
          cout << "At least one person was unable to reach the destination building. Finding next closest building..." << endl;
          unreachable.insert(dest.Fullname);
          findDestination(Buildings, source1, source2, dest, unreachable);
          end = closestNode(Nodes, Footways, dest);
          cout << "New Destination building:" << endl
              << dest.Fullname << endl
              << "(" << dest.Coords.Lat << "," << dest.Coords.Lon << ")"<< endl;
          cout << "Nearest destination node:" << endl;
          displayNode(Nodes, end);
          test = Dijkstra(src,end,src2,d1, G);
          test2 = Dijkstra(src2,end,src,d2, G);
        }
      }
      cout << "Person 1's distance to des: " << endl;
      cout << test << endl;
      cout << "Person 2's distance to des: " << endl;
      cout << test2 << endl;

    }


}    


int main() 
{
  graph<long long, double> G;

  // maps a Node ID to it's coordinates (lat, lon)
  map<long long, Coordinates>  Nodes;
  // info about each footway, in no particular order
  vector<FootwayInfo>          Footways;
  // info about each building, in no particular order
  vector<BuildingInfo>         Buildings;
  XMLDocument                  xmldoc;

  cout << "** Navigating UIC open street map **" << endl;
  cout << endl;
  cout << std::setprecision(8);

  string def_filename = "map.osm";
  string filename;

  cout << "Enter map filename> ";
  getline(cin, filename);

  if (filename == "") {
    filename = def_filename;
  }

  //
  // Load XML-based map file
  //
  if (!LoadOpenStreetMap(filename, xmldoc)) {
    cout << "**Error: unable to load open street map." << endl;
    cout << endl;
    return 0;
  }

  //
  // Read the nodes, which are the various known positions on the map:
  //
  int nodeCount = ReadMapNodes(xmldoc, Nodes);

  //
  // Read the footways, which are the walking paths:
  //
  int footwayCount = ReadFootways(xmldoc, Footways);

  //
  // Read the university buildings:
  //
  int buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);

  //
  // Stats
  //
  assert(nodeCount == (int)Nodes.size());
  assert(footwayCount == (int)Footways.size());
  assert(buildingCount == (int)Buildings.size());

  cout << endl;



  //
  // TO DO: build the graph, output stats:
  //


  // cout << "# of vertices: " << G.NumVertices() << endl;
  // cout << "# of edges: " << G.NumEdges() << endl;
  cout << endl;

  // Execute Application
  application(Nodes, Footways, Buildings, G);

  //
  // done:
  //
  cout << "** Done **" << endl;
  return 0;
}
