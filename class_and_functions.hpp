#include <set>
#include <queue>
#include <vector>
#include <chrono>
#include <sstream>
#include <iostream>
#include <unordered_map>
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
        int n;          // size of puzzle (n x n)
        int cost=0;     // determines the order in queue, g(n) + h(n) where h(n) is 0 for Uniform Cost Search
        int current=0;  // this is g(n) in A*
        int search=3;   // search type: 1 = Uniform Cost, 2 = A* w/ Misplaced Tiles, 3 = A* w/ Manhattan Distance. Default to Manhattan.
        bool fail=0;    // denote failure node
        unsigned int expanded=0; // number of nodes expanded
        unsigned int queueSize=0; // the maximum size of the queue (nodes in frontier)

        Node(int size) {
            n = size;
            initial_state = vector<vector<int>>(n, vector<int>(n));
            state = initial_state;
        }

        Node(vector<vector<int>> init_state) {
            n = init_state.size();
            initial_state = init_state;
            state = init_state;
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

        bool GOAL_STATE() {
            //We know the blank tile has to be in the bottom right corner
            if(state[n-1][n-1] != 0) return false;

            //Check if the tiles are in sorted order [1 to n^2-1] (excluding the blank tile)
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
            if(fail) return;
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
            cout << output.substr(0, output.size()-2) << "\n\n" << flush; // remove last comma and space in final output
        }

        void walk_through() {
            cout << "\nWalk Through: " << endl;
            if(!moves.size()) {
                cout << "Puzzle was already solved!" << endl;
                return;
            }
            int rz=-1, cz=-1; // row and column of '0' (the blank tile)
            for(int i=0; i<n && rz==-1; ++i) {
                for(int j=0; j<n; ++j) {
                    if(initial_state[i][j] == 0) {
                        rz = i;
                        cz = j;
                        break;
                    }
                }
            }

            unordered_map<string, pair<int, int>> moves_map = {
                {"Left", {0, -1}}, {"Right", {0, 1}}, {"Down", {1, 0}}, {"Up", {-1, 0}}
            };
            auto walk = initial_state;
            for(int i=0; i<(int)moves.size(); ++i) {
                int r = rz + moves_map[moves[i]].first;
                int c = cz + moves_map[moves[i]].second;
                swap(walk[rz][cz], walk[r][c]);
                rz = r; cz = c;
                cout << "After move " << i+1 << ":\n";
                for(int j=0; j<n; ++j) {
                    for(int k=0; k<n; ++k) {
                        cout << walk[j][k] << " ";
                    } cout << endl;
                }
                cout << endl;
            }

        }
};

/*This is a modified custom comparator (for priority queue specifically), original version was taken from here:
https://www.geeksforgeeks.org/stl-priority-queue-for-structure-or-class/*/
struct comp {
    inline bool operator()(Node const &a, Node const &b) { return a.cost > b.cost; }
};

typedef priority_queue<Node, vector<Node>, comp> p_queue;
p_queue MAKE_QUEUE(const Node &node) {
    p_queue nodes;
    nodes.push(node);
    return nodes;
}

Node MAKE_NODE(const Node &problem) {
    return problem;
}

Node REMOVE_FRONT(p_queue &nodes) {
    Node node = nodes.top();
    nodes.pop();
    return node;
}

bool valid_index(int r, int c, int n) {
    return (r >= 0 && c >= 0 && r < n && c < n);
}

/*Observe that we can calculate the goal state for any value 'x' using:
 row = (x-1)/n and col = (x-1)%n, the -1 is because of 0 indexing. */
int calcManhattan(const vector<vector<int>> &state) {
    int n = state.size();
    int heuristic = 0; //This is h(n)
    int count = 0;
    for(int i=0; i<n; ++i) {
        for(int j=0; j<n; ++j) {
            if(state[i][j] == ++count || state[i][j] == 0) continue;
            int row = (state[i][j]-1) / n;
            int col = (state[i][j]-1) % n;
            heuristic += abs(row-i) + abs(col-j);
        }
    }
    return heuristic;
}

//count misplaced tiles
int calcMisplaced(const vector<vector<int>> &state) { 
    int n = state.size();
    int heuristic = 0; //This is h(n)
    int count = 0;
    for(int i=0; i<n; ++i) {
        for(int j=0; j<n; ++j) {
            if(state[i][j] == ++count || state[i][j] == 0) continue;
            ++heuristic;
        }
    }

    return heuristic;
}

void QUEUEING_FUNCTION(p_queue &nodes, const Node &node, const vector<pair<int, int>> &OPERATORS) {
    int rz=-1, cz=-1; // row and column of zero (aka the blank tile)
    for(int i=0; i<node.n && rz == -1; ++i) {
        for(int j=0; j<node.n; ++j) {
            if(node.state[i][j] == 0) {
                rz = i;
                cz = j;
                break;
            }
        }
    }

    for(int i=0; i < (int)OPERATORS.size(); ++i) {
        int r = rz + OPERATORS[i].first;
        int c = cz + OPERATORS[i].second;
        if (valid_index(r, c, node.n)) {
            Node expanded = node;
            swap(expanded.state[rz][cz], expanded.state[r][c]); // equivalent to moving the blank tile to the new position
            expanded.moves.push_back(expanded.operators_map[i]);
            expanded.current += 1;
            expanded.cost = expanded.current;
            if(expanded.search == 2)
                expanded.cost += calcMisplaced(expanded.state);
            else if(expanded.search == 3)
                expanded.cost += calcManhattan(expanded.state);
            nodes.push(expanded);
        }
    }
}

Node general_search(Node &problem, void (*QUEUEING_FUNCTION)(p_queue&, const Node&, const vector<pair<int, int>> &));

void Run() {
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
        },
        { // Depth 31
            {6, 4, 7},
            {8, 5, 0},
            {3, 2, 1}
        }
    };

    cout << "Would you like to input a custom puzzle? (y/n): " << flush;
    char choice;
    cin >> choice;
    if(choice == 'y' || choice == 'Y') {
        cout << "Enter search type:\n1 for Uniform Cost Search\n2 for A* with Misplaced Tiles\n3 for A* with Manhattan Distance\n" << flush;
        int search = 0;
        cin >> search; 

        cout << "\nEnter n, the side length of the puzzle. For the 8 puzzle, n = sqrt(8+1) = 3."
        << " For the 15 puzzle, n = sqrt(15+1) = 4, and so on.\nPROCEED WITH CAUTION for n >= 4.\n";
        int puzzleSize = 3;
        cin >> puzzleSize;

        Node customPuzzle(puzzleSize);
        customPuzzle.search = search;
        customPuzzle.readPuzzle();
        chrono::steady_clock::time_point begin = chrono::steady_clock::now();
        Node solution = general_search(customPuzzle, &QUEUEING_FUNCTION);
        chrono::steady_clock::time_point end = chrono::steady_clock::now();

        cout << "Time Elapsed: " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " milliseconds or about "
        << chrono::duration_cast<chrono::seconds> (end - begin).count() << " seconds\n";

        if(solution.fail) 
            cout << "\nFailure\n";
        else 
            cout << "\nSUCCESS!\n";
        solution.solution();
        if(!solution.fail)
            cout << "Solution Depth: " << solution.moves.size() << '\n';
        cout << "Nodes Expanded: " << solution.expanded << "\nMax Queue Size: " << solution.queueSize << '\n';
        // solution.walk_through();
        cout << "=======================================================================\n\n" << flush;
    }

    cout << "Would you like to run all 9 sample puzzles? (y/n): " << flush;
    cin >> choice;
    if(choice == 'y' || choice == 'Y') {
        cout << "Enter search type:\n1 for Uniform Cost Search\n2 for A* with Misplaced Tiles\n3 for A* with Manhattan Distance\n" << flush;
        int search_type = 0;
        cin >> search_type; 

        for(int i=0; i<(int)puzzles.size(); ++i) {
            Node problem(puzzles[i]);
            problem.search = search_type;
            chrono::steady_clock::time_point begin = chrono::steady_clock::now();
            Node result = general_search(problem, &QUEUEING_FUNCTION);
            chrono::steady_clock::time_point end = chrono::steady_clock::now();

            result.print_prob();
            cout << "\nTime Elapsed: " << chrono::duration_cast<chrono::milliseconds> (end - begin).count() << " milliseconds\n";
            if(result.fail)
                cout << "\nFAILURE\n";
            else
                cout << "\nSUCCESS\n";
            result.solution();
            cout << "Solution Depth: " << result.moves.size() << "\nNodes Expanded: " << result.expanded << "\nMax Queue Size: " << result.queueSize << '\n';
            // result.walk_through();
            cout << "=======================================================================\n\n" << flush;
        }
    }
    else {
        cout << "Enter the sample puzzle you would like to run (1-9): " << flush;
        cin >> choice;
        cout << "Enter search type:\n1 for Uniform Cost Search\n2 for A* with Misplaced Tiles\n3 for A* with Manhattan Distance\n" << flush;
        int search_type = 0;
        cin >> search_type;
        Node problem(puzzles[choice - '1']);
        problem.search = search_type;
        problem.print_prob();
        chrono::steady_clock::time_point begin = chrono::steady_clock::now();
        Node result = general_search(problem, &QUEUEING_FUNCTION);
        chrono::steady_clock::time_point end = chrono::steady_clock::now();

        cout << "\nTime Elapsed: " << chrono::duration_cast<chrono::milliseconds> (end - begin).count() << " milliseconds\n";
        if(result.fail)
            cout << "\nFAILURE\n";
        else
            cout << "\nSUCCESS\n";
        result.solution();
        cout << "Solution Depth: " << result.moves.size() << "\nNodes Expanded: " << result.expanded << "\nMax Queue Size: " << result.queueSize << '\n';
        // result.walk_through();
        cout << "=======================================================================\n\n" << flush;
    }
}