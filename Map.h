#ifndef __MAP_H__
#define __MAP_H__

#include "Path.h"
#include "Math.h"
#include "Listener.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <OpenGL/GL.h>

#include <pjson.hpp>

class Map : public Listener
{
	size_t width, height;
	Vector2 highlighted;
	bool draw_meta;

	Path *path;
	std::vector<GLint> textures;
	float **heightmap;
	Vector3 **normals;

	void load_textures(const Json::Value& v);
	void create_heightmap();
	void generate_normals();

	void draw_square(const int&x, const int& y) const;

public:
	Map();
	~Map();

	void load(const std::string& file);
	void event(const float& elapsed);
	void event(const SDL_MouseMotionEvent& event);
	void event(const SDL_KeyboardEvent& event);

	void draw() const;
	void draw_normals() const;
	Vector3 get_center_of(int x, int y) const;
	const Path * get_path() const;

	void set_hightlight(const int& x, const int& y);
	Vector2 get_highlight() const;
};

#endif
