#pragma once
#include "Audio/SoundEvent.h"
#include "Component/Component.h"
#include <string>
#include <vector>

namespace Knuckles {
class AudioComponent : public Component {
public:
  AudioComponent(class Object *owner, int updateOrder = 200);
  ~AudioComponent();

  void Update(float deltaTime) override;
  void OnUpdateWorldTransform() override;

  SoundEvent PlayEvent(const std::string &name);
  void StopAllEvents();

  TypeID GetType() const override { return TAudioComponent; }

private:
  std::vector<SoundEvent> mEvents2D;
  std::vector<SoundEvent> mEvents3D;
};
} // namespace Knuckles
