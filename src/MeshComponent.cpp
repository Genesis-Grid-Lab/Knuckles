#include "Component/MeshComponent.h"
#include "Core/Application.h"
#include "LevelLoader.h"
#include "Object/Object.h"
#include "Renderer/Mesh.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/VertexArray.h"

namespace Knuckles
{
MeshComponent::MeshComponent(Object *owner, bool isSkeletal)
    : Component(owner), mMesh(nullptr), mTextureIndex(0), mVisible(true),
      mIsSkeletal(isSkeletal) {
  mOwner->GetApplication()->GetRenderer()->AddMeshComp(this);
}

MeshComponent::~MeshComponent() {
  mOwner->GetApplication()->GetRenderer()->RemoveMeshComp(this);
}

void MeshComponent::Draw(Shader *shader) {
  if (mMesh) {
    // Set the world transform
    shader->SetMatrixUniform("uWorldTransform", mOwner->GetWorldTransform());
    // Set specular power
    shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());
    // Set the active texture
    mT = mMesh->GetTexture(mTextureIndex);
    if (mT) {
      mT->SetActive();
    }
    // Set the mesh's vertex array as active
    VertexArray *va = mMesh->GetVertexArray();
    va->SetActive();
    // Draw
    glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
  }
}

void MeshComponent::LoadProperties(const rapidjson::Value &inObj) {
  Component::LoadProperties(inObj);

  std::string meshFile;
  if (JsonHelper::GetString(inObj, "meshFile", meshFile)) {
    SetMesh(mOwner->GetApplication()->GetRenderer()->GetMesh(meshFile));
  }

  int idx;
  if (JsonHelper::GetInt(inObj, "textureIndex", idx)) {
    mTextureIndex = static_cast<size_t>(idx);
  }

  JsonHelper::GetBool(inObj, "visible", mVisible);
  JsonHelper::GetBool(inObj, "isSkeletal", mIsSkeletal);
}

void MeshComponent::SaveProperties(rapidjson::Document::AllocatorType &alloc,
                                   rapidjson::Value &inObj) const {
  Component::SaveProperties(alloc, inObj);

  if (mMesh) {
    JsonHelper::AddString(alloc, inObj, "meshFile", mMesh->GetFileName());
  }
  JsonHelper::AddInt(alloc, inObj, "textureIndex",
                     static_cast<int>(mTextureIndex));
  JsonHelper::AddBool(alloc, inObj, "visible", mVisible);
  JsonHelper::AddBool(alloc, inObj, "isSkeletal", mIsSkeletal);
}
}
