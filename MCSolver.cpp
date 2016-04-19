#include "MCSolver.h"

using namespace std;

int MCNode::simulate(int** monte_carlo_board) {

}

void MCSolver::next_step(const int* top, const int** board, const int lastX, const int lastY, int& x, int& y) {
	// Move current_node...
	if (current_root == 0) {
		MCNode root;
		root.move = Point(lastX, lastY);
		root.who = ENEMY_ACT;
		pool.push_back(root);
	}
	else {
		move_current_root_to(lastY);
	}

	// while (time enough) {
	simulate_at(choose_node());
	// }

	y = choose_step();
	x = top[y] - 1;
	move_current_root_to(y);
}

int MCSolver::choose_node() {

}

int MCSolver::get_best_child_at(int node) {

}

void MCSolver::simulate_at(int node) {

}

int MCSolver::choose_step() {

}

void MCSolver::move_current_root_to(int lastY) {
	if (nodeat(current_root).child[lastY] == 0) {
		expand_node_at(current_root, Point(nodeat(current_root).top[lastY], lastY));
	}
	current_root = nodeat(current_root).child[lastY];
}

void MCSolver::expand_node_at(int node, Point& step) {
	MCNode tempnode;
	tempnode.move = step;
	if (nodeat(node).who == ENEMY_ACT)
		tempnode.who = MY_ACT;
	else if (nodeat(node).who == MY_ACT)
		tempnode.who = ENEMY_ACT;
	tempnode.parent = node;
	for (int i = 0; i < column; ++i) {
		tempnode.top[i] = nodeat(node).top[i];
	}
	tempnode.top[step.y] = step.x - 1;
	if (step.y == noY && tempnode.top[step.y] == noX) {
		--tempnode[step.y];
	}
	pool.push_back(tempnode);
	nodeat(node).child[step.y] = pool.size() - 1;
}