#include "Coordinate.h"
#include <iostream>

using namespace std;

Coordinate::Coordinate() {
	this->lat = 0;
	this->lng = 0;
}

Coordinate::Coordinate(double lat, double lng) {
	this->lat = lat;
	this->lng = lng;
}

bool Coordinate::operator == (const Coordinate& other) const {
	bool latMatch = this->lat == other.lat;
	bool lngMatch = this->lng == other.lng;
	return latMatch && lngMatch;
}

bool Coordinate::operator != (const Coordinate& other) const {
	bool latMatch = this->lat == other.lat;
	bool lngMatch = this->lng == other.lng;
	return !(latMatch && lngMatch);
}

ostream& operator << (ostream& stream, const Coordinate& obj) {
	stream << "(" << obj.lat << ", " << obj.lng << ")";
	return stream;
}