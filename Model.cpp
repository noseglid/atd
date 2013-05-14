#include "Model.h"
#include "Debug.h"
#include "ModelException.h"
#include "ImageLoader.h"

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

std::map<std::string, Model*> Model::loaded_models;

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

Model::Model(std::string file)
{
  importer.ReadFile(file, aiProcessPreset_TargetRealtime_Quality);
  scene = importer.GetScene();
  if (NULL == scene) {
    throw ModelException("Could not load file: " + file);
  }

  get_bounding_box(scene_min, scene_max);
  scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
  scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
  scene_center.z = (scene_min.z + scene_max.z) / 2.0f;

  load_nodes(scene, scene->mRootNode);

  size_t num_vertices = 0;
  for (size_t i = 0; i < scene->mNumMeshes; ++i) {
    num_vertices += scene->mMeshes[i]->mNumVertices;
  }

  DBG("Loaded '" << file << "', " << num_vertices << " vertices.");
}

Model::~Model()
{
  delete scene;
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
    std::cerr << "Warning: could not load texture from: " << path.data << std::endl;
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
mtlpart(const aiMaterial *mtl, GLenum gltype, const char *pKey, unsigned int type, unsigned int idx)
{
  aiColor4D clr;
  if (aiReturn_SUCCESS != mtl->Get(pKey, type, idx, clr)) {
    return;
  }

  GLfloat params[4] = { clr.r, clr.g, clr.b, clr.a };
  glMaterialfv(GL_FRONT, gltype, params);
}

void
Model::apply_material(const aiMaterial *mtl)
{
  mtlpart(mtl, GL_DIFFUSE,  AI_MATKEY_COLOR_DIFFUSE);
  mtlpart(mtl, GL_SPECULAR, AI_MATKEY_COLOR_SPECULAR);
  mtlpart(mtl, GL_AMBIENT,  AI_MATKEY_COLOR_AMBIENT);
  mtlpart(mtl, GL_EMISSION, AI_MATKEY_COLOR_EMISSIVE);

  GLfloat shininess = 64.0;
  mtl->Get(AI_MATKEY_SHININESS, shininess);
  glMaterialf(GL_FRONT, GL_SHININESS, shininess);
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
  glPushMatrix();

  aiMatrix4x4 trafo = node->mTransformation;
  glMultTransposeMatrixf((GLfloat*)&trafo);

  for (unsigned int n = 0; n < node->mNumMeshes; ++n) {
    const aiMesh *mesh = scene->mMeshes[node->mMeshes[n]];
    const aiMaterial *mtl = scene->mMaterials[mesh->mMaterialIndex];

    mesh->HasNormals() ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);

    if (0 != mtl->GetTextureCount(aiTextureType_DIFFUSE)) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, textures[mesh->mMaterialIndex]);
    } else {
      glDisable(GL_TEXTURE_2D);
    }

    apply_material(mtl);

    std::vector<aiVector3D> resultPos(mesh->mNumVertices);
    std::vector<aiVector3D> resultNrm(mesh->mNumVertices);
    if (mesh->mNumBones == 0) {
      for (size_t i = 0; i < mesh->mNumVertices; ++i) {
        resultPos[i] = mesh->mVertices[i];
        resultNrm[i] = mesh->mNormals[i];
      }
    }

    for (size_t b = 0; b < mesh->mNumBones; ++b) {
      const aiBone *abone = mesh->mBones[b];
      struct bone bone = bones[abone->mName.data];

      aiMatrix4x4 posTrafo = bone.offset * bone.begin;
      posTrafo.Transpose();

      aiMatrix3x3 nrmTrafo = aiMatrix3x3(posTrafo);

      for (size_t w = 0; w < abone->mNumWeights; ++w) {
        const aiVertexWeight& weight = abone->mWeights[w];
        size_t vertexId = weight.mVertexId;

        aiVector3D& srcPos = mesh->mVertices[vertexId];
        aiVector3D& srcNrm = mesh->mNormals[vertexId];

        resultPos[vertexId] += (weight.mWeight * (posTrafo * srcPos));
        resultNrm[vertexId] += weight.mWeight * (nrmTrafo * srcNrm);
      }
    }

    for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
      const aiFace *face = &mesh->mFaces[t];

      glBegin(GL_TRIANGLES);
      for (unsigned int i = 0; i < face->mNumIndices; ++i) {
        int index = face->mIndices[i];
        if (mesh->HasTextureCoords(0))
          glTexCoord3fv((GLfloat*)&mesh->mTextureCoords[0][index]);
        glNormal3fv((GLfloat*)&resultNrm[index]);
        glVertex3fv((GLfloat*)&resultPos[index]);
      }
      glEnd();
    }
  }

  for (unsigned int c = 0; c < node->mNumChildren; ++c) {
    rrender(node->mChildren[c]);
  }

  glPopMatrix();
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
Model::draw(float elapsed)
{
  if (0 < scene->mNumAnimations) {
    const aiAnimation *anim = scene->mAnimations[0];
    float animtime = fmod(elapsed * anim->mTicksPerSecond, anim->mDuration);
    this->animate(scene->mRootNode, animtime, aiMatrix4x4());
  }

  this->rrender(scene->mRootNode);
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

  glColor3f(0.5, 0.5, 0.5);
}

void
Model::drawBones()
{
  rrenderbones(scene->mRootNode);
}

Model *
Model::load(std::string file)
{
  DBG("Loading model: " << file);
  file = std::string("models/") + file;
  auto it = loaded_models.find(file);
  if (loaded_models.end() != it) {
    DBG("Using cached at: " << it->second);
    return it->second;
  }

  Model *m = new Model(file);
  loaded_models.insert(std::make_pair(file, m));
  return m;
}
