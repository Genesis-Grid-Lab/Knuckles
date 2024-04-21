#include "Component/PointLightComponent.h"
#include "Core/Application.h"
#include "LevelLoader.h"
#include "Object/Object.h"
#include "Renderer/Mesh.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"

namespace Knuckles
{
PointLightComponent::PointLightComponent(Object *owner) : Component(owner) {
  owner->GetApplication()->GetRenderer()->AddPointLight(this);
}

PointLightComponent::~PointLightComponent() {
  mOwner->GetApplication()->GetRenderer()->RemovePointLight(this);
}

void PointLightComponent::Draw(Shader *shader, Mesh *mesh) {
  // We assume, coming into this function, that the shader is active
  // and the sphere mesh is active

  // World transform is scaled to the outer radius (divided by the mesh radius)
  // and positioned to the world position
  Matrix4 scale = Matrix4::CreateScale(mOwner->GetScale() * mOuterRadius /
                                       mesh->GetRadius());
  Matrix4 trans = Matrix4::CreateTranslation(mOwner->GetPosition());
  Matrix4 worldTransform = scale * trans;
  shader->SetMatrixUniform("uWorldTransform", worldTransform);
  // Set point light shader constants
  shader->SetVectorUniform("uPointLight.mWorldPos", mOwner->GetPosition());
  shader->SetVectorUniform("uPointLight.mDiffuseColor", mDiffuseColor);
  shader->SetFloatUniform("uPointLight.mInnerRadius", mInnerRadius);
  shader->SetFloatUniform("uPointLight.mOuterRadius", mOuterRadius);

  // Draw the sphere
  glDrawElements(GL_TRIANGLES, mesh->GetVertexArray()->GetNumIndices(),
                 GL_UNSIGNED_INT, nullptr);
}

void PointLightComponent::LoadProperties(const rapidjson::Value &inObj) {
  Component::LoadProperties(inObj);
  JsonHelper::GetVector3(inObj, "color", mDiffuseColor);
  JsonHelper::GetFloat(inObj, "innerRadius", mInnerRadius);
  JsonHelper::GetFloat(inObj, "outerRadius", mOuterRadius);
}

void PointLightComponent::SaveProperties(
    rapidjson::Document::AllocatorType &alloc, rapidjson::Value &inObj) const {
  JsonHelper::AddVector3(alloc, inObj, "color", mDiffuseColor);
  JsonHelper::AddFloat(alloc, inObj, "innerRadius", mInnerRadius);
  JsonHelper::AddFloat(alloc, inObj, "outerRadius", mOuterRadius);
}
}