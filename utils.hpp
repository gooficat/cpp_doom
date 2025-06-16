#ifndef UTILS_HPP
#define UTILS_HPP
#include <ctime>
#include <cstdlib>


enum game_state_t {
	PLAYING,
	PAUSED,
	MAP,
	MENU,
	QUIT
};

int randRange(int min, int max);
#endif