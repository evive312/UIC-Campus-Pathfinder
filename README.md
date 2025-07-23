<h1> UIC Campus Pathfinder</h1>
<h2>Description</h2>
Project is a C++ console program that lets two people meet on the UIC campus.  
Given an OpenStreetMap (`.osm`) file, each user enters a building name or abbreviation.  
The program:

1. Parses the map with **TinyXML-2**.
2. Builds a weighted, directed graph of footway nodes.
3. Finds the nearest footway node to each person and to the best “meet-in-the-middle” destination building.
4. Runs **Dijkstra’s algorithm** (implemented from scratch) to output the shortest walking paths and distances.

If the closest destination is unreachable, the algorithm automatically tries the next-closest building.

<br />


<h2>Languages and Libraries Used</h2>

- <b>C++17</b>  
- <b>TinyXML-2</b> (XML parsing)  
- <b>OpenStreetMap</b> data  
- Custom <b>graph</b> &amp; <b>Dijkstra</b> implementation  

<h2>Environment Used</h2>

- <b>Ubuntu 22.04 / g++ 11</b> (tested)  
