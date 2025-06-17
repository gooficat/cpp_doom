#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "types.hpp"
#include <cmath>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

class player_t {
public:
	vec2_t position;
	double z;
	double angle;
	
	const double speed = 150.0;
	const double elevation_speed = 50000;
	const double rotation_speed = 4;
	player_t(double x, double y, double z, double angle);
	void update(bool* keys, double delta_time);
};

#endif