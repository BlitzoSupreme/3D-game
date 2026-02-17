#pragma once

#include <algorithm>
#include <vector>
#include <array>
#include <stack>
#include "Map.h"

using namespace std;

struct Node {
	int x, y; 
	int parentX, parentY;
	float gCost; 
	float hCost; 
	float fCost; 
};

bool isValid(Map& map, int x, int y);

bool isDestination(int x, int y, Node destination);

float calculateH(int x, int y, Node destination);

vector<Node> makePath(array<array<Node, 32>, 24>& map, Node dest);

// Main A* algorithm
vector<Node> aStar(const Map& theMap, const Node& player, const Node& dest);