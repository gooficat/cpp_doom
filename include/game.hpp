#ifndef GAME_HPP
#define GAME_HPP

#define SDL_MAIN_HANDLED
#include <cmath>
#include <SDL2/SDL.h>
#include "renderer.hpp"
#include "player.hpp"
#include "utils.hpp"


class Game {
public:
	Game(unsigned width, unsigned height, unsigned target_fps);
	~Game();
	int run();
	SDL_Window* window;
	Renderer* renderer;
	player_t* player;
	double delta_time;
	game_state_t game_state;
	
	bool debug;
	bool keys[512];
	unsigned width, height;
	double target_fps, target_frametime;
	int frame_start;
	void pollInput();
	void update();
	void draw();
	void startFrame();
	void endFrame();
};

#endif