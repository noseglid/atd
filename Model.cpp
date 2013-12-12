#include "Model.h"
#include "Debug.h"
#include "ModelException.h"
#include "ImageLoader.h"

#include <time.h>
#include <sstream>

std::map<std::string, Model*> Model::loaded_models;

gl::ShaderProgram *shader_program = NULL;

Model::VertexBoneInfluence::VertexBoneInfluence()
{
  memset(boneids, 0, 4 * sizeof(GLfloat));
  memset(weights, 0, 4 * sizeof(GLfloat));
}

void
Model::VertexBoneInfluence::add(GLint boneid, GLfloat weight)
{
  for (int i = 0; i < 4; ++i) {
    if (boneids[i] == 0 && weights[i] == 0.0) {
      boneids[i] = boneid;
      weights[i] = weight;
      return;
    }
  }

  /* Ooops, more bones/weights than we have room for */
  throw ModelException("More than 4 bones is not supported");
}

Model::Model(std::string file) : n_vertices(0), bone_index(0)
{
  DBG("Loading model: " << file);

  clock_t begin = clock();

  importer.ReadFile(file, aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs);
  scene = importer.GetScene();
  if (NULL == scene) throw ModelException("Could not load file: " + file);

  get_bounding_box(scene_min, scene_max);
  scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
  scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
  scene_center.z = (scene_min.z + scene_max.z) / 2.0f;

  load_nodes(scene, scene->mRootNode);
  build_vbo(scene->mRootNode);

  for (size_t i = 0; i < scene->mNumMeshes; ++i) {
    n_vertices += scene->mMeshes[i]->mNumVertices;
  }

  DBG("Loaded '" << file << "', " << n_vertices << " vertices in "
    << (float)(clock() - begin)/CLOCKS_PER_SEC << " s");
}

Model::~Model()
{
  delete scene;

  for (gl::VBO *vbo : vertex_buffers) {
    delete vbo;
  }
}

void
Model::load_nodes(const aiScene *scene, const aiNode *node)
{
  for (unsigned int n = 0; n < node->mNumMeshes; ++n) {
    const aiMesh *mesh = scene->mMeshes[node->mMeshes[n]];
    load_textures(scene, mesh);
    load_bones(scene, mesh);
  }

  for (unsigned int n = 0; n < node->mNumChildren; ++n) {
    load_nodes(scene, node->mChildren[n]);
  }
}

void
Model::load_textures(const aiScene *scene, const aiMesh *mesh)
{
  const aiMaterial *mtl = scene->mMaterials[mesh->mMaterialIndex];
  if (0 == mtl->GetTextureCount(aiTextureType_DIFFUSE)) {
    return;
  }

  struct aiString path;
  mtl->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL);
  try {
    GLuint t = IL::GL::texture(path.data);
    textures.insert(std::make_pair(mesh->mMaterialIndex, t));
  } catch (const std::exception& e) {
    throw ModelException("Warning: could not load texture from: " +  std::string(path.data));
  }
}

struct bone
Model::load_bone(const aiScene *scene, const aiBone *bone)
{
  std::string name(bone->mName.data);
  const aiNode *node;
  struct bone b;

  node = scene->mRootNode->FindNode(name.c_str());
  do {
    b.begin = node->mTransformation * b.begin;
  } while ((node = node->mParent));

  node = scene->mRootNode->FindNode(name.c_str())->mParent;
  while (node) {
    b.end = node->mTransformation * b.end;
    node = node->mParent;
  }

  b.offset = bone->mOffsetMatrix;
  b.id = bone_index++;

  return b;
}

void
Model::load_bones(const aiScene *scene, const aiMesh *mesh)
{
  for (unsigned int i = 0; i < mesh->mNumBones; ++i) {
    std::string name(mesh->mBones[i]->mName.data);
    bones[name] = load_bone(scene, mesh->mBones[i]);
  }
}

