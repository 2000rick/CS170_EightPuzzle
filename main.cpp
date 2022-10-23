#include "class_and_functions.hpp"

Node general_search(Node &problem, void (*QUEUEING_FUNCTION)(p_queue&, const Node&, const vector<pair<int, int>> &)) {
    unsigned int nodesExpanded = 0;
    set<vector<vector<int>>> visited;
    p_queue nodes = MAKE_QUEUE(MAKE_NODE(problem));
    long long threshold = 100;
    while(true) {
        if(nodes.empty()) {
            Node failure(1);
            failure.expanded = nodesExpanded;
            failure.fail = true;
            return failure;
        }

        if(nodes.size() >= threshold) {
            threshold *= 10;
            cout << "Nodes in frontier: " << nodes.size() << endl;
        }
        
        Node node = REMOVE_FRONT(nodes);
        if(visited.find(node.state) != visited.end()) continue;
        node.expanded = nodesExpanded;
        if(node.GOAL_STATE()) return node;
        ++nodesExpanded;
        QUEUEING_FUNCTION(nodes, node, node.OPERATORS);
        visited.insert(node.state);
    }

    return problem; //never reached
}

int main() {
    Run();
    
    return 0;
}
