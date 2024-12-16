#ifndef STREET_H
#define STREET_H

#include "Coordinate.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

class Street {
	friend ostream& operator <<(ostream& stream, const Street& obj);

public:
	Street(json coordinates);
	//friend ostream& operator <<(ostream& stream, const Street& obj);
	//bool operator == (const Street& other);
	Coordinate getStart() const;
	Coordinate getEnd() const;
	vector<Coordinate> getBody() const;


private:
	vector<Coordinate> body;
	Coordinate start;
	Coordinate end;
};

#endif