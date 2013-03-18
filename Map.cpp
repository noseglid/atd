#include "Map.h"
#include "Game.h"
#include "Math.h"
#include "ImageLoader.h"
#include "Exception.h"

#include <iostream>
#include <iterator>
#include <OpenGL/gl.h>
#include <pjson.hpp>

#define SQSIZE 1.0f /* ALTERING NOT SUPPORTED YET */

Map::Map()
{
	draw_meta = false;
	highlighted.x = 20;
	highlighted.y = 10;

	Game::instance().register_listener(this)
		<< LE_TICK
		<< SDL_MOUSEMOTION
		<< SDL_KEYDOWN;
}

Map::~Map()
{
	for (size_t h = 0; h < height; ++h) {
		delete[] heightmap[h];
		delete[] normals[h];
	}
	delete[] heightmap;
	delete[] normals;

	delete path;
}

void
Map::load(const std::string& file)
{
	DBG("Loading file: " << file);
	std::ifstream ifs(file.c_str());
	if (!ifs.good()) throw Exception(std::string("Could not open file: ") + file);

	std::string content(
		(std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>())
	);
	Json::Value map = Json::deserialize(content);

	/* Load metadata */
	width  = map["width"].asInt();
	height = map["height"].asInt();

	/* Load the textures */
	for (Json::Value entry : map["textures"].asArray()) {
		textures.push_back(IL::GL::texture(entry.asString()));
	}

	/* Load the paths */
	Json::Array paths = map["paths"].asArray();
	if (paths.size() != 1) {
		throw Exception("Only one path per map currently supported.");
	}
	Json::Value p = paths[0];
	path = new Path(p["texture"].asInt(), p["path"].asString());

	create_heightmap();
	generate_normals();
}

void
Map::create_heightmap()
{
	heightmap = new float*[height];
	for (size_t h = 0; h < height; ++h) {
		heightmap[h] = new float[width];
		for (size_t w = 0; w < width; ++w) {
			heightmap[h][w] = (rand() / (float)RAND_MAX) / 5.0f;
		}
	}
}

void
Map::generate_normals()
{
	normals = new Vector3*[height];
	for (size_t h = 0; h < height; ++h) {
		normals[h] = new Vector3[width];
		normals[h][0]         = Vector3(0.0f, 1.0f, 0.0f);
		normals[h][width - 1] = Vector3(0.0f, 1.0f, 0.0f);
	}

	for (size_t w = 0; w < width; ++w) {
		normals[0][w]          = Vector3(0.0f, 1.0f, 0.0f);
		normals[height - 1][w] = Vector3(0.0f, 1.0f, 0.0f);
	}

	for (size_t h = 1; h < height - 1; ++h) {
		for (size_t w = 1; w < width - 1; ++w) {
			Vector3 v1(0,      heightmap[h - 1][w], SQSIZE);
			Vector3 v2(SQSIZE, heightmap[h][w + 1], 0);
			Vector3 v3(0,      heightmap[h + 1][w], -SQSIZE);
			Vector3 v4(-SQSIZE, heightmap[h][w - 1], 0);

			Vector3 v21 = v1 * v2;
			Vector3 v32 = v2 * v3;
			Vector3 v43 = v3 * v4;
			Vector3 v14 = v4 * v1;

			normals[h][w] = v21 + v32 + v43 + v14;
			normals[h][w].normalize();
		}
	}
}

void
Map::event(const float& elapsed)
{
	this->draw();
	if (draw_meta) this->draw_normals();
}

void
Map::event(const SDL_KeyboardEvent& event)
{
	switch (event.keysym.sym) {
		case SDLK_m: draw_meta = !draw_meta; break;
		default: break;
	}
}

void
Map::event(const SDL_MouseMotionEvent& event)
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	Vector3 v = unproject(x, y);
	set_hightlight(floor(v.x), floor(v.z));
}

void
Map::draw_square(const int& x, const int& y) const
{
	static const GLfloat texcoords[4][2] = {
		{ 0.0f, 0.0f },
		{ 0.0f, 1.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f }
	};

	GLfloat emission[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	if (y == highlighted.y && x == highlighted.x) {
		emission[0] = emission[1] = emission[2] = 0.8f;
	}
	glMaterialfv(GL_FRONT, GL_EMISSION, emission);

	int textureid = (path->has_coord(x, y)) ? path->textureid : 0;
	glBindTexture(GL_TEXTURE_2D, textures[textureid]);

	glBegin(GL_TRIANGLE_STRIP);
	glNormal3fv((GLfloat*)&(normals[y][x]));
	glTexCoord2fv(texcoords[0]);
	glVertex3f(x, heightmap[y][x], y);

	glNormal3fv((GLfloat*)&(normals[y + 1][x]));
	glTexCoord2fv(texcoords[1]);
	glVertex3f(x, heightmap[y + 1][x], y + SQSIZE);

	glNormal3fv((GLfloat*)&(normals[y][x + 1]));
	glTexCoord2fv(texcoords[2]);
	glVertex3f(x + SQSIZE, heightmap[y][x + 1], y);

	glNormal3fv((GLfloat*)&(normals[y + 1][x + 1]));
	glTexCoord2fv(texcoords[3]);
	glVertex3f(x + SQSIZE, heightmap[y + 1][x + 1], y + SQSIZE);
	glEnd();
}

void
Map::draw() const
{
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);

	GLfloat
		diffuse[]  = { 0.8f, 0.8f, 0.8f, 1.0f },
		specular[] = { 0.0f, 0.0f, 0.0f, 1.0f },
		ambient[]  = { 0.1f, 0.1f, 0.1f, 1.0f };

	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);

	for (size_t h = 0; h < height - 1; ++h) {
		for (size_t w = 0; w < width - 1; ++w) {
			draw_square(w, h);
		}
	}
}

void
Map::draw_normals() const
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glColor3f(1.0f, 1.0f, 1.0f);

	for (size_t h = 0; h < height; ++h) {
		for (size_t w = 0; w < width; ++w) {
			glBegin(GL_LINES);
			Vector3 n = normals[h][w];
			n /= 4;
			glVertex3f(w, heightmap[h][w], h);
			glVertex3f(w + n.x, heightmap[h][w] + n.y, h + n.z);
			glEnd();
		}
	}

	glColor3f(0.5f, 0.5f, 0.5f);
	glEnable(GL_DEPTH_TEST);
}

Vector3
Map::get_center_of(int x, int y) const
{
	float dx = (float)x + SQSIZE / 2.0f;
	float dy = (
		heightmap[y][x] + heightmap[y][x + 1] + heightmap[y + 1][x] + heightmap[y + 1][x + 1]
	) / 4;
	float dz = (float)y + SQSIZE / 2.0f;

	return Vector3(dx, dy, dz);
}

const Path *
Map::get_path() const
{
	return path;
}

void
Map::set_hightlight(const int& x, const int& y)
{
	if (path->has_coord(x, y)) {
		highlighted.x = highlighted.y = -1;
		return;
	}

	highlighted.x = x;
	highlighted.y = y;
}

Vector2
Map::get_highlight() const
{
	return highlighted;
}
