#pragma once
#include "Judge.h"
#include "Point.h"
#include <vector>
#include <cstring>
#include <cstdlib>
#include <ctime>

const int MY_ACT = 2;
const int ENEMY_ACT = 1;

const double WEIGHT = 1;

const int MAXROW = 12;
const int MAXCOLUMN = 12;

namespace std {
	struct MCNode {
		Point move; // The position of this move
		int who; // Who did the move
		int test_time; // How many times this board has been simulated
		int win_time; // How many times this board has won during simulation
		double value; // Value used by UCT
		int child[MAXCOLUMN]; // Possible next step
		int top[MAXCOLUMN]; // Top array at this node
		int parent;

		MCNode() : move(Point(0, 0)), who(0), test_time(0), win_time(0), value(-1), parent(-1) {
			memset(child, 0, sizeof(child));
			memset(top, 0, sizeof(top));
		}

		int simulate(int** monte_carlo_board); // Simulate once at current node and return result
	};

	class MCSolver {
	public:
		MCSolver(const int M, const int N, const int nX, const int nY) : row(M), column(N), current_root(0), noX(nX), noY(nY) {
			monte_carlo_board = new int*[M];
			for (int i = 0; i < M; ++i) {
				monte_carlo_board[i] = new int[N];
				memset(monte_carlo_board[i], 0, sizeof(int) * N);
			}
			srand(time(NULL));
		}
		~MCSolver() {
			for (int i = 0; i < row; ++i) {
				delete[]monte_carlo_board[i];
			}
			delete[]monte_carlo_board;
		}
		void next_step(const int* top, const int** board, const int lastX, const int lastY, int& x, int& y);

	private:
		const int row;
		const int column;
		const int noX;
		const int noY;
		int current_root;
		int** monte_carlo_board;
		vector<MCNode> pool;

		int choose_node(); // Choose node to simulate
		int get_best_child_at(int node); // Choose best child of some node according to some policy
		void simulate_at(int node); // Do simulate and refresh values
		int choose_step(); // Choose next step
		void move_current_root_to(int lastY); // Move current root
		void expand_node_at(int node, Point& step); // Expand node for a column

		MCNode& nodeat(int node) { return pool.at(node);  }
	};
}