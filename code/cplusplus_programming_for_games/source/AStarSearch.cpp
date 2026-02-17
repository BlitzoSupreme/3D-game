#include "AStarSearch.h"
#include <queue>
#include <iostream>
#include <fstream>

// checks if player can move to position
bool isValid(const Map& theMap, int x, int y)
{
	if (x < 0 || x >= 32) return false;
	if (y < 0 || y >= 24) return false;

	// block walls (1) and breakables (3)
	int t = theMap[y][x];
	if (t == 1 || t == 3) return false;

	return true;
}

// Indicates if the position x,y corresponds to the destination
bool isDestination(int x, int y, Node destination)
{
	if (x == destination.x && y == destination.y) {
		return true;
	}
	return false;
}

float calculateH(int x, int y, Node destination)
{
	int xDist = max(x, destination.x) - min(x, destination.x);
	int yDist = max(y, destination.y) - min(y, destination.y);

	return xDist + yDist;
}

//works out pathing
vector<Node> makePath(array<array<Node, 32>, 24>& map, Node dest)
{
	// Stores the path
	vector<Node> usablePath;

	// Start at the destination
	int x = dest.x;
	int y = dest.y;

	//finds required nodes to make path
	while (!(map[y][x].parentX == x && map[y][x].parentY == y)
		&& map[y][x].x != -1 && map[y][x].y != -1)
	{
		// Put the node in the list
		usablePath.push_back(map[y][x]);

		int tempX = map[y][x].parentX;
		int tempY = map[y][x].parentY;

		// Move to parent for next repetition
		x = tempX;
		y = tempY;
	}

	// Push the start node into the list
	usablePath.push_back(map[y][x]);

	// Reverse the list so in start to dest order
	reverse(usablePath.begin(), usablePath.end());

	return usablePath;
}


array<array<Node, 32>, 24> nodeDetails;

array<array<bool, 32>, 24> closedList;

vector<Node> emptyPath;


vector<Node> aStar(const Map& theMap, const Node& player, const Node& dest) {
	if (!isValid(theMap, dest.x, dest.y)) {
		return emptyPath;
	}

	if (isDestination(player.x, player.y, dest)) {
		return emptyPath;
	}

	// Initialise the helper arrays
	for (int y = 0; y < 24; y++) {
		for (int x = 0; x < 32; x++) {

			nodeDetails[y][x].x = x;
			nodeDetails[y][x].y = y;

			nodeDetails[y][x].fCost = FLT_MAX;
			nodeDetails[y][x].gCost = FLT_MAX;
			nodeDetails[y][x].hCost = FLT_MAX;

			nodeDetails[y][x].parentX = -1;
			nodeDetails[y][x].parentY = -1;
			closedList[y][x] = false;
		}
	}

	// list of nodes to visit
	vector<Node> openList;

	// Initialise the starting point to the player position
	nodeDetails[player.y][player.x].x = player.x;
	nodeDetails[player.y][player.x].y = player.y;
	nodeDetails[player.y][player.x].fCost = 0.0f;
	nodeDetails[player.y][player.x].gCost = 0.0f;
	nodeDetails[player.y][player.x].hCost = 0.0f;
	nodeDetails[player.y][player.x].parentX = player.x;
	nodeDetails[player.y][player.x].parentY = player.y;

	// Put the start node into the open list to start the algorithm
	openList.push_back(nodeDetails[player.y][player.x]);

	// Indicates if the destination was found.
	bool found = false;

	// While there are nodes to process
	while (!openList.empty()) {
		// Get one from the list
		Node current = openList.front();

		// Remove it from the list so it isn't processed again
		openList.erase(openList.begin());

		// Indicate visited
		closedList[current.y][current.x] = true;

		// Is the node the destination?
		if (isDestination(current.x, current.y, dest)) {
			// Indicate found it if it is
			found = true;

			// And exit the loop
			break;
		}

	    //checks surrounding
		if (isValid(theMap, current.x, current.y - 1) && !closedList[current.y - 1][current.x]) {
			float gNew = nodeDetails[current.y][current.x].gCost + 1.0f;
			float hNew = calculateH(current.x, current.y - 1, dest);
			float fNew = gNew + hNew;

			if (nodeDetails[current.y - 1][current.x].fCost == FLT_MAX || nodeDetails[current.y - 1][current.x].fCost > fNew) {

				nodeDetails[current.y - 1][current.x].fCost = fNew;
				nodeDetails[current.y - 1][current.x].gCost = gNew;
				nodeDetails[current.y - 1][current.x].hCost = hNew;
				nodeDetails[current.y - 1][current.x].parentX = current.x;
				nodeDetails[current.y - 1][current.x].parentY = current.y;

				openList.push_back(nodeDetails[current.y - 1][current.x]);
			}
		}

		// Right
		if (isValid(theMap, current.x + 1, current.y) && !closedList[current.y][current.x + 1]) {
			float gNew = nodeDetails[current.y][current.x].gCost + 1.0f;
			float hNew = calculateH(current.x + 1, current.y, dest);
			float fNew = gNew + hNew;

			if (nodeDetails[current.y][current.x + 1].fCost == FLT_MAX || nodeDetails[current.y][current.x + 1].fCost > fNew) {
	
				nodeDetails[current.y][current.x + 1].fCost = fNew;
				nodeDetails[current.y][current.x + 1].gCost = gNew;
				nodeDetails[current.y][current.x + 1].hCost = hNew;
				nodeDetails[current.y][current.x + 1].parentX = current.x;
				nodeDetails[current.y][current.x + 1].parentY = current.y;

				openList.push_back(nodeDetails[current.y][current.x + 1]);
			}
		}

		// Down
		if (isValid(theMap, current.x, current.y + 1) && !closedList[current.y + 1][current.x]) {
			float gNew = nodeDetails[current.y][current.x].gCost + 1.0f;
			float hNew = calculateH(current.x, current.y + 1, dest);
			float fNew = gNew + hNew;

			if (nodeDetails[current.y + 1][current.x].fCost == FLT_MAX || nodeDetails[current.y + 1][current.x].fCost > fNew) {

				nodeDetails[current.y + 1][current.x].fCost = fNew;
				nodeDetails[current.y + 1][current.x].gCost = gNew;
				nodeDetails[current.y + 1][current.x].hCost = hNew;
				nodeDetails[current.y + 1][current.x].parentX = current.x;
				nodeDetails[current.y + 1][current.x].parentY = current.y;

				openList.push_back(nodeDetails[current.y + 1][current.x]);
			}
		}

		// Left
		if (isValid(theMap, current.x - 1, current.y) && !closedList[current.y][current.x - 1]) {
			float gNew = nodeDetails[current.y][current.x].gCost + 1.0f;
			float hNew = calculateH(current.x - 1, current.y, dest);
			float fNew = gNew + hNew;

			if (nodeDetails[current.y][current.x - 1].fCost == FLT_MAX || nodeDetails[current.y][current.x - 1].fCost > fNew) {

				nodeDetails[current.y][current.x - 1].fCost = fNew;
				nodeDetails[current.y][current.x - 1].gCost = gNew;
				nodeDetails[current.y][current.x - 1].hCost = hNew;
				nodeDetails[current.y][current.x - 1].parentX = current.x;
				nodeDetails[current.y][current.x - 1].parentY = current.y;

				openList.push_back(nodeDetails[current.y][current.x - 1]);
			}
		}

	}

	// Out of loop.  Was the destination found?
	if (found) {
		// Yes, then create the path for the current node state
		return makePath(nodeDetails, dest);
	}
	else {
		// No, return no path
		return emptyPath;
	}
}