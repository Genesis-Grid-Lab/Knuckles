#pragma once
#include "Collision/Collision.h"
#include "Component/Component.h"

namespace Knuckles {
class BoxComponent : public Component {
public:
  BoxComponent(class Object *owner, int updateOrder = 100);
  ~BoxComponent();

  void OnUpdateWorldTransform() override;

  void SetObjectBox(const AABB &model) { mObjectBox = model; }
  const AABB &GetWorldBox() const { return mWorldBox; }

  TypeID GetType() const override { return TBoxComponent; }

  void LoadProperties(const rapidjson::Value &inObj) override;
  void SaveProperties(rapidjson::Document::AllocatorType &alloc,
                      rapidjson::Value &inObj) const override;
  void SetShouldRotate(bool value) { mShouldRotate = value; }

private:
  AABB mObjectBox;
  AABB mWorldBox;
  bool mShouldRotate;
};
} // namespace Knuckles