void
Model::build_vbo(const aiNode* node)
{
  for (unsigned int n = 0; n < node->mNumMeshes; ++n) {
    const aiMesh *mesh = scene->mMeshes[node->mMeshes[n]];
    const aiMaterial *mtl = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<GLfloat> vertices, normals, texcoords;
    for (size_t i = 0; i < mesh->mNumVertices; ++i) {
      aiVector3D v = node->mTransformation * mesh->mVertices[i];
      aiVector3D n = node->mTransformation * mesh->mNormals[i];
      vertices.push_back(v.x);
      vertices.push_back(v.y);
      vertices.push_back(v.z);
      normals.push_back(n.x);
      normals.push_back(n.y);
      normals.push_back(n.z);
      texcoords.push_back(mesh->mTextureCoords[0][i].x);
      texcoords.push_back(mesh->mTextureCoords[0][i].y);
    }

    std::vector<GLushort> indices;
    for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
      const aiFace *face = &mesh->mFaces[t];
      for (unsigned int i = 0; i < face->mNumIndices; ++i) {
        unsigned short index = face->mIndices[i];
        indices.push_back(index);
      }
    }

    std::vector<VertexBoneInfluence> influences(indices.size());
    for (size_t b = 0; b < mesh->mNumBones; ++b) {
      const aiBone *abone = mesh->mBones[b];
      struct bone bone = bones[abone->mName.data];

      for (size_t w = 0; w < abone->mNumWeights; ++w) {
        influences[abone->mWeights[w].mVertexId].add(bone.id, abone->mWeights[w].mWeight);
      }
    }

    if (0 == mesh->mNumBones) {
      /* If there are no bones, create a dummy influence where bone index 0
       * have all weight. In render bone index 0 will be set to identity */
      for (size_t i = 0; i < influences.size(); ++i) {
        influences[i].add(0, 1.0);
      }
    }

    gl::VBO *vbo = new gl::VBO();
    if (0 != mtl->GetTextureCount(aiTextureType_DIFFUSE)) {
      vbo->set_texture(textures[mesh->mMaterialIndex]);
    }

    vbo->set_material(gl::Material::from_assimp(mtl));

    vbo->bind_indices(indices);
    vbo->bind_data(gl::VBO::TEXCOORD, texcoords);
    vbo->bind_data(gl::VBO::VERTEX,   vertices);
    vbo->bind_data(gl::VBO::NORMAL,   normals);

    GLuint buffer = vbo->create_vertex_attrib(influences);

    GLint boneids_location = shader_program->getAttribLocation("boneids");
    vbo->set_vertex_attrib(boneids_location, buffer, 4, GL_FLOAT, sizeof(VertexBoneInfluence), 0);
    GLint weights_location = shader_program->getAttribLocation("weights");
    vbo->set_vertex_attrib(weights_location, buffer, 4, GL_FLOAT, sizeof(VertexBoneInfluence), 4 * sizeof(GLfloat));

    vertex_buffers.push_back(vbo);
  }

  for (unsigned int c = 0; c < node->mNumChildren; ++c) {
    build_vbo(node->mChildren[c]);
  }
}

void
Model::get_bounding_box_for_node(
  const aiNode* nd,
  glm::vec3& min,
  glm::vec3& max,
  aiMatrix4x4* trafo)
{
  *trafo = (*trafo) * (nd->mTransformation);

  for (unsigned int n = 0; n < nd->mNumMeshes; ++n) {
    const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
    for (unsigned int t = 0; t < mesh->mNumVertices; ++t) {

      aiVector3D tmp = mesh->mVertices[t];
      tmp *= *trafo;

      min.x = std::min(min.x, tmp.x);
      min.y = std::min(min.y, tmp.y);
      min.z = std::min(min.z, tmp.z);

      max.x = std::max(max.x, tmp.x);
      max.y = std::max(max.y, tmp.y);
      max.z = std::max(max.z, tmp.z);
    }
  }

  for (unsigned int n = 0; n < nd->mNumChildren; ++n) {
    get_bounding_box_for_node(nd->mChildren[n], min, max, trafo);
  }
}

