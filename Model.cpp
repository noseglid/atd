#include "Model.h"
#include "Debug.h"
#include "ModelException.h"
#include "ImageLoader.h"
#include "gl/Shader.h"
#include "gl/ShaderProgram.h"

#include <time.h>

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

std::map<std::string, Model*> Model::loaded_models;

gl::ShaderProgram *shader_program = NULL;

void
Model::get_bounding_box_for_node(
  const aiNode* nd,
  aiVector3D& min,
  aiVector3D& max,
  aiMatrix4x4* trafo)
{
  aiMatrix4x4 prev;
  unsigned int n = 0, t;

  prev = *trafo;
  *trafo = (*trafo) * (nd->mTransformation);

  for (; n < nd->mNumMeshes; ++n) {
    const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
    for (t = 0; t < mesh->mNumVertices; ++t) {

      aiVector3D tmp = mesh->mVertices[t];
      tmp *= *trafo;

      min.x = aisgl_min(min.x, tmp.x);
      min.y = aisgl_min(min.y, tmp.y);
      min.z = aisgl_min(min.z, tmp.z);

      max.x = aisgl_max(max.x, tmp.x);
      max.y = aisgl_max(max.y, tmp.y);
      max.z = aisgl_max(max.z, tmp.z);
    }
  }

  for (n = 0; n < nd->mNumChildren; ++n) {
    get_bounding_box_for_node(nd->mChildren[n], min, max, trafo);
  }
  *trafo = prev;
}

void
Model::get_bounding_box(aiVector3D& min, aiVector3D& max)
{
  aiMatrix4x4 trafo;

  min.x = min.y = min.z =  1e10f;
  max.x = max.y = max.z = -1e10f;
  get_bounding_box_for_node(scene->mRootNode, min, max, &trafo);
}

Model::Model(std::string file) : n_vertices(0)
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

  build_vbo(scene->mRootNode, aiMatrix4x4());

  for (size_t i = 0; i < scene->mNumMeshes; ++i) {
    n_vertices += scene->mMeshes[i]->mNumVertices;
  }

  DBG("Loaded '" << file << "', " << n_vertices << " vertices in "
    << (float)(clock() - begin)/CLOCKS_PER_SEC << " s");
}

Model::~Model()
{
  delete scene;

  for (VBO *vbo : vertex_buffers) {
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

static struct bone
load_bone(const aiScene *scene, const aiBone *bone)
{
  std::string name(bone->mName.data);
  const aiNode *node;
  struct bone b;

  node = scene->mRootNode->FindNode(name.c_str());
  do {
    b.begin = node->mTransformation * b.begin;
  } while ((node = node->mParent));
  b.begin.Transpose();

  node = scene->mRootNode->FindNode(name.c_str())->mParent;
  while (node) {
    b.end = node->mTransformation * b.end;
    node = node->mParent;
  }
  b.end.Transpose();

  b.offset = bone->mOffsetMatrix;
  b.offset.Transpose();

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
Model::build_vbo(const aiNode* node, aiMatrix4x4 trafo)
{
  trafo *= node->mTransformation;

  for (unsigned int n = 0; n < node->mNumMeshes; ++n) {
    const aiMesh *mesh = scene->mMeshes[node->mMeshes[n]];
    const aiMaterial *mtl = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<GLfloat> vertices, normals, texcoords;
    for (size_t i = 0; i < mesh->mNumVertices; ++i) {
      aiVector3D v = trafo * mesh->mVertices[i];
      aiVector3D n = trafo * mesh->mNormals[i];
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

    VBO *vbo = new VBO();
    if (0 != mtl->GetTextureCount(aiTextureType_DIFFUSE)) {
      vbo->set_texture(textures[mesh->mMaterialIndex]);
    }

    GLfloat shininess = 64.0;
    aiColor4D ambient, diffuse, specular, emissive;
    if (aiReturn_SUCCESS != mtl->Get(AI_MATKEY_SHININESS, shininess)) {
      DBGWRN("Could not get shininess component. Will use " << shininess << "instead.");
    }
    if (aiReturn_SUCCESS == mtl->Get(AI_MATKEY_COLOR_AMBIENT, ambient)) {
      vbo->mtl_ambient((GLfloat*)&ambient);
    }
    if (aiReturn_SUCCESS == mtl->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse)) {
      vbo->mtl_diffuse((GLfloat*)&diffuse);
    }
    if (aiReturn_SUCCESS == mtl->Get(AI_MATKEY_COLOR_SPECULAR, specular)) {
      vbo->mtl_specular((GLfloat*)&specular, shininess);
    }
    if (aiReturn_SUCCESS == mtl->Get(AI_MATKEY_COLOR_EMISSIVE, emissive)) {
      vbo->mtl_emission((GLfloat*)&emissive);
    }

    vbo->bind_indices(indices);
    vbo->bind_data(VBO::TEXCOORD, texcoords);
    vbo->bind_data(VBO::VERTEX,   vertices);
    vbo->bind_data(VBO::NORMAL,   normals);

    vertex_buffers.push_back(vbo);
  }

  for (unsigned int c = 0; c < node->mNumChildren; ++c) {
    build_vbo(node->mChildren[c], trafo);
  }
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

void
Model::rrender(const aiNode *node)
{
  //glPushMatrix();

  //aiMatrix4x4 trafo = node->mTransformation;
  //glMultTransposeMatrixf((GLfloat*)&trafo);

  for (unsigned int n = 0; n < node->mNumMeshes; ++n) {
    const aiMesh *mesh = scene->mMeshes[node->mMeshes[n]];
    const aiMaterial *mtl = scene->mMaterials[mesh->mMaterialIndex];

    mesh->HasNormals() ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);

    //if (0 != mtl->GetTextureCount(aiTextureType_DIFFUSE)) {
    //  glEnable(GL_TEXTURE_2D);
    //  glBindTexture(GL_TEXTURE_2D, textures[mesh->mMaterialIndex]);
    //} else {
    //  glDisable(GL_TEXTURE_2D);
    //}

    //apply_material(mtl);

    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> texcoords;
    //if (mesh->mNumBones == 0) {
      for (size_t i = 0; i < mesh->mNumVertices; ++i) {
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);
        normals.push_back(mesh->mNormals[i].x);
        normals.push_back(mesh->mNormals[i].y);
        normals.push_back(mesh->mNormals[i].z);
        texcoords.push_back(mesh->mTextureCoords[0][i].x);
        texcoords.push_back(mesh->mTextureCoords[0][i].y);
      }
    //}

    std::vector<GLushort> indices;
    for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
      const aiFace *face = &mesh->mFaces[t];
      for (unsigned int i = 0; i < face->mNumIndices; ++i) {
        unsigned short index = face->mIndices[i];
        indices.push_back(index);
      }
    }


    //for (size_t b = 0; b < mesh->mNumBones; ++b) {
    //  const aiBone *abone = mesh->mBones[b];
    //  struct bone bone = bones[abone->mName.data];

    //  //aiMatrix4x4 posTrafo = bone.offset * bone.begin;
    //  //posTrafo.Transpose();

    //  //aiMatrix3x3 nrmTrafo = aiMatrix3x3(posTrafo);

    //  for (size_t w = 0; w < abone->mNumWeights; ++w) {
    //    const aiVertexWeight& weight = abone->mWeights[w];
    //    size_t vertexId = weight.mVertexId;

    //    //aiVector3D& srcPos = mesh->mVertices[vertexId];
    //    //aiVector3D& srcNrm = mesh->mNormals[vertexId];

    //    //resultPos[vertexId] += (weight.mWeight * (posTrafo * srcPos));
    //    //resultNrm[vertexId] += weight.mWeight * (nrmTrafo * srcNrm);
    //  }
    //}

    VBO *vbo = new VBO();
    if (0 != mtl->GetTextureCount(aiTextureType_DIFFUSE)) {
      vbo->set_texture(textures[mesh->mMaterialIndex]);
    }

    vbo->bind_indices(indices);
    vbo->bind_data(VBO::TEXCOORD, texcoords);
    vbo->bind_data(VBO::VERTEX,   vertices, GL_DYNAMIC_COPY);
    vbo->bind_data(VBO::NORMAL,   normals,  GL_DYNAMIC_COPY);

    vertex_buffers.push_back(vbo);

    //for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
    //  const aiFace *face = &mesh->mFaces[t];

    //  glBegin(GL_TRIANGLES);
    //  for (unsigned int i = 0; i < face->mNumIndices; ++i) {
    //    int index = face->mIndices[i];
    //    //if (mesh->HasTextureCoords(0))
    //    //  glTexCoord3fv((GLfloat*)&mesh->mTextureCoords[0][index]);
    //    //glNormal3fv((GLfloat*)&resultNrm[index]);
    //    //glVertex3fv((GLfloat*)&resultPos[index]);
    //  }
    //  glEnd();
    //}
  }

  for (unsigned int c = 0; c < node->mNumChildren; ++c) {
    rrender(node->mChildren[c]);
  }

  //glPopMatrix();
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
  trafo.Transpose();
  trafo *= tp;

  if (bones.end() != bit) {
    bit->second.end   = tp;
    bit->second.begin = trafo;
  }

  for (unsigned int c = 0; c < node->mNumChildren; ++c) {
    animate(node->mChildren[c], animtime, trafo);
  }
}

void
Model::normalize()
{
  GLfloat tmp = scene_max.x - scene_min.x;
  tmp = aisgl_max(scene_max.y - scene_min.y, tmp);
  tmp = aisgl_max(scene_max.z - scene_min.z, tmp);
  tmp = 1.f / tmp;
  glScalef(tmp, tmp, tmp);
}

void
Model::draw(float elapsed, float opacity, bool bones)
{
  glEnable(GL_LIGHTING);

  if (0 < scene->mNumAnimations) {
    const aiAnimation *anim = scene->mAnimations[0];
    float animtime = fmod(elapsed * anim->mTicksPerSecond, anim->mDuration);
    this->animate(scene->mRootNode, animtime, aiMatrix4x4());
  }

  shader_program->use();
  for (VBO *vbo : vertex_buffers) {
    vbo->draw();
  }
  shader_program->disuse();

  if (bones)
    this->rrenderbones(scene->mRootNode);
}

void
Model::rrenderbones(const aiNode *node)
{
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);

  glColor3f(1.0, 0.0, 0.0);
  for (std::pair<std::string, struct bone> p : bones) {

    glPushMatrix();
    aiMatrix4x4 m = p.second.begin;

    glMultMatrixf((GLfloat*)&m);
    glBegin(GL_POINTS);
    glVertex3f(0, 0, 0);
    glEnd();

    glPopMatrix();
  }

  glBegin(GL_LINES);
  glColor3f(1.0, 1.0, 1.0);
  for (std::pair<std::string, struct bone> p : bones) {
    glPushMatrix();
    aiMatrix4x4 begin = p.second.begin;
    aiMatrix4x4 end   = p.second.end;
    glVertex3f(begin.d1, begin.d2, begin.d3);
    glVertex3f(end.d1, end.d2, end.d3);
    glPopMatrix();
  }
  glEnd();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
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
    shader_program = new gl::ShaderProgram();
    gl::Shader *vertex_shader   = new gl::Shader(GL_VERTEX_SHADER, "resources/shaders/model.v.glsl");
    gl::Shader *fragment_shader = new gl::Shader(GL_FRAGMENT_SHADER, "resources/shaders/model.f.glsl");
    shader_program->attachShader(vertex_shader);
    shader_program->attachShader(fragment_shader);
    shader_program->link();
  }

  auto it = loaded_models.find(file);
  if (loaded_models.end() != it) {
    return it->second;
  }

  Model *m = new Model(file);
  loaded_models.insert(std::make_pair(file, m));
  return m;
}
