#ifndef UTILS_HPP
#define UTILS_HPP
#include <ctime>
#include <cstdlib>
#include <cmath>


enum game_state_t {
	PLAYING,
	PAUSED,
	MAP,
	MENU,
	QUIT
};

int randRange(int min, int max);

double SafeDivide(double numerator, double denominator, double fallback = 1.0);
#endif