void
Model::get_bounding_box(glm::vec3& min, glm::vec3& max)
{
  aiMatrix4x4 trafo;

  min.x = min.y = min.z =  1e10f;
  max.x = max.y = max.z = -1e10f;
  get_bounding_box_for_node(scene->mRootNode, min, max, &trafo);
}

unsigned int
find_rotation(float animtime, const aiNodeAnim *nodeanim)
{
  for (unsigned int i = 0; i < nodeanim->mNumRotationKeys - 1; ++i) {
    if (animtime < nodeanim->mRotationKeys[i + 1].mTime) {
      return i;
    }
  }

  throw ModelException("Could not find rotation key.");
}

void
Model::interpolated_rotation(aiQuaternion& out, float animtime, const aiNodeAnim *nodeanim)
{
  if (1 == nodeanim->mNumRotationKeys) {
    out = nodeanim->mRotationKeys[0].mValue;
    return;
  }

  unsigned int currot = find_rotation(animtime, nodeanim);
  unsigned int nextrot = currot + 1;

  float delta = nodeanim->mRotationKeys[nextrot].mTime - nodeanim->mRotationKeys[currot].mTime;
  float factor = (animtime - (float)nodeanim->mRotationKeys[currot].mTime) / delta;

  aiQuaternion& start = nodeanim->mRotationKeys[currot].mValue;
  aiQuaternion& end   = nodeanim->mRotationKeys[nextrot].mValue;

  aiQuaternion::Interpolate(out, start, end, factor);
  out.Normalize();
}

unsigned int
find_position(float animtime, const aiNodeAnim *nodeanim)
{
  for (unsigned int i = 0; i < nodeanim->mNumPositionKeys - 1; ++i) {
    if (animtime < nodeanim->mPositionKeys[i + 1].mTime) {
      return i;
    }
  }

  throw ModelException("Could not find position key.");
}

void
Model::interpolated_position(aiMatrix4x4& out, float animtime, const aiNodeAnim *nodeanim)
{
  if (1 == nodeanim->mNumPositionKeys) {
    aiMatrix4x4::Translation(nodeanim->mPositionKeys[0].mValue, out);
    return;
  }

  unsigned int idx = find_position(animtime, nodeanim);
  aiVectorKey pre  = nodeanim->mPositionKeys[idx];
  aiVectorKey post = nodeanim->mPositionKeys[idx + 1];

  float timedelta = post.mTime - pre.mTime;
  float factor = (animtime - (float)pre.mTime) / timedelta;
  aiVector3D delta = post.mValue - pre.mValue;

  aiVector3D res = pre.mValue + factor * delta;

  aiMatrix4x4::Translation(res, out);
}

unsigned int
find_scale(float animtime, const aiNodeAnim *nodeanim)
{
  for (unsigned int i = 0; i < nodeanim->mNumScalingKeys - 1; ++i) {
    if (animtime < nodeanim->mScalingKeys[i + 1].mTime) {
      return i;
    }
  }

  throw ModelException("Could not find scaling key.");
}

void
Model::interpolated_scale(aiMatrix4x4& out, float animtime, const aiNodeAnim *nodeanim)
{
  if (1 == nodeanim->mNumScalingKeys) {
    aiMatrix4x4::Scaling(nodeanim->mScalingKeys[0].mValue, out);
    return;
  }

  unsigned int idx = find_scale(animtime, nodeanim);
  aiVectorKey pre  = nodeanim->mScalingKeys[idx];
  aiVectorKey post = nodeanim->mScalingKeys[idx + 1];

  float timedelta = post.mTime - pre.mTime;
  float factor = (animtime - (float)pre.mTime) / timedelta;
  aiVector3D delta = post.mValue - pre.mValue;

  aiVector3D res = pre.mValue + factor * delta;

  aiMatrix4x4::Scaling(res, out);
}

static const aiNodeAnim *
find_nodeanim(const aiAnimation *anim, std::string name)
{
  for (size_t i = 0; i < anim->mNumChannels; ++i) {
    if (std::string(anim->mChannels[i]->mNodeName.data) == name) {
      return anim->mChannels[i];
    }
  }

  return NULL;
}

void
Model::animate(aiNode *node, float animtime, const aiMatrix4x4& tp)
{
  std::string node_name(node->mName.data);
  const aiNodeAnim *nodeanim = find_nodeanim(scene->mAnimations[0], node_name);
  aiMatrix4x4 trafo = node->mTransformation;

  auto bit = bones.find(node_name);
  if (bones.end() == bit) {
    goto cont;
  }

  if (NULL != nodeanim) {
    aiMatrix4x4 mrotation, mposition, mscale;

    try {
      aiQuaternion qrotation;
      interpolated_rotation(qrotation, animtime, nodeanim);
      mrotation = aiMatrix4x4(qrotation.GetMatrix());
    } catch (ModelException& e) {}

    try {
      interpolated_position(mposition, animtime, nodeanim);
    } catch (ModelException& e) {}

    try {
      interpolated_scale(mscale, animtime, nodeanim);
    } catch (ModelException& e) {}

    trafo = mposition * mrotation * mscale;
  }

cont:
  trafo = tp * trafo;

  if (bones.end() != bit) {
    bit->second.end   = tp;
    bit->second.begin = trafo;
    bit->second.current = trafo * bit->second.offset;
    bit->second.current.Transpose();
  }

  for (unsigned int c = 0; c < node->mNumChildren; ++c) {
    animate(node->mChildren[c], animtime, trafo);
  }
}

void
Model::normalize()
{
  GLfloat tmp = scene_max.x - scene_min.x;
  tmp = std::max(scene_max.y - scene_min.y, tmp);
  tmp = std::max(scene_max.z - scene_min.z, tmp);
  tmp = 1.f / tmp;
  glScalef(tmp, tmp, tmp);
}

void
Model::draw(float elapsed, gl::ShaderProgram *shader)
{
  if (NULL == shader) shader = shader_program;
  shader->use();

  if (0 < scene->mNumAnimations) {
    const aiAnimation *anim = scene->mAnimations[0];
    float animtime = fmod(elapsed * anim->mTicksPerSecond, anim->mDuration);
    this->animate(scene->mRootNode, animtime, aiMatrix4x4());

    for (auto bone : bones) {
      std::stringstream ss;
      ss << "bones[" << bone.second.id << "]";
      GLint loc = shader->getUniformLocation(ss.str());
      glUniformMatrix4fv(loc, 1, GL_FALSE, (GLfloat*)&bone.second.current);
    }
  } else {
    /* No animations, upload identity matrix as bones[0] */
    GLint loc = shader->getUniformLocation("bones[0]");
    aiMatrix4x4 identity;
    glUniformMatrix4fv(loc, 1, GL_FALSE, (GLfloat*)&identity);
  }

  for (gl::VBO *vbo : vertex_buffers) {
    vbo->draw();
  }

  shader->reset();
}

void
Model::bounding_box(glm::vec3& min, glm::vec3& max) const
{
  min = scene_min;
  max = scene_max;
}

int
Model::get_vertex_count() const
{
  return n_vertices;
}

Model *
Model::load(std::string file)
{
  if (NULL == shader_program) {
    shader_program = gl::ShaderProgram::create({ "model.v.glsl" }, { "default.f.glsl", "fog.f.glsl" });
  }

  auto it = loaded_models.find(file);
  if (loaded_models.end() != it) {
    return it->second;
  }

  Model *m = new Model(file);
  loaded_models.insert(std::make_pair(file, m));
  return m;
}
