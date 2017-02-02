#include <iostream>
#include <vector>

using namespace std;

struct node {
    private:
        double F;
    public:
        double h, g;
        int x, y, parent, status, pos;
        char move;
        node (double _f) : F(_f) {}
        node (int _x, int _y, int _parent, int _status = 0) {
            x = _x;
            y = _y;
            parent = _parent;
            F = 0;
            h = 0;
            g = 0;
            status = _status;
        }
        double f() {
            return F;
        }
};

int open(vector<node>& n) {
    int num = 0;
    for (unsigned int i = 0; i < n.size(); i++) if (n.at(i).status == 0) num++;
    return num;
}

bool isInside (vector<node>& nl, const node& n, int status) {
    for (unsigned int i = 0; i < nl.size(); i++) {
        if (nl.at(i).x == n.x && nl.at(i).y == n.y && nl.at(i).status == status) return true;
    }
    return false;
}

string findPath (int ** world, int worldX, int worldY, int startX, int startY, int endX, int endY) {
    vector<node> nodelist;
    string path = "";
    nodelist.push_back(node(startX, startY, -1));
    while (open(nodelist) > 0) {
        node n(9999);
        for (unsigned int i = 0; i < nodelist.size(); i++) if (nodelist.at(i).status == 0 && nodelist.at(i).f() < n.f()) {
            n = nodelist.at(i);
            n.pos = i;
        }
        nodelist.at(n.pos).status = 1;
        if (n.x == endX && n.y == endY) {
            do {
                path.insert(path.begin(), n.move);
                n = nodelist.at(n.parent);
            } while (n.parent != 0);
            return path;
        }
        int moves[4][3] = {{1, 0, 'S'}, {-1, 0, 'N'}, {0, 1, 'E'}, {0, -1, 'W'}};
        for (unsigned int i = 0; i < 4; i++) {
            if (!(n.x + moves[i][0] >= 0 && n.x + moves[i][0] < worldX && n.y + moves[i][1] >= 0 && n.y + moves[i][1] < worldY)) continue;
            if (world[n.x + moves[i][0]][n.y + moves[i][1]] == 0) {
                node n2(n.x + moves[i][0], n.y + moves[i][1], n.pos);
                if (isInside(nodelist, n2, 1)) continue;
                int dx = (n.x - n2.x < 0 ? -(n.x - n2.x) : n.x - n2.x), dy = (n.y - n2.y < 0 ? -(n.y - n2.y) : n.y - n2.y);
                int cost1 = n.g + (dx > dy ? (14 * dy + 10 * (dx - dy)) : (14 * dx + 10 * (dy - dx)));
                bool isNotIn = !isInside(nodelist, n2, 0);
                if ((cost1 < n2.g && isNotIn) || isNotIn) {
                    n2.g = cost1;
                    dx = (n2.x - endX < 0 ? -(n2.x - endX) : n2.x - endX);
                    dy = (n2.y - endY < 0 ? -(n2.y - endY) : n2.y - endY);
                    n2.h = (dx > dy ? (14 * dy + 10 * (dx - dy)) : (14 * dx + 10 * (dy - dx)));
                    n2.parent = n.pos;
                    n2.status = 0;
                    n2.move = moves[i][2];
                    n2.pos = nodelist.size();
                    nodelist.push_back(n2);
                }
            }
        }
    }
    return path;
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

int main()
{
    int **dW = new int *[12];
	for (int i = 0; i < 12; i++) {
		dW[i] = new int[12];
		for (int j = 0; j < 12; j++) {
			dW[i][j] = worldMap[i][j];
		}
	}
	for (int x = 0; x < 12; x++) {
        for (int y = 0; y < 12; y++) {
            cout << (worldMap[x][y] == 1 ? "[]" : "  ");
        }
        cout << endl;
	}
    cout << "Path: " << findPath(dW, 12, 12, 1, 1, 10, 10) << endl;
    return 0;
}
