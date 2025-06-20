#include "game.hpp"




Game::Game(unsigned width, unsigned height, unsigned target_fps) : width(width), height(height), target_fps(target_fps) {
	game_state = PLAYING;
	target_frametime = 1.0 / (double) target_fps;
	delta_time = target_frametime;
	debug = false;
	frame_start = 0;
	
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	window = SDL_CreateWindow("doom clone", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	
	player = new player_t(40.0, 40.0, height*10, M_PI/2);
	renderer = new Renderer(*player, game_state, window);
	sector_t s1(10, 1, 0xd6382d, 0xf54236, 0x9c2921);
	sector_t s2(80, 0, 0x29ba48, 0x43f068, 0x209138);
	
	int s1v[4*4] = {
		0, 220, 100, 220,
		100, 220, 100, 240,
		100, 240, 0, 240,
		0, 240, 0, 220
	};
	int s2v[4*4] = {
		-30, 120, 40, 120,
		40, 120, 40, 190,
		40, 190, -30, 190,
		-30, 190, -30, 120
	};
	
	for(int i = 0; i < 16; i +=4 ) {
		wall_t w(s1v[i], s1v[i+1], s1v[i+2], s1v[i+3]);
		s1.AddWall(w);
		wall_t p(s2v[i], s2v[i+1], s2v[i+2], s2v[i+3], 20.0, 10.0);
		s2.AddWall(p);
	}
	
	renderer->sector_queue.push_back(s1);
	renderer->sector_queue.push_back(s2);

}


void Game::update() {
	pollInput();
	player->update(keys, delta_time);
}

void Game::draw() {
	renderer->Render(*player, delta_time);
}

int Game::run() {
	while(game_state != QUIT) {
		
		startFrame();
		update();
		draw();
		endFrame();
	}
	return 0;
}

Game::~Game() {
}
void Game::startFrame() {
	frame_start	= SDL_GetTicks();
}

void Game::endFrame() {
	delta_time = (SDL_GetTicks() - frame_start) / 1000.0;
	
	if (delta_time < target_frametime)
		SDL_Delay((target_frametime - delta_time) * 1000.0);
	delta_time = target_frametime;
}

void Game::pollInput() {
	SDL_Event event;
	SDL_PollEvent(&event);
	
	switch (event.type)
	{
		case SDL_KEYDOWN:
			keys[event.key.keysym.scancode] = true;
			break;
		case SDL_KEYUP:
			keys[event.key.keysym.scancode] = false;
			break;
		case SDL_QUIT:
			game_state = QUIT;
			break;
		default:
			break;
	}
}