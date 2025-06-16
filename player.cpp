#include "player.hpp"



player_t::player_t(double x, double y, double z_, double a) {
		position.x = x;
		position.y = y;
		z = z_;
		angle = a;
}

void player_t::update(bool* keys, double delta_time) {
	if(keys[SDLK_w]) {
		position.x += speed * cos(angle) * delta_time;
		position.y += speed * sin(angle) * delta_time;
	}
	if(keys[SDLK_s]) {
		position.x -= speed * cos(angle) * delta_time;
		position.y -= speed * sin(angle) * delta_time;
	}	
	if(keys[SDLK_a]) {
		position.x -= speed * sin(angle) * delta_time;
		position.y -= speed * cos(angle) * delta_time;
	}
	if(keys[SDLK_d]) {
		position.x += speed * sin(angle) * delta_time;
		position.y += speed * cos(angle) * delta_time;
	}	
	if(keys[SDLK_SPACE]) {
		//z += elevation_speed * delta_time;
	}
	if(keys[SDLK_TAB]) {
		//z -= elevation_speed * delta_time;
	}
}