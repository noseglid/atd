#ifndef MODEL_H
#define MODEL_H

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

  void animate(aiNode *node, float animtime, const aiMatrix4x4& tp);
  void apply_material(const aiMaterial *mtl);
  void rrender(const aiNode* node);
  void rrenderbones(const aiNode* node);

  Model(std::string file);

public:
  ~Model();

  static Model *load(std::string file);

  void normalize();
  void draw(float animtime);
};

#endif
