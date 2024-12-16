#include "raylib-cpp.hpp"
#include <nlohmann/json.hpp>

#include <array>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <set>
#include <stack>
#include <string>
#include <vector>

#include "Coordinate.h"
#include "Street.h"
#include "Node.h"


using namespace std;
using json = nlohmann::json;

string FILE_NAME = "road_line.geojson";

const int FPS = 60;
const int SCREEN_WIDTH = 1500;
const int SCREEN_HEIGHT = 1500;
const int LOGO_OFFSET = 500;

map<array<double, 2>, Node> construct_graph(vector<Street>& roadData);
vector<Street> extractRoads(ifstream& dataFile);
vector<Node*> extractRoute(Node startNode, Node targetNode);
vector<Node*>& DFS(stack<Node*>& stck, vector<Node*>& route, const Node& target);


//raylib::Texture logo("raylib_logo.png");
//obj.DrawLine(0, 0, 1600, 450, DARKGREEN);

int main() {
	ifstream dataFile(FILE_NAME);
	vector<Street> roadData = extractRoads(dataFile);

	map<array<double, 2>, Node> nodeMap = construct_graph(roadData);

	vector<Node> nodeList; //will be used later for batch updates that require index based iteration
	for (auto pair : nodeMap) nodeList.push_back(pair.second);


	//########### Coordinate Normalization ###########
	double maxLat = -100;
	double minLat = 0;
	double maxLng = 0;
	double minLng = 100;

	for (auto pair : nodeMap) {
		if (pair.second.lat > maxLat) maxLat = pair.second.lat;
		if (pair.second.lat < minLat) minLat = pair.second.lat;
		if (pair.second.lng > maxLng) maxLng = pair.second.lng;
		if (pair.second.lng < minLng) minLng = pair.second.lng;
	}

	double latRange = maxLat - minLat;
	double lngRange = maxLat - minLat;

	int framesCounter = 0;
    raylib::Window window(SCREEN_WIDTH, SCREEN_HEIGHT, "Depth First Search - Open Street Data");
    
    SetTargetFPS(FPS);

	while (!window.ShouldClose())
	{
		framesCounter++;
		BeginDrawing();
		DrawText("Pathfinding on OpenStreetMap Data", 25, 0, 80, BLUE);

		//########### Draw Street Graph ###########
		raylib::Color mapDisplay = DARKGREEN;
		float normalizedLatStart;
		float normalizedLngStart;
		float normalizedLatEnd;
		float normalizedLngEnd;

		Node n;
		for (int i = 0; i < nodeList.size(); i++) {
			if (i % 10 == 0) BeginDrawing();
			n = nodeList[i];
			normalizedLatStart = (n.lat - minLat) / latRange * SCREEN_WIDTH;
			normalizedLngStart = (n.lng - minLng) / lngRange * SCREEN_HEIGHT + LOGO_OFFSET;
			for (auto connection : n.connections) {
				normalizedLatEnd = (connection->lat - minLat) / latRange * SCREEN_WIDTH;
				normalizedLngEnd = (connection->lng - minLng) / lngRange * SCREEN_HEIGHT + LOGO_OFFSET;
			}
			mapDisplay.DrawLine(Vector2{ normalizedLatStart , normalizedLngStart }, Vector2{ normalizedLatEnd , normalizedLngEnd }, 3);
			if (i > 0 && (i + 1) % 10 == 0) EndDrawing();
		}


		EndDrawing();

		//########### set start & end points ###########
		array<double, 2> startNode = { roadData[0].getStart().lat, roadData[0].getStart().lng };
		array<double, 2> targetNode = { roadData[100].getEnd().lat, roadData[100].getEnd().lng };

		cout << endl << endl;
		cout << "Start Node: " << nodeMap[startNode].id << " " << nodeMap[startNode] << endl;
		cout << "Target Node: " << nodeMap[targetNode].id << " " << nodeMap[targetNode] << endl;


		//########### prime data structures ###########
		stack<Node*> stck;
		vector<Node*> route;
		//route.push_back(&(nodeMap[startNode]));
		//for (Node* n : nodeMap[startNode].connections) stck.push(n);

		//vector<Node*> fakeRoute;
		//fakeRoute.push_back(&nodeList[0]);
		//for (int i = 0; i < 100; i++) {
		//	Node* nextNode = fakeRoute[0]->connections._Get_container_()[fakeRoute[0]->connections.size() - 1];
		//	fakeRoute.push_back(nextNode);
		//}

		vector<Node*> finalRoute;
		for (Node* n : extractRoute(nodeMap[startNode], nodeMap[targetNode])) finalRoute.push_back(n);


		cout << "Final Route Confirmation (" << finalRoute.size() << "): " << endl;
		for (Node* n : finalRoute) cout << '\t' << n->id << " " << *n << endl;

		cout << "Press enter to continue to route display: ";
		string temp;
		cin >> temp;

		//########## Draw Route Overlay ###########
		raylib::Color routeDisplay = BLUE;
		Node currentNode;
		Node prevNode;
		for (int i = 1; i < finalRoute.size(); i++) {
			BeginDrawing();
			normalizedLatStart = (finalRoute[i - 1]->lat - minLat) / latRange * SCREEN_WIDTH;
			normalizedLngStart = (finalRoute[i - 1]->lng - minLng) / lngRange * SCREEN_HEIGHT + LOGO_OFFSET;
			normalizedLatEnd = (finalRoute[i]->lat - minLat) / latRange * SCREEN_WIDTH;
			normalizedLngEnd = (finalRoute[i]->lng - minLng) / lngRange * SCREEN_HEIGHT + LOGO_OFFSET;

			routeDisplay.DrawLine(Vector2{ normalizedLatStart , normalizedLngStart }, Vector2{ normalizedLatEnd , normalizedLngEnd }, 5);
			EndDrawing();
		}
		cout << "Drawing complete" << endl << endl;
		cout << "To close the window please enter any key: ";
		cin >> temp;;
		CloseWindow();
	}
  
    return 0;
}

