#pragma once
#include "Judge.h"
#include "Point.h"
#include <vector>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <fstream>

const int MY_ACT = 2;
const int ENEMY_ACT = 1;

const double WEIGHT = 1;

const int MAXROW = 12;
const int MAXCOLUMN = 12;

const int MUST_WIN = 1;
const int MUST_LOSE = -1;
const int MUST_TIE = 2;
const int NOMUST = 0;
const int UNCLEAR = -2;

namespace std {
	inline double UCT_func(int win_time, int test_time, int parent_test_time, double weight) {
		return (double)win_time / (double)test_time + weight * sqrt(2 * log((double)parent_test_time) / test_time);
	}

	struct MCNode {
		Point move; // The position of this move
		int who; // Who did the move
		int test_time; // How many times this board has been simulated
		int win_time; // How many times this board has won during simulation. Notice that it is a net value (win - lost)
		// double value; // Value used by UCT
		int child[MAXCOLUMN]; // Possible next step
		int top[MAXCOLUMN]; // Top array at this node
		int parent;
		int fixed_fate; // If must win/lose at this node, this value is 1/-1, else it is 0. If it has not been calculated, it is -2;

		MCNode() : move(Point(0, 0)), who(0), test_time(0), win_time(0), parent(-1), fixed_fate(UNCLEAR) {
			memset(child, 0, sizeof(child));
			memset(top, 0, sizeof(top));
		}

		int simulate(int** monte_carlo_board, int row, int column, int noX, int noY); // Simulate once at current node and return result
	};

	class MCSolver {
	public:
		MCSolver(const int M, const int N, const int nX, const int nY) : row(M), column(N), current_root(0), noX(nX), noY(nY) {
			monte_carlo_board = new int*[MAXROW];
			for (int i = 0; i < MAXROW; ++i) {
				monte_carlo_board[i] = new int[MAXCOLUMN];
				memset(monte_carlo_board[i], 0, sizeof(int) * MAXCOLUMN);
			}
			srand(time(NULL));
		}
		~MCSolver() {
			for (int i = 0; i < row; ++i) {
				delete[]monte_carlo_board[i];
			}
			delete[]monte_carlo_board;
		}
		void next_step(const int* top, int** board, const int lastX, const int lastY, int& x, int& y);
		void reset_solver(const int M, const int N, const int nX, const int nY) {
			row = M;
			column = N;
			noX = nX;
			noY = nY;
			current_root = 0;
			pool.clear();
		}

	private:
		int row;
		int column;
		int noX;
		int noY;
		int current_root;
		int** monte_carlo_board;
		int** init_board;
		vector<MCNode> pool;

		int choose_node(); // Choose node to simulate
		int node_has_child(int node); // Judge if current node is able to have child. If current node has won/lost/tie, no child possible.
		int get_best_child_at(int node); // Choose best child of some node according to some policy
		void simulate_at(int node); // Do simulate and refresh values
		int choose_step(); // Choose next step, return the selected column number
		void move_current_root_to(int lastY); // Move current root
		void expand_node_at(int node, int lastY); // Expand node for a column

		MCNode& nodeat(int node) { return pool.at(node);  }
	};
}