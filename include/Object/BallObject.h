#pragma once
#include "Object/Object.h"
#include <string>

namespace Knuckles {
class BallObject : public Object {
public:
  BallObject(class Application *game, std::string title);

  void UpdateObject(float deltaTime) override;

  void HitTarget();

  void LoadProperties(const rapidjson::Value &inObj) override;
  void SaveProperties(rapidjson::Document::AllocatorType &alloc,
                      rapidjson::Value &inObj) const override;

  TypeID GetType() const override { return TBallObject; }

  Object *Clone(std::string title) override { return new BallObject(mApp, title);}

private:
  class Application *mApp;
  std::string mTitle;
  class AudioComponent *mAudioComp;
  float mLifeSpan;
};
} // namespace Knuckles
