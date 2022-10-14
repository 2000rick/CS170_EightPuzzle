#include <bits/stdc++.h>
using namespace std;

class Node {
    public:
        Node(int size) {
            n = size;
            state = vector<vector<int>>(n, vector<int>(n));
        }

        Node(const Node &node) {
            n = node.n;
            state = node.state;
        }

        void readPuzzle() {
            cout << "Enter the puzzle: " << endl;
            cout << "(Number by number, separated by a space)" << endl;
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    cin >> state[i][j];
                }
            }
        }

        void setPuzzle() {
            state = {
                {1, 2, 3},
                {4, 5, 6},
                {0, 7, 8}
            };
        }

        void setPuzzle(vector<vector<int>> &puzzle) {
            state = puzzle;
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


    private:
        vector<vector<int>> state;
        int n; // size of puzzle (n x n)
};



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

    return 0;
}