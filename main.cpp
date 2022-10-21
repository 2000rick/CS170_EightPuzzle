#include <iostream>
#include <vector>
#include <queue>
using namespace std;

class Node {
    public:
        vector<vector<int>> initial_state;
        vector<vector<int>> state;
        vector<pair<int, int>> OPERATORS = {
             {0, -1}, {0, 1}, {1, 0}, {-1, 0} // left, right, down, up
        };
        vector<string> operators_map = {"left", "right", "down", "up"}; 
        vector<string> moves; // solution moves
        int n; // size of puzzle (n x n)
        int cost; // determines the order in queue
        int search; // search type: 0 = Uniform Cost, 1 = A* w/ Misplaced Tiles, 2 = A* w/ Manhattan Distance
        bool fail; //denote failure node

        Node(int size) {
            n = size;
            initial_state = vector<vector<int>>(n, vector<int>(n));
            state = initial_state;
            search = 0;
            cost = 0;
        }

        Node(vector<vector<int>> init_state) {
            n = init_state.size();
            initial_state = init_state;
            state = init_state;
            search = 0;
            cost = 0;
        }

        void readPuzzle() {
            cout << "Enter the puzzle: " << endl;
            cout << "(Number by number, separated by a space)" << endl;
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    cin >> state[i][j];
                }
            }
            cout << endl;
            initial_state = state;
        }

        void setPuzzle() {
            state = {
                {1, 2, 3},
                {4, 5, 6},
                {0, 7, 8}
            };
            initial_state = state;
        }

        void setPuzzle(vector<vector<int>> &puzzle) {
            state = puzzle;
            initial_state = state;
        }

        bool GOAL_STATE() {
            if(state[n-1][n-1] != 0) return false;

            state[n-1][n-1] = state[n-1][n-2] + 1;
            int count = 1;
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    if (state[i][j] != count++) {
                        state[n-1][n-1] = 0;
                        return false;
                    }
                }
            }
            state[n-1][n-1] = 0;
            return true;
        }

        void print() {
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    cout << state[i][j] << " ";
                }
                cout << endl;
            }
        }
};

struct comp {
    inline bool operator()(Node const &a, Node const &b) { return a.cost > b.cost; }
};

typedef priority_queue<Node, vector<Node>, comp> p_queue;
p_queue MAKE_QUEUE(Node node) {
    priority_queue<Node, vector<Node>, comp> nodes;
    nodes.push(node);
    return nodes;
}

Node MAKE_NODE(vector<vector<int>> &initial_state) {
    Node node(initial_state);
    return node;
}

Node REMOVE_FRONT(p_queue &nodes) {
    Node node = nodes.top();
    nodes.pop();
    return node;
}

bool valid_index(int r, int c, int n) {
    return (r >= 0 && c >= 0 && r < n && c < n);
}

void QUEUEING_FUNCTION(p_queue &nodes, const Node &node, const vector<pair<int, int>> &OPERATORS) {
    int rz=0, cz=0; // row and column of zero (aka the blank tile)
    for(int i=0; i<node.n; ++i) {
        for(int j=0; j<node.n; ++j) {
            if(node.state[i][j] == 0) {
                rz = i;
                cz = j;
            }
        }
    }

    for (int i=0; i < OPERATORS.size(); ++i) {
        int r = rz + OPERATORS[i].first;
        int c = cz + OPERATORS[i].second;
        if (valid_index(r, c, node.n)) {
            Node expanded = node;
            swap(expanded.state[rz][cz], expanded.state[r][c]); // equivalent to moving the blank tile to the new position
            expanded.moves.push_back(expanded.operators_map[i]);
            expanded.cost += 1;
            nodes.push(expanded);
        }
    }
}

Node general_search(Node &problem) {
    p_queue nodes = MAKE_QUEUE(MAKE_NODE(problem.initial_state));
    while(true) {
        if(nodes.empty()) {
            Node failure(1);
            failure.fail = true;
            return failure;
        }

        Node node = REMOVE_FRONT(nodes);
        if(node.GOAL_STATE()) return node;

    }

    return problem;
}


int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    Node problem(3);
    problem.print();
    problem.readPuzzle();
    problem.print();
    problem.GOAL_STATE() ? cout << "GOAL!" << endl : cout << "NOT GOAL STATE" << endl;

    vector<vector<int>> input = {
        {1, 2, 3},
        {4, 5, 6},
        {0, 7, 8}
    };

    problem.setPuzzle(input);
    problem.print();
    problem.GOAL_STATE() ? cout << "GOAL!" << endl : cout << "NOT GOAL STATE" << endl;

    Node result = general_search(problem);
    if(result.fail) {
        cout << "FAILURE" << endl;
    }
    else {
        cout << "SUCCESS" << endl;
    }

    return 0;
}