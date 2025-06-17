#include "renderer.hpp"

Renderer::Renderer(player_t& player, game_state_t& game_state, SDL_Window* main_win) {
	SDL_GetWindowSize(main_win, &width, &height);
	window = main_win;
	renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_SOFTWARE);
	
	
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

wall_t::wall_t(int ax, int ay, int bx, int by, double pth, double pbh) {
	a.x = ax;
	a.y = ay;
	b.x = bx;
	b.y = by;
	portal_top_height = pth;
	portal_bot_height = pbh;
	is_portal = true;
}

sector_t::sector_t(int height, int elevation, unsigned color, unsigned ceil_color, unsigned floor_color) :
	height(height), elevation(elevation), color(color), floor_color(floor_color), ceil_color(ceil_color) {
	static int sector_id = 0;
	id = ++ sector_id;
}

void sector_t::AddWall(wall_t vertices) {
	walls.push_back(vertices);
}

void Renderer::rasterize(quad_t& q) {
	if (q.type == WALL and q.ax > q.bx) {
		return;
	}
	bool is_back_wall = false;
	
	if (q.type != WALL and q.ax > q.bx) {
		is_back_wall = true;
		q.swapPoints();
	}
	
	double delta_height, delta_elevation;
	
	q.calcInterp(delta_height, delta_elevation);
	if (delta_height == -1 and delta_elevation == -1)
		return;
	
	for (int x = q.ax, i = 1; x < q.bx; x++, i++) {
		if (x < 0 or x > width-1) continue;
		
		double dh = delta_height * i;
		double dy_player_elev = delta_elevation * 1;
		
		int y1 = q.at - (dh / 2) + dy_player_elev;
		int y2 = q.ab + (dh / 2) + dy_player_elev;
		if (y1 > height) y1 = height;
		if (y2 > height) y2 = height;
		if (y1 < 0) y1 = 0;
		if (y2 < 0) y2 = 0;
		
		if (q.type == CEILING) {
			if(!is_back_wall)
				q.plane->t[x] = y1;
			else
				q.plane->b[x] = y1;
		}
		else if (q.type == FLOOR) {
			if(!is_back_wall)
				q.plane->t[x] = y2;
			else
				q.plane->b[x] = y2;
		}
		else {
			DrawLine(x, y1, x, y2, q.color);
		}
	}
	
}

void quad_t::calcInterp(double& delta_height, double& delta_elevation) {
	int width = abs(ax - bx);
	if (!width) {
		delta_height = -1;
		delta_elevation = -1;
		return;
	}
	
	int a_height = ab - at;
	int b_height = bb - bt;
	
	delta_height = (double)(b_height - a_height) / (double)width;
	
	int y_center_a = (ab - (a_height / 2));
	int y_center_b = (bb - (b_height / 2));
	
	delta_elevation = (y_center_b - y_center_a) / (double)width;
}

void quad_t::swapPoints() {
	int t = bx;
	bx = ax;
	ax = t;
	t = bt;
	bt = at;
	at = t;
	t=bb;
	bb = ab;
	ab = t;
}

quad_t::quad_t(int ax, int bx, int at, int ab, int bt, int bb, unsigned color, quad_type_t type, plane_t* plane) :
ax(ax), bx(bx), at(at), ab(ab), bt(bt), bb(bb), color(color), type(type), plane(plane) {
	
}


void Renderer::DrawPoint(int x, int y, unsigned int color)
{
    bool is_out_of_bounds = (x < 0 or x > width or y < 0 or y >= height);
    bool is_outside_mem_buff = (width * y + x) >= (width * height);

    if (is_out_of_bounds or is_outside_mem_buff)
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

    for (;;)
    {
        DrawPoint(x0, y0, color);
        if (x0 == x1 and y0 == y1)
            break;

        e2 = err;

        if (e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void Renderer::clear() {
	memset(screen_buffer, 0, sizeof(uint32_t) * width * height);
}

void Renderer::clipBehindPlayer(double& ax, double& ay, double bx, double by)
{
    double px1 = 1;
    double py1 = 1;
    double px2 = 200;
    double py2 = 1;

    double a = (px1 - px2) * (ay - py2) - (py1 - py2) * (ax - px2);
    double b = (py1 - py2) * (ax - bx) - (px1 - px2) * (ay - by);
	
	double t;
	
	t = SafeDivide(a, b);
	
    ax = ax - (t * (bx - ax));
    ay = ay - (t * (by - ay));
}

void Renderer::DrawSectors(player_t& player) {
	double half_w = width/2;
	double half_h = height/2;
	double fov = 250;
	unsigned wall_color = 0x3355AAFF;
	clear();
	
	for (int i = 0; i < sector_queue.size(); i++) {
		sector_t *s = &sector_queue[i];
		int sector_h = s->height;
		int sector_e = s->elevation;
		int sector_clr = s->color;
		
		for(int k = 0; k < 1024; k++) {
			s->ceil.t[k] = 0;
			s->ceil.b[k] = 0;
			s->floor.t[k] = 0;
			s->floor.b[k] = 0;
			
			s->portals_ceil.t[k] = 0;
			s->portals_ceil.b[k] = 0;
			s->portals_floor.t[k] = 0;
			s->portals_floor.b[k] = 0;
		}
		
		for (int j = 0; j < s->walls.size(); j++) {
			wall_t *w = &s->walls[j];
			
			double dx1 = w->a.x - player.position.x;
			double dy1 = w->a.y - player.position.y;
			double dx2 = w->b.x - player.position.x;
			double dy2 = w->b.y - player.position.y;
			
            double SN = sin(player.angle);
            double CN = cos(player.angle);
            double wx1 = dx1 * SN - dy1 * CN;
            double wz1 = dx1 * CN + dy1 * SN;
            double wx2 = dx2 * SN - dy2 * CN;
            double wz2 = dx2 * CN + dy2 * SN;
			
             if (wz1 < 0 and wz2 < 0)
                 continue;
             else if (wz1 < 0)
                 clipBehindPlayer(wx1, wz1, wx2, wz2);
             else if (wz2 < 0)
                 clipBehindPlayer(wx2, wz2, wx1, wz1);
			
			if (wz1<=1)wz1++;
			
			if (wz2<=1)wz2++;
			
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
			
            // // // top
            // DrawLine(sx1, sy1 - wh1, sx2, sy2 - wh2, sector_clr);
            // // // bottom
            // DrawLine(sx1, sy1, sx2, sy2, sector_clr);
            // // // left edge
            // DrawLine(sx1, sy1 - wh1, sx1, sy1, sector_clr);
            // // // right edge
            // DrawLine(sx2, sy2 - wh2, sx2, sy2, sector_clr);
			
			// if (w->is_portal) {
				// DrawLine(sx1, sy1 - wh1 + pth1, sx2, sy2 - wh2 + pth2, sector_clr);
				// DrawLine(sx1, sy1 - pbh1, sx2, sy2 - pbh2, sector_clr);
			// }
			
			if (w->is_portal) {
				quad_t qt(sx1, sx2, sy1 - wh1, sy1 - wh1 + pth1, sy2 - wh2, sy2 - wh2 + pth2, s->ceil_color, CEILING, &s->portals_ceil);
				quad_t qb(sx1, sx2, sy1 - pbh1, sy1, sy2 - pbh2, sy2, s->floor_color, FLOOR, &s->portals_floor);
				
				rasterize(qt);
				qt.plane = &s->portals_floor;
				qt.type = FLOOR;
				qt.color = s->floor_color;
				rasterize(qt);
				qt.plane = NULL;
				qt.type = WALL;
				qt.color = s->color;
				rasterize(qt);
				
				rasterize(qb);
				qb.plane = &s->portals_ceil;
				qb.type = FLOOR;
				qb.color = s->ceil_color;
				rasterize(qb);
				qb.plane = NULL;
				qb.type = WALL;
				qb.color = s->color;
				rasterize(qb);
			
			}
			else {
				quad_t qt(sx1, sx2, sy1 - wh1, sy1, sy2 - wh2, sy2, s->ceil_color, CEILING, &s->ceil);
				rasterize(qt);
				qt.plane = &s->floor;
				qt.type = FLOOR;
				qt.color = s->color;
				rasterize(qt);
				qt.plane = NULL;
				qt.type = WALL;
				qt.color = s->color;
			}
		}
		
		for (int x = 1; x < 1024; x++) {
			int cy1 = s->ceil.t[x];
			int cy2 = s->ceil.b[x];
			int cf1 = s->floor.t[x];
			int cf2 = s->floor.b[x];
			
			int pcy1 = s->portals_ceil.t[x];
			int pcy2 = s->portals_ceil.b[x];
			int pfy1 = s->portals_floor.t[x];
			int pfy2 = s->portals_floor.b[x];
			
			if((player.z > s->elevation + s->height) and (cy1 > cy2) and (cy1 and cy2)) {
				DrawLine(x, cy1, x, cy2, s->ceil_color);
			}
			
			if ((player.z < s->elevation) and (pfy1 < pfy2) and (pfy1 or pfy2))
				DrawLine(x, pfy1, x, pfy2, s->color);
			
			if (pcy1 > pcy2 and (pcy1 and pcy2))
				DrawLine(x, pcy1, x, pcy2, s->ceil_color);
			
			if (pfy1 < pfy2 and (pfy1 or pfy2))
				DrawLine(x, pfy1, x, pfy2, s->color);
			
		}
	}
}
