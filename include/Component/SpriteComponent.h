#pragma once
#include "Component/Component.h"

namespace Knuckles {
class SpriteComponent : public Component {
public:
  // (Lower draw order corresponds with further back)
  SpriteComponent(class Object *owner, int drawOrder = 100);
  ~SpriteComponent();

  virtual void Draw(class Shader *shader);
  virtual void SetTexture(class Texture *texture);

  int GetDrawOrder() const { return mDrawOrder; }
  int GetTexHeight() const { return mTexHeight; }
  int GetTexWidth() const { return mTexWidth; }

  void SetVisible(bool visible) { mVisible = visible; }
  bool GetVisible() const { return mVisible; }

  TypeID GetType() const override { return TSpriteComponent; }

  void LoadProperties(const rapidjson::Value &inObj) override;
  void SaveProperties(rapidjson::Document::AllocatorType &alloc,
                      rapidjson::Value &inObj) const override;

protected:
  class Texture *mTexture;
  int mDrawOrder;
  int mTexWidth;
  int mTexHeight;
  bool mVisible;
};
} // namespace Knuckles
