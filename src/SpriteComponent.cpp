#include "Component/SpriteComponent.h"
#include "Core/Application.h"
#include "LevelLoader.h"
#include "Object/Object.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"

namespace Knuckles
{
SpriteComponent::SpriteComponent(Object *owner, int drawOrder)
    : Component(owner), mTexture(nullptr), mDrawOrder(drawOrder), mTexWidth(0),
      mTexHeight(0), mVisible(true) {
  mOwner->GetApplication()->GetRenderer()->AddSprite(this);
}

SpriteComponent::~SpriteComponent() {
  mOwner->GetApplication()->GetRenderer()->RemoveSprite(this);
}

void SpriteComponent::Draw(Shader *shader) {
  if (mTexture) {
    // Scale the quad by the width/height of texture
    Matrix4 scaleMat = Matrix4::CreateScale(
        static_cast<float>(mTexWidth), static_cast<float>(mTexHeight), 1.0f);

    Matrix4 world = scaleMat * mOwner->GetWorldTransform();

    // Since all sprites use the same shader/vertices,
    // the game first sets them active before any sprite draws

    // Set world transform
    shader->SetMatrixUniform("uWorldTransform", world);
    // Set current texture
    mTexture->SetActive();
    // Draw quad
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
  }
}

void SpriteComponent::SetTexture(Texture *texture) {
  mTexture = texture;
  // Set width/height
  mTexWidth = texture->GetWidth();
  mTexHeight = texture->GetHeight();
}

void SpriteComponent::LoadProperties(const rapidjson::Value &inObj) {
  Component::LoadProperties(inObj);

  std::string texFile;
  if (JsonHelper::GetString(inObj, "textureFile", texFile)) {
    SetTexture(mOwner->GetApplication()->GetRenderer()->GetTexture(texFile));
  }

  JsonHelper::GetInt(inObj, "drawOrder", mDrawOrder);
  JsonHelper::GetBool(inObj, "visible", mVisible);
}

void SpriteComponent::SaveProperties(rapidjson::Document::AllocatorType &alloc,
                                     rapidjson::Value &inObj) const {
  Component::SaveProperties(alloc, inObj);

  if (mTexture) {
    JsonHelper::AddString(alloc, inObj, "textureFile", mTexture->GetFileName());
  }

  JsonHelper::AddInt(alloc, inObj, "drawOrder", mDrawOrder);
  JsonHelper::AddBool(alloc, inObj, "visible", mVisible);
}
}