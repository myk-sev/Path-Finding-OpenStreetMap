#include "Street.h"
#include <iostream>

using namespace std;

Street::Street(json coordinates) {
	start.lat = coordinates[0][0];
	start.lng = coordinates[0][1];
	end.lat = coordinates[coordinates.size() - 1][0];
	end.lng = coordinates[coordinates.size() - 1][1];
	for (auto coord : coordinates) {
		body.emplace_back(coord[0], coord[1]);
	}
}

Coordinate Street::getStart() const {
	return start;
}

Coordinate Street::getEnd() const {
	return end;
}

vector<Coordinate> Street::getBody() const {
	return body;
}

ostream& operator << (ostream& stream, const Street& obj) {
	stream << "Start: (" << obj.start.lat << ", " << obj.start.lng << ")" << endl;
	cout << "Body: [";
	for (auto coord : obj.body) {
		stream << coord;
		if (coord != obj.body[obj.body.size() -1]) stream << ", "; //only places comma if the coordinate is no the last in the list
	}
	stream << "]" << endl;
	stream << "End: (" << obj.end.lat << ", " << obj.end.lng << ")" << endl;
	return stream;
}