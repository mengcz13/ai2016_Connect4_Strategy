/********************************************************
*	Strategy.h : 策略接口文件                           *
*	张永锋                                              *
*	zhangyf07@gmail.com                                 *
*	2010.8                                              *
*********************************************************/

#ifndef STRATEGY_H_
#define	STRATEGY_H_

#include "Point.h"

extern "C" __declspec(dllexport) Point* getPoint(const int M, const int N, const int* top, const int* _board, 
	const int lastX, const int lastY, const int noX, const int noY);

extern "C" __declspec(dllexport) void clearPoint(Point* p);

void clearArray(int M, int N, int** board);

/*
	添加你自己的辅助函数
*/

#include "MCSolver.h"

// In case that the strategy is used to fight against itself!
// When we use the strategy to fight against a different one, only getPoint_first or getPoint_second will be called.
int point_num(const int M, const int N, int** board, int& enemynum, int& mynum); // Get how many points are on the board. If the number is even, then the first is to go, else the second is to go.

void getPoint_first(const int M, const int N, const int* top, int** board,
	const int lastX, const int lastY, const int noX, const int noY, int enemynum, int mynum, int& x, int& y); // Used for current player who is the first to play

void getPoint_second(const int M, const int N, const int* top, int** board,
	const int lastX, const int lastY, const int noX, const int noY, int enemynum, int mynum, int& x, int& y); // Used for current player who is the second to play

#endif