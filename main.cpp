#include <iostream>
#include <vector>
#include <queue>
#include <sstream>
using namespace std;

class Node {
    public:
        vector<vector<int>> initial_state;
        vector<vector<int>> state;
        vector<pair<int, int>> OPERATORS = {
             {0, -1}, {0, 1}, {1, 0}, {-1, 0} // left, right, down, up
        };
        vector<string> operators_map = {"Left", "Right", "Down", "Up"}; 
        vector<string> moves; // solution moves
        int n; // size of puzzle (n x n)
        int cost; // determines the order in queue
        int search; // search type: 0 = Uniform Cost, 1 = A* w/ Misplaced Tiles, 2 = A* w/ Manhattan Distance
        bool fail=false; //denote failure node

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
            for(int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    cout << state[i][j] << " ";
                } cout << endl;
            }
        }

        void print_prob() {
            cout << "Problem:\n";
            for(int i=0; i< n; ++i) {
                for(int j=0; j< n; ++j) {
                    cout << initial_state[i][j] << " ";
                } cout << endl;
            }
        }

        void solution() {
            cout << "\nSolution: " << endl;
            if(!moves.size()) {
                cout << "Puzzle was already solved!" << endl;
                return;
            }
            stringstream sol;
            for (int i = 0; i < (int)moves.size(); ++i) {
                sol << moves[i] << ", ";
            }
            string output = sol.str();
            cout << output.substr(0, output.size()-2) << endl; // remove last comma and space in final output
        }
};

/*This is a modified custom comparator (for priority queue specifically), original version was taken from here:
https://www.geeksforgeeks.org/stl-priority-queue-for-structure-or-class/*/
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

    for (int i=0; i < (int)OPERATORS.size(); ++i) {
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

Node general_search(Node &problem, void (*QUEUEING_FUNCTION)(p_queue&, const Node&, const vector<pair<int, int>> &)) {
    p_queue nodes = MAKE_QUEUE(MAKE_NODE(problem.initial_state));
    while(true) {
        if(nodes.empty()) {
            Node failure(1);
            failure.fail = true;
            return failure;
        }

        Node node = REMOVE_FRONT(nodes);
        if(node.GOAL_STATE()) return node;
        QUEUEING_FUNCTION(nodes, node, node.OPERATORS);
    }

    return problem; //never reached
}


int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);

    Node problem(3);
    // problem.print();
    // problem.readPuzzle();
    // problem.print();
    // problem.GOAL_STATE() ? cout << "GOAL!" << endl : cout << "NOT GOAL STATE" << endl;

    // vector<vector<int>> input = {
    //     {1, 2, 3},
    //     {4, 5, 6},
    //     {0, 7, 8}
    // };

    vector<vector<vector<int>>> puzzles = {
        {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 0}
        },
        {
            {1, 2, 3},
            {4, 5, 6},
            {0, 7, 8}
        },
        {
            {1, 2, 3},
            {5, 0, 6},
            {4, 7, 8}
        },
        {
            {1, 3, 6},
            {5, 0, 2},
            {4, 7, 8}
        },
        {
            {1, 3, 6},
            {5, 0, 7},
            {4, 8, 2}
        },
        {
            {1, 6, 7},
            {5, 0, 3},
            {4, 8, 2}
        },
        {
            {7, 1, 2},
            {4, 8, 5},
            {6, 3, 0}
        },
        {
            {0, 7, 2},
            {4, 6, 1},
            {3, 5, 8}
        }
    };

    for(int i=0; i<(int)puzzles.size(); ++i) {
        problem.setPuzzle(puzzles[i]);
        // cout << "Problem:\n";
        // problem.print();
        Node result = general_search(problem, &QUEUEING_FUNCTION);
        result.print_prob();
        if(result.fail) {
            cout << "\nFAILURE" << endl;
        }
        else {
            cout << "\nSUCCESS" << endl;
        }
        result.print();
        result.solution();
        cout << "===============================================================\n\n" << flush;
    }

    return 0;
}