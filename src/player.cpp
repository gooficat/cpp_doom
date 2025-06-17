#include "player.hpp"



player_t::player_t(double x, double y, double z_, double a) {
		position.x = x;
		position.y = y;
		z = z_;
		angle = a;
}

void player_t::update(bool* keys, double delta_time) {
	if(keys[SDL_SCANCODE_W] == true) {
		position.x += speed * cos(angle) * delta_time;
		position.y += speed * sin(angle) * delta_time;
	}
	if(keys[SDL_SCANCODE_S] == true) {
		position.x -= speed * cos(angle) * delta_time;
		position.y -= speed * sin(angle) * delta_time;
	}	
	if(keys[SDL_SCANCODE_A] == true) {
		position.x += speed * cos(angle+M_PI/2) * delta_time;
		position.y += speed * sin(angle+M_PI/2) * delta_time;
	}
	if(keys[SDL_SCANCODE_D] == true) {
		position.x -= speed * cos(angle+M_PI/2) * delta_time;
		position.y -= speed * sin(angle+M_PI/2) * delta_time;
	}	
	if(keys[SDL_SCANCODE_SPACE] == true) {
		z += elevation_speed * delta_time;
	}
	if(keys[SDL_SCANCODE_LSHIFT] == true) {
		z -= elevation_speed * delta_time;
	}
	if(keys[SDL_SCANCODE_Q] == true) {
		angle += rotation_speed * delta_time;
	}
	if(keys[SDL_SCANCODE_E] == true) {
		angle -= rotation_speed * delta_time;
	}
}