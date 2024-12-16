#include "Node.h"
#include <iostream>

using namespace std;


Node::Node(double lat, double lng, int id) {
	this->lat = lat;
	this->lng = lng;
	this->id = id;
	visited = false;
}

Node::Node() {
	lat = 0;
	lng = 0;
	id = 0;
	visited = false;
}

bool Node::operator < (const Node& other) const {
	if (this->lat < other.lat) return true;
	else if (this->lat == other.lat) {
		if (this->lng < other.lng) return true;
		else return false;
	}
	else return false;
}

bool Node::operator == (const Node& other) const {
	bool isLatSame = lat == other.lat;
	bool isLngSame = lng == other.lng;
	return isLatSame && isLngSame;
}

Node::Node(const Node& other) {
	//cout << "Const Copy Constructor Called" << endl;
	id = other.id;
	lat = other.lat;
	lng = other.lng;
	visited = other.visited;
	connections = other.connections;
}

Node::Node(Node& other) {
	//cout << "Copy Constructor Called" << endl;
	id = other.id;
	lat = other.lat;
	lng = other.lng;
	visited = other.visited;
	connections = other.connections;
}

ostream& operator << (ostream& stream, const Node& n) {
	stream << '(' << n.lat << ", " << n.lng << ')';
	return stream;
}