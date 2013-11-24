#ifndef MODEL_H
#define MODEL_H

#include "VBO.h"
#include "math/Matrix4.h"

#include <SDL/SDL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>

#include <map>
#include <iostream>

struct bone
{
  aiMatrix4x4 begin, end;
  aiMatrix4x4 offset;
};

class Model
{
  static std::map<std::string, Model*> loaded_models;

  Assimp::Importer importer;
  aiVector3D scene_min, scene_max, scene_center;
  int n_vertices;

  std::vector<VBO *> vertex_buffers;

  void get_bounding_box_for_node(const aiNode* nd, aiVector3D& min, aiVector3D& max, aiMatrix4x4* trafo);
  void get_bounding_box (aiVector3D& min, aiVector3D& max);

  const aiScene *scene;
  std::map<unsigned int, GLuint> textures;
  std::map<std::string, struct bone> bones;

  void interpolated_rotation(aiQuaternion& out, float animtime, const aiNodeAnim *nodeanim);
  void interpolated_position(aiMatrix4x4& out, float animtime, const aiNodeAnim *nodeanim);
  void interpolated_scale(aiMatrix4x4& out, float animtime, const aiNodeAnim *nodeanim);

  void load_nodes(const aiScene *sceme, const aiNode *node);
  void load_textures(const aiScene *scene, const aiMesh* mesh);
  void load_bones(const aiScene *scene, const aiMesh* mesh);
  void build_vbo(const aiNode* node, aiMatrix4x4 trafo);

  void animate(aiNode *node, float animtime, const aiMatrix4x4& tp);
  void rrender(const aiNode* node);
  void rrenderbones(const aiNode* node);

  Model(std::string file);

public:
  ~Model();

  static Model *load(std::string file);

  void normalize();
  void draw(float animtime, float opacity = 1.0, bool bones = false);

  int get_vertex_count() const;
};

#endif
