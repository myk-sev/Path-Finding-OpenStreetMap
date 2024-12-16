#ifndef COORDINATE_H
#define COORDINATE_H

#include <iostream>

using namespace std;

struct Coordinate {
public:
	Coordinate();
	Coordinate(double lat, double lng);
	double lat = 0;
	double lng = 0;
	bool operator == (const Coordinate& other) const;
	bool operator != (const Coordinate& other) const;
	friend ostream& operator << (ostream& stream, const Coordinate& obj);
};

#endif