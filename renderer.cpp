#include "renderer.hpp"

Renderer::Renderer(player_t& player, game_state_t& game_state, SDL_Window* main_win) {
	SDL_GetWindowSize(main_win, &width, &height);
	window = main_win;
	renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
	
	buffer_size = sizeof(unsigned) * width * height;
	screen_buffer = (unsigned*)malloc(buffer_size);
	if(!screen_buffer)
		game_state = QUIT;
	
	memset(screen_buffer, 0, buffer_size);
	
	texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING,
		width, height
	);
	
	if (!texture)
		game_state = QUIT;
	
	SDL_RenderSetLogicalSize(renderer, width, height);
}

void Renderer::Render(player_t& player, double delta_time) {
	DrawSectors(player);
	
	SDL_UpdateTexture(texture, NULL, screen_buffer, width * sizeof(unsigned));
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);	
}
Renderer::~Renderer() {
	SDL_DestroyTexture(texture);
	free(screen_buffer);
	SDL_DestroyRenderer(renderer);
}

wall_t::wall_t(int ax, int ay, int bx, int by) {
	a.x = ax;
	a.y = ay;
	b.x = bx;
	b.y = by;
	is_portal = false;
}

sector_t::sector_t(int height, int elevation, unsigned color, unsigned ceil_color, unsigned floor_color) :
	height(height), elevation(elevation), color(color), floor_color(floor_color), ceil_color(ceil_color) {
	static int sector_id = 0;
	id = ++ sector_id;
}

void sector_t::AddWall(wall_t vertices) {
	walls.push_back(vertices);
}

void Renderer::DrawPoint(int x, int y, unsigned color) {
	bool out_of_bounds = (x < 0 || x > width || y < 0 || y >= height);
	bool out_of_memory = (width * y + x) >= (width * height);
	
	if (out_of_bounds || out_of_memory)
		return;
	
	screen_buffer[width * y + x] = color;
}

void Renderer::DrawLine(int x0, int y0, int x1, int y1, unsigned color) {
	int dx;
	if (x1 > x0)
		dx = x1 - x0;
	else
		dx = x0 - x1;
	int dy;
	if (y1 > y0)
		dy = y1 - y0;
	else
		dy = y0 - y1;
	
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;
	int err = (dx > dy ? dx : -dy) / 2, e2;
	
	for (;;) {
		DrawPoint(x0, y0, color);
		if (x0 == x1 && y0 == y1)
			break;
		e2 = err;
		
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		}
		
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		}
	}
}

void Renderer::clear() {
	memset(screen_buffer, 0, sizeof(uint32_t) * width * height);
}

void Renderer::DrawSectors(player_t& player) {
	double half_w = width/2;
	double half_h = height/2;
	double fov = 300;
	unsigned wall_color = 0xFFFF0FFF;
	clear();
	
	for (int i = 0; i < sector_queue.size(); i++) {
		sector_t *s = &sector_queue[i];
		int sector_h = s->height;
		int sector_e = s->elevation;
		int sector_clr = s->color;
		
		for (int j = 0; j < s->walls.size(); j++) {
			wall_t *w = &s->walls[j];
			
			double dx1 = w->a.x - player.position.x;
			double dy1 = w->a.y - player.position.y;
			double dx2 = w->b.x - player.position.x;
			double dy2 = w->b.y - player.position.y;
			
			double SN = sin(player.angle);
			double CS = cos(player.angle);
			double wx1 = dx1 * SN - dy1 * CS;
			double wz1 = dx1 * CS + dy1 * SN;
			double wx2 = dx2 * SN - dy2 * CS;
			double wz2 = dx2 * CS + dy2 * SN;
			
			double wh1 = (sector_h / wz1) * fov;
			double wh2 = (sector_h / wz2) * fov;
			
			double sx1 = (wx1 / wz1) * fov;
			double sy1 = ((height + player.z) / wz1);
			double sx2 = (wx2 / wz2) * fov;
			double sy2 = ((height + player.z) / wz2);
			
			double s_level1 = (sector_e / wz1) * fov;
			double s_level2 = (sector_e / wz2) * fov;
			sy1 -= s_level1;
			sy2 -= s_level2;
			
			double pbh1 = 0;
			double pbh2 = 0;
			double pth1 = 0;
			double pth2 = 0;
			if(w->is_portal) {
				pth1 = (w->portal_top_height / wz1) * fov;
				pth2 = (w->portal_top_height / wz2) * fov;
				pbh1 = (w->portal_bot_height / wz1) * fov;
				pbh2 = (w->portal_bot_height / wz2) * fov;
			}
			
			sx1 += half_w;
			sy1 += half_h;
			
			sx2 += half_w;
			sy2 += half_h;
			
			DrawLine(sx1, sy1 - wh1, sx2, sy2 - wh2, wall_color);
			DrawLine(sx1, sy1, sx2, sy2, wall_color);
			
			DrawLine(sx1, sy1 - wh1, sx1, sy1, wall_color);
			DrawLine(sx2, sy2 - wh2, sx2, sy2, wall_color);
		}
	}
}