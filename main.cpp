#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>

struct World {
    int X, Y; // map limits
	int** map; // ** map
	World(int x = 0, int y = 0, int **map = NULL) : X(x), Y(y), map(map) { }
	void draw(std::ostream& o) { // insert map into output stream
		for (int y = 0; y < Y; y++) {
			for (int x = 0; x < X; x++) {
				o << (map[y][x] == 1 ? "[]" : "  ");
				if (x == X - 1) o << std::endl;
			}
		}
	}
	void dealloc() { // deallocate structure
		if (Y > 0) {
			for (int i = 0; i < Y; i++) delete[] map[i];
			delete[] map;
		}
	}
};

class Vector2i {
    private:
        int x, y; // vector cords
    public:
        Vector2i(int _x = -1, int _y = -1) : x(_x), y(_y) {}
        bool operator==(const Vector2i& v) { // set vector to vector
            return (x == v.x && y == v.y);
        }
        void operator+=(const Vector2i& v) { // add vector to vector
            x += v.x;
            y += v.y;
        }
        double distance(const Vector2i& v) { // calculate distance of 2 vectors
            double dx = x - v.x, dy = y - v.y;
            return sqrt(dx * dx + dy * dy);
        }
        inline int X() const { return x; } // return private x / y
        inline int Y() const { return y; }
        bool inBounds(int minx, int maxx, int miny, int maxy) { // check if cords are in set bounds
            return (x >= minx && x <= maxx && y >= miny && y <= maxy);
        }
};

struct Node {
	Vector2i pos; // position of node
	Node *parent; // parent of node
	int gCost, hCost; // costs of node
	int xStep, yStep; // parent / this node cords change
	Node(const Vector2i& _pos, int _gCost = 0, int _hCost = 0) {
		this->pos = _pos;
		this->gCost = _gCost;
		this->hCost = _hCost;
	}
	inline int fCost() const { return this->gCost + this->hCost; } // get f cost
	int linearDistance(Node* n) { // calculate linear distance of two nodes
		int dx = abs(pos.X() - n->pos.X());
		int dy = abs(pos.Y() - n->pos.Y());
		if (dx > dy) return 14 * dy + 10 * (dx - dy);
		else return 14 * dx + 10 * (dy - dx);
	}
	bool inList(std::vector<Node*> n) { // check if node is in node vector (by vector)
		for (unsigned int i = 0; i < n.size(); i++) {
			if (n.at(i)->pos == pos) return true;
		}
		return false;
	}
	void dealloc() { // deallocate structure
        delete this;
	}
};



std::vector<Node*> getNeighbourNodes(World world, Node* n) {
	std::vector<Node*> neighbours;
	int moves[4][2] = { { 1, 0 },{ -1, 0 },{ 0, 1 },{ 0, -1 } }; // straight lines only move map
	for (int i = 0; i < 4; i++) {
		Vector2i v(n->pos); // copy parents vector and add d for neighbor vector
		v += Vector2i(moves[i][0], moves[i][1]);
		if (!v.inBounds(0, world.X - 1, 0, world.Y - 1) || v == n->pos) continue; // check if d is in bounds or lays over (not used with move map)
		else if (world.map[v.Y()][v.X()] == 1) continue; // continue if tile is solid

		Node* node = new Node(v); // create new instance of node and fill with x / y value
		node->xStep = moves[i][0];
		node->yStep = moves[i][1];
		neighbours.push_back(node);
	}

	return neighbours;
}

