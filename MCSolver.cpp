#include "MCSolver.h"

using namespace std;

const int SIMTIME = 100; // Fixed simulation time

int MCNode::simulate(int** monte_carlo_board, int row, int column, int noX, int noY) {
	int temptop[MAXCOLUMN];
	for (int i = 0; i < column; ++i)
		temptop[i] = this->top[i];
	Point lastmove = this->move;
	int lastwho = this->who;
	while (1) {
		if (machineWin(lastmove.x, lastmove.y, row, column, monte_carlo_board)) {
			if (this->who == MY_ACT)
				return 1;
			else if (this->who == ENEMY_ACT)
				return -1;
		}
		else if (userWin(lastmove.x, lastmove.y, row, column, monte_carlo_board)) {
			if (this->who == ENEMY_ACT)
				return 1;
			else if (this->who == MY_ACT)
				return -1;
		}
		else if (isTie(column, temptop)) {
			return 0;
		}

		int choose_col = rand() % column;
		while (temptop[choose_col] <= 0)
			choose_col = rand() % column;
		lastmove.x = temptop[choose_col] - 1;
		lastmove.y = choose_col;
		if (lastwho == MY_ACT)
			lastwho = ENEMY_ACT;
		else if (lastwho == ENEMY_ACT)
			lastwho = MY_ACT;

		--temptop[choose_col];
		if (choose_col == noY && temptop[choose_col] == noX)
			--temptop[choose_col];
		monte_carlo_board[lastmove.x][lastmove.y] = lastwho;
	}
}

void MCSolver::next_step(const int* top, int** board, const int lastX, const int lastY, int& x, int& y) {
	// Move current_node...
	if (current_root == 0) {
		MCNode root;
		root.move = Point(lastX, lastY);
		root.who = ENEMY_ACT;
		for (int i = 0; i < column; ++i)
			root.top[i] = top[i];
		pool.push_back(root);
	}
	else {
		move_current_root_to(lastY);
	}
	init_board = board;
	// while (time enough) {
	for (int i = 0; i < 10000; ++i)
		simulate_at(choose_node());
	// }

	y = choose_step();
	x = top[y] - 1;
	move_current_root_to(y);
}

int MCSolver::choose_node() {
	// Restore current board, modify it in the process of choosing
	for (int i = 0; i < row; ++i) {
		for (int j = 0; j < column; ++j) {
			monte_carlo_board[i][j] = init_board[i][j];
		}
	}

	int choose = current_root;
	while (node_has_child(choose)) {
		for (int i = 0; i < column; ++i) {
			if (nodeat(choose).child[i] == 0 && nodeat(choose).top[i] > 0) {
				expand_node_at(choose, i);
				MCNode& newnode = nodeat(nodeat(choose).child[i]);
				monte_carlo_board[newnode.move.x][newnode.move.y] = newnode.who;
				return nodeat(choose).child[i];
			}
		}
		choose = get_best_child_at(choose);
		monte_carlo_board[nodeat(choose).move.x][nodeat(choose).move.y] = nodeat(choose).who;
	}
	return choose;
}

bool MCSolver::node_has_child(int node) {
	if (node == 0)
		return true; // Case for first step, it must have children.
	MCNode& laststep = nodeat(node);
	if (laststep.who == MY_ACT) {
		if (machineWin(laststep.move.x, laststep.move.y, row, column, monte_carlo_board) || isTie(column, laststep.top))
			return false;
	}
	else if (laststep.who == ENEMY_ACT) {
		if (userWin(laststep.move.x, laststep.move.y, row, column, monte_carlo_board) || isTie(column, laststep.top))
			return false;
	}
	return true;
}

int MCSolver::get_best_child_at(int node) {
	double max_value = -1e8;
	int max_child = 0;
	// Choose max_value child
	for (int i = 0; i < column; ++i) {
		if (nodeat(node).child[i] > 0) {
			MCNode& child = nodeat(nodeat(node).child[i]);
			double node_value = UCT_func(child.win_time, child.test_time, nodeat(child.parent).test_time, WEIGHT);
			if (node_value > max_value) {
				max_value = node_value;
				max_child = i;
			}
		}
	}
	return nodeat(node).child[max_child];
}

void MCSolver::simulate_at(int node) {
	int backup[MAXROW][MAXCOLUMN];
	for (int i = 0; i < row; ++i)
		for (int j = 0; j < column; ++j)
			backup[i][j] = monte_carlo_board[i][j];

	int simtime = 0, total_res = 0;
	for (int t = 0; t < SIMTIME; ++t) {
		total_res += nodeat(node).simulate(monte_carlo_board, row, column, noX, noY);
		++simtime;

		for (int i = 0; i < row; ++i)
			for (int j = 0; j < column; ++j)
				monte_carlo_board[i][j] = backup[i][j];
	}
	int p = node;
	while (p != current_root) {
		nodeat(p).test_time += simtime;
		nodeat(p).win_time += total_res;
		total_res = -total_res;
		p = nodeat(p).parent;
	}
}

int MCSolver::choose_step() {
	MCNode& croot = nodeat(current_root);
	double max_value = -1e8;
	int max_child = 0;
	for (int i = 0; i < column; ++i) {
		if (nodeat(croot.child[i]).test_time == 0 && croot.top[i] > 0)
			return i;
		double nodevalue = UCT_func(nodeat(croot.child[i]).win_time, nodeat(croot.child[i]).test_time, croot.test_time, 0);
		if (nodevalue > max_value && croot.top[i] > 0) {
			max_value = nodevalue;
			max_child = i;
		}
	}
	return max_child;
}

void MCSolver::move_current_root_to(int lastY) {
	if (nodeat(current_root).child[lastY] == 0) {
		expand_node_at(current_root, lastY);
	}
	current_root = nodeat(current_root).child[lastY];
}

void MCSolver::expand_node_at(int node, int lastY) {
	MCNode tempnode;
	Point step(nodeat(node).top[lastY] - 1, lastY);
	if (step.x == noX && step.y == noY) // In case step is at the point not allowed
		--step.x;
	tempnode.move = step;
	if (nodeat(node).who == ENEMY_ACT)
		tempnode.who = MY_ACT;
	else if (nodeat(node).who == MY_ACT)
		tempnode.who = ENEMY_ACT;
	tempnode.parent = node;
	for (int i = 0; i < column; ++i) {
		tempnode.top[i] = nodeat(node).top[i];
	}
	// tempnode.top[step.y] = step.x - 1;
	tempnode.top[step.y] = step.x;
	if (step.y == noY && tempnode.top[step.y] - 1 == noX) {
		--tempnode.top[step.y];
	}
	pool.push_back(tempnode);
	nodeat(node).child[step.y] = pool.size() - 1;
}