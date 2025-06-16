#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <cmath>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "types.hpp"

#include "player.hpp"
#include "utils.hpp"


struct plane_t {
	std::vector<int> t, b;
};

struct wall_t {
	vec2_t a, b;
	double portal_top_height;
	double portal_bot_height;
	bool is_portal;
	wall_t(int ax, int ay, int bx, int by);
};

struct sector_t {
	int id;
	std::vector<wall_t> walls;
	int height, elevation;
	double dist;
	unsigned color, floor_color, ceil_color;
	plane_t portals_floor, portals_ceil, floor, ceil;
	sector_t(int height, int elevation, unsigned color, unsigned ceil_color, unsigned floor_color);
	void AddWall(wall_t vertices);
};

class Renderer {
public:
	SDL_Window* window;
	SDL_Texture* texture;
	SDL_Renderer* renderer;
	unsigned* screen_buffer;
	int buffer_size;
	int width, height;
	std::vector<sector_t> sector_queue;
	
	Renderer(player_t& player, game_state_t& game_state, SDL_Window* main_win);
	~Renderer();
	void Render(player_t& player, double delta_time);
	void DrawWalls(player_t& player);
	void QueueSector(sector_t& sector);
	void DrawPoint(int x, int y, unsigned color);
	void DrawLine(int x0, int y0, int x1, int y1, unsigned color);
	void clear();
	void DrawSectors(player_t& player);
};

#endif