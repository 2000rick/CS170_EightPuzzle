#include "class_and_functions.hpp"

Node general_search(Node &problem, void (*QUEUEING_FUNCTION)(p_queue&, const Node&, const vector<pair<int, int>> &)) {
    unsigned int nodesExpanded = 0;
    set<vector<vector<int>>> visited;
    priority_queue nodes = MAKE_QUEUE(MAKE_NODE(problem));
    while(true) {
        if(nodes.empty()) {
            Node failure(1);
            failure.expanded = nodesExpanded;
            failure.fail = true;
            return failure;
        }
        
        Node node = REMOVE_FRONT(nodes);
        if(visited.find(node.state) != visited.end()) continue;
        node.expanded = nodesExpanded;
        if(node.GOAL_STATE()) return node;
        ++nodesExpanded;
        visited.insert(node.state);
        QUEUEING_FUNCTION(nodes, node, node.OPERATORS);
    }

    return problem; //never reached
}

int main() {
    Run();
    
    return 0;
}