std::string findPath(World world, const Vector2i& start, const Vector2i& target) {
	Node* startNode = new Node(start); // create start and end nodes from input vectors
	Node* targetNode = new Node(target);
	std::string path; // output path (nodes are internal and destroyed in the end)
	std::vector<Node*> openSet, closedSet; // checked and possible nodes
	openSet.push_back(startNode);

	while (openSet.size() > 0) { // if there are nodes to check
		unsigned int currentNodePosition = 0;
		Node* currentNode = openSet.at(currentNodePosition); // retrieve possible neighbor nodes
		for (unsigned int i = 0; i < openSet.size(); i++) {
			if (openSet.at(i)->fCost() < currentNode->fCost() || (openSet.at(i)->fCost() == currentNode->fCost() && openSet.at(i)->hCost < currentNode->hCost)) {
				currentNode = openSet.at(i); // get lowest f among all found nodes (not checked) and pick that node
				currentNodePosition = i;
			}
		}

		openSet.erase(openSet.begin() + currentNodePosition); // remove current node
		closedSet.push_back(currentNode); // insert current node to checked list

		if (currentNode->pos == targetNode->pos) { // end if current node is matching target node
			do {
				char s = '-';
				if (currentNode->xStep == 1) s = 'E'; // translate d in cords into chars and output them into output string
				if (currentNode->xStep == -1) s = 'W';
				if (currentNode->yStep == 1) s = 'S';
				if (currentNode->yStep == -1) s = 'N';
				path.insert(path.begin(), s);
				currentNode = currentNode->parent; // recurse back to the start node
			} while (!(currentNode->pos == start));

			for (unsigned int i = 0; i < openSet.size(); i++) openSet.at(i)->dealloc(); // deallocate all nodes
			for (unsigned int i = 0; i < closedSet.size(); i++) closedSet.at(i)->dealloc();
			targetNode->dealloc();

			return path;
		}

		std::vector<Node*> neighbours = getNeighbourNodes(world, currentNode); // get possible neighbors of current node (currently limited by move map)
		for (unsigned int i = 0; i < neighbours.size(); i++) { // check all neighbor nodes
			Node* neighbour = neighbours.at(i);
			if (neighbour->inList(closedSet)) { // check if node is in checked list, if so, deallocate current copy.
				neighbour->dealloc();
				continue;
			}
			int costCurrentToNeighbour = currentNode->gCost + currentNode->linearDistance(neighbour); // calculate cost of neighbor
			if (costCurrentToNeighbour < neighbour->gCost || !neighbour->inList(openSet)) { // use the node if costs are lower, or node is not in open set
				neighbour->gCost = costCurrentToNeighbour; // set all costs & parent node
				neighbour->hCost = neighbour->linearDistance(targetNode);
				neighbour->parent = currentNode;
				if (!neighbour->inList(openSet)) openSet.push_back(neighbour); // if neighbor is not in open list, insert it there
			}
		}
	}

	for (unsigned int i = 0; i < openSet.size(); i++) openSet.at(i)->dealloc(); // deallocate all nodes
	for (unsigned int i = 0; i < closedSet.size(); i++) closedSet.at(i)->dealloc();
	targetNode->dealloc();

	return path; // path is blank (not found any possible way)
}


int worldMap[12][12] = { // static map layout
	{ 1,1,1,1,1,1,1,1,1,1,1,1 },
	{ 1,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,0,1,1,1,1,1,1,0,1,1,1 },
	{ 1,0,1,0,0,0,0,0,0,0,0,1 },
	{ 1,0,1,0,1,1,1,1,1,1,0,1 },
	{ 1,0,1,0,1,0,0,0,0,0,0,1 },
	{ 1,0,0,0,1,1,0,1,1,1,0,1 },
	{ 1,0,1,0,0,0,0,1,0,1,1,1 },
	{ 1,0,1,1,0,1,0,0,0,0,0,1 },
	{ 1,0,1,0,0,1,1,1,1,1,0,1 },
	{ 1,0,0,0,1,1,0,0,0,0,0,1 },
	{ 1,1,1,1,1,1,1,1,1,1,1,1 }
};

/// MAIN

int main()
{
	int **dW = new int *[12]; // dynamic **, static / dynamic conversion
	for (int i = 0; i < 12; i++) {
		dW[i] = new int[12];
		for (int j = 0; j < 12; j++) {
			dW[i][j] = worldMap[i][j];
		}
	}


	World world(12, 12, dW); // create new world
	world.draw(std::cout); // draw the world
	Vector2i begin(1, 1), end(10, 10); // create initial vectors
	std::string n = findPath(world, begin, end); // find path between nodes (best possible)
	world.dealloc(); // deallocate world

	std::cout << "A* Algorithm" << std::endl << std::endl;
	std::cout << std::endl << "Shortest path: " << n.size() << " nodes" << std::endl << "Steps: " << n;

	return 0;
}
