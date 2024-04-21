#pragma once
#include "Component/Component.h"

namespace Knuckles {
class MeshComponent : public Component {
public:
  MeshComponent(class Object *owner, bool isSkeletal = false);
  ~MeshComponent();
  // Draw this mesh component
  virtual void Draw(class Shader *shader);
  // Set the mesh/texture index used by mesh component
  virtual void SetMesh(class Mesh *mesh) { mMesh = mesh; }
  void SetTextureIndex(size_t index) { mTextureIndex = index; }

  void SetVisible(bool visible) { mVisible = visible; }
  bool GetVisible() const { return mVisible; }

  bool GetIsSkeletal() const { return mIsSkeletal; }

  TypeID GetType() const override { return TMeshComponent; }

  void LoadProperties(const rapidjson::Value &inObj) override;
  void SaveProperties(rapidjson::Document::AllocatorType &alloc,
                      rapidjson::Value &inObj) const override;

  class Texture *GetTex() { return mT;}

protected:
  class Mesh *mMesh;
  size_t mTextureIndex;
  class Texture *mT;
  bool mVisible;
  bool mIsSkeletal;
};
} // namespace Knuckles
