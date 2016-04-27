#pragma once
#include "Judge.h"
#include "Point.h"
#include <vector>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <cmath>

const int MY_ACT = 2;
const int ENEMY_ACT = 1;

const double WEIGHT = 0.85;

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
		size_t child[MAXCOLUMN]; // Possible next step
		int top[MAXCOLUMN]; // Top array at this node
		size_t parent;
		int fixed_fate; // If must win/lose at this node, this value is 1/-1, else it is 0. If it has not been calculated, it is -2;

		MCNode() : move(Point(0, 0)), who(0), test_time(0), win_time(0), parent(0), fixed_fate(UNCLEAR) {
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
		size_t current_root;
		int** monte_carlo_board;
		int** init_board;
		vector<MCNode> pool;

		size_t choose_node(); // Choose node to simulate
		int node_has_child(size_t node); // Judge if current node is able to have child. If current node has won/lost/tie, no child possible.
		size_t get_best_child_at(size_t node); // Choose best child of some node according to some policy
		void simulate_at(size_t node); // Do simulate and refresh values
		int choose_step(); // Choose next step, return the selected column number
		void move_current_root_to(int lastY); // Move current root
		void expand_node_at(size_t node, int lastY); // Expand node for a column

		MCNode& nodeat(size_t node) { return pool.at(node);  }
	};

	// A pre-trained neural network
	class ANN {
	public:
		ANN(int n_in = MAXROW * MAXCOLUMN, int n_hidden = 200, int n_out = MAXCOLUMN) : n_in(n_in), n_hidden(n_hidden), n_out(n_out) {
			W1 = new double[n_in * n_hidden];
			b1 = new double[n_hidden];
			W2 = new double[n_hidden * n_out];
			b2 = new double[n_out];
			transboard = new double[n_in];
			hidden_vec = new double[n_hidden];
			output_vec = new double[n_out];

			FILE* fp = fopen("w1.txt", "r");
			for (int i = 0; i < n_in * n_hidden; ++i)
				fscanf_s(fp, "%lf", &W1[i]);
			fclose(fp);

			fp = fopen("b1.txt", "r");
			for (int i = 0; i < n_hidden; ++i)
				fscanf_s(fp, "%lf", &b1[i]);
			fclose(fp);

			fp = fopen("w2.txt", "r");
			for (int i = 0; i < n_hidden * n_out; ++i)
				fscanf_s(fp, "%lf", &W2[i]);
			fclose(fp);

			fp = fopen("b2.txt", "r");
			for (int i = 0; i < n_out; ++i)
				fscanf_s(fp, "%lf", &b2[i]);
			fclose(fp);
		}
		~ANN() {
			delete[]W1;
			delete[]b1;
			delete[]W2;
			delete[]b2;
			delete[]transboard;
			delete[]hidden_vec;
			delete[]output_vec;
		}
		int get_output_column_for_me(int** board, const int M, const int N, const int noX, const int noY, const int* top);
		double score_for_out(int i) { return output_vec[i];  }
	private:
		int n_in;
		int n_hidden;
		int n_out;
		double* W1;
		double* b1;
		double* W2;
		double* b2;
		double* transboard;
		double* hidden_vec;
		double* output_vec;
	};
}