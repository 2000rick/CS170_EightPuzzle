#include "class_and_functions.hpp"

Node general_search(Node &problem, void (*QUEUEING_FUNCTION)(p_queue&, const Node&, const vector<pair<int, int>> &)) {
    unsigned int nodesExpanded = 0; // number of nodes expanded
    unsigned int queueMaxSize = 0; // the maximum size of the queue
    set<vector<vector<int>>> visited;
    priority_queue nodes = MAKE_QUEUE(MAKE_NODE(problem));
    while(true) {
        queueMaxSize = max(queueMaxSize, (unsigned int)nodes.size());
        if(nodes.empty()) {
            Node failure(1);
            failure.expanded = nodesExpanded;
            failure.queueSize = queueMaxSize;
            failure.fail = true;
            return failure;
        }
        
        Node node = REMOVE_FRONT(nodes);
        if(visited.find(node.state) != visited.end()) continue;
        ++nodesExpanded;
        node.expanded = nodesExpanded;
        node.queueSize = queueMaxSize;
        if(node.GOAL_STATE()) return node;
        visited.insert(node.state);
        QUEUEING_FUNCTION(nodes, node, node.OPERATORS);
    }

    return problem; //never reached
}

int main() {
    Run();
    
    return 0;
}
