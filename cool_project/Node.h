#ifndef NODE_H
#define NODE_H

#include <set>
#include <iostream>

using namespace std;

class Node {
public:
	Node();
	Node(double lat, double lng, int id);
	Node(const Node& other);
	Node(Node& other);

	bool operator < (const Node& other) const;
	bool operator == (const Node& other) const;
	friend ostream& operator << (ostream& stream, const Node& n);

	bool visited;
	int id;
	double lat;
	double lng;
	set<Node*> connections;
};

#endif