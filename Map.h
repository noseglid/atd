#ifndef __MAP_H__
#define __MAP_H__

#include "Path.h"
#include "math/Math.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <OpenGL/GL.h>

#include <pjson.hpp>

struct GameEvent;

class Map
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
	void tick(const GameEvent& ge);
	void mousemotion(const GameEvent& ge);
	void keydown(const GameEvent& ge);

	void draw() const;
	void draw_normals() const;
	Vector3 get_center_of(int x, int y) const;
	const Path *get_path() const;
	size_t get_width() const;
	size_t get_height() const;

	void set_hightlight(const int& x, const int& y);
	Vector2 get_highlight() const;

	static Map& instance();
};

#endif
