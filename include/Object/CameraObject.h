#pragma once
#include "Object/Object.h"
#include <string>

namespace Knuckles {
class CameraObject : public Object {
public:
  CameraObject(class Application *game, std::string title);

  void ObjectInput(const struct InputState &state) override;

  void SetVisible(bool visible);

  void LoadProperties(const rapidjson::Value &inObj) override;
  void SaveProperties(rapidjson::Document::AllocatorType &alloc,
                      rapidjson::Value &inObj) const override;

  TypeID GetType() const override { return TFollowObject; }

  Object *Clone(std::string title) override {
    return new CameraObject(mApp, title);
  }

  class MeshComponent *GetMeshComp() override { return mMc; }

private:
  class Application *mApp;
  std::string mTitle;
  class MeshComponent *mMc;

  class MoveComponent *mMoveComp;
  class FollowCamera *mCameraComp;
  class SkeletalMeshComponent *mMeshComp;
  bool mMoving;
};
} // namespace Knuckles