map<array<double, 2>, Node> construct_graph(vector<Street>& roadData) {
	////########## Graph Construction ##########
	cout << "Graph Construction In Progress..." << endl;
	map<array<double, 2>, Node> flattenedGraph;
	array<double, 2> prevCoord;
	array<double, 2> currentCoord;

	int count = 0;
	for (Street street : roadData) {
		vector<Coordinate> streetCoords = street.getBody();
		for (int i = 0; i < streetCoords.size(); i++) {
			currentCoord[0] = streetCoords[i].lat;
			currentCoord[1] = streetCoords[i].lng;
			if (flattenedGraph.find(currentCoord) == flattenedGraph.end()) {
				flattenedGraph[currentCoord] = Node(streetCoords[i].lat, streetCoords[i].lng, count);
				count++;
			}
			//add forward and backword connections
			if (i > 0) {
				prevCoord[0] = streetCoords[i - 1].lat;
				prevCoord[1] = streetCoords[i - 1].lng;
				flattenedGraph[currentCoord].connections.insert(&flattenedGraph[prevCoord]);
				flattenedGraph[prevCoord].connections.insert(&flattenedGraph[currentCoord]);
			}
		}
	}
	cout << "Graph Construction Complete" << endl;
	return flattenedGraph;
}

vector<Node*> extractRoute(Node startNode, Node targetNode) {
	stack<Node*> stck;
	vector<Node*> route;

	//prime data structures
	route.push_back(&startNode);
	for (Node* n : startNode.connections) stck.push(n);

	route = DFS(stck, route, targetNode);
	cout << endl << endl;

	return route;
}

vector<Node*>& DFS(stack<Node*>& stck, vector<Node*>& route, const Node& target) {
	Node* next;
	//get next potential node in route
	do {
		next = stck.top();
		stck.pop();

	} while (next->visited); //loops through stack until a new node is pulled 

	if (route[route.size() - 1]->connections.find(next) == route[route.size() - 1]->connections.end()) {
		route.pop_back(); //if next node is not a connection of the previous node in the route then that node has been exhausted
	}

	route.push_back(next);
	next->visited = true; //prevents circular looping

	if (*next == target) return route; //if the node is the target node return the route
	else {
		stack<Node*> prevStack = stck;

		for (Node* n : next->connections) {
			stck.push(n);
		}
		if (prevStack.size() == stck.size()) route.pop_back(); //if node is a end node and not the final node remove it from the route
		route = DFS(stck, route, target);
		return route;
	}

}

void generateStreetLengthDistribution(vector<Street> allRoads) {
	//utility function
	int numOfCoords;
	map<int, int> streetLengths;
	for (Street street : allRoads) {
		numOfCoords = street.getBody().size();

		if (streetLengths.find(numOfCoords) == streetLengths.end()) streetLengths[numOfCoords] = 1;
		else streetLengths[numOfCoords]++;
	}

	cout << "Street Length Distribution" << endl;
	for (auto pair : streetLengths) {
		cout << pair.first << ": " << pair.second << endl;
	}
}


vector<Street> extractRoads(ifstream& dataFile) {
	cout << "Street extraction starting..." << endl;

	json dataJSON = json::parse(dataFile); //read data into program
	json allNodes = dataJSON.at("features"); //json entries related to paths
	vector<json> streetJSONS;
	vector<json> streetLists; //all data used by this library is of type "json"

	//extracts only paths with a speedlimit 
	for (auto node : allNodes) {
		if (node.at("properties").at("maxspeed").type() != json::value_t::null) { //but! different json values have different value_t's
			streetJSONS.push_back(node);										  //this is used to extract out only non null values
		}
	}
	cout << "Street Node Count: " << streetJSONS.size() << endl;

	json street; //list of coordinates
	for (auto node : streetJSONS) {
		street = node.at("geometry").at("coordinates");
		streetLists.push_back(street);
	}

	vector<Street> streets;
	for (auto node : streetLists) {
		streets.emplace_back(node);
	}

	cout << "Street extraction complete." << endl << endl;

	return streets;
}