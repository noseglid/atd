#ifndef MODEL_H
#define MODEL_H

#include "gl/VBO.h"
#include "gl/ShaderProgram.h"
#include "gl/glm.h"

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
  GLint id;
  aiMatrix4x4 begin, end;
  aiMatrix4x4 offset;
  aiMatrix4x4 current;
};

class Model
{
  struct VertexBoneInfluence
  {
    GLfloat boneids[4];
    GLfloat weights[4];

    VertexBoneInfluence();
    void add(GLint boneid, GLfloat weight);
  };

  static std::map<std::string, Model*> loaded_models;

  Assimp::Importer importer;
  glm::vec3 scene_min, scene_max, scene_center;
  int n_vertices;

  std::vector<gl::VBO*> vertex_buffers;

  const aiScene *scene;
  std::map<unsigned int, GLuint> textures;

  int bone_index;
  std::map<std::string, struct bone> bones;

  void interpolated_rotation(aiQuaternion& out, float animtime, const aiNodeAnim *nodeanim);
  void interpolated_position(aiMatrix4x4& out, float animtime, const aiNodeAnim *nodeanim);
  void interpolated_scale(aiMatrix4x4& out, float animtime, const aiNodeAnim *nodeanim);

  void load_nodes(const aiScene *scene, const aiNode *node);
  void load_textures(const aiScene *scene, const aiMesh* mesh);
  struct bone load_bone(const aiScene *scene, const aiBone* bone);
  void load_bones(const aiScene *scene, const aiMesh* mesh);
  void build_vbo(const aiNode* node);

  void get_bounding_box_for_node(const aiNode* nd, glm::vec3& min, glm::vec3& max, aiMatrix4x4* trafo);
  void get_bounding_box(glm::vec3& min, glm::vec3& max);

  void animate(aiNode *node, float animtime, const aiMatrix4x4& tp);
  void rrender(const aiNode* node);
  void rrenderbones(const aiNode* node);

  Model(std::string file);

public:
  ~Model();

  static Model *load(std::string file);

  void normalize();
  void draw(float animtime, gl::ShaderProgram *shader = NULL);
  void bounding_box(glm::vec3& min, glm::vec3& max) const;

  int get_vertex_count() const;
};

#endif
