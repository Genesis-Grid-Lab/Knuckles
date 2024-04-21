#pragma once
#include "Component/Component.h"

namespace Knuckles {
class MoveComponent : public Component {
public:
  // Lower update order to update first
  MoveComponent(class Object *owner, int updateOrder = 10);
  void Update(float deltaTime) override;

  float GetAngularSpeed() const { return mAngularSpeed; }
  float GetForwardSpeed() const { return mForwardSpeed; }
  float GetStrafeSpeed() const { return mStrafeSpeed; }
  float GetUpSpeed() const { return mUpSpeed;}
  void SetAngularSpeed(float speed) { mAngularSpeed = speed; }
  void SetForwardSpeed(float speed) { mForwardSpeed = speed; }
  void SetStrafeSpeed(float speed) { mStrafeSpeed = speed; }
  void SetUpSpeed(float speed) { mUpSpeed = speed;}

  TypeID GetType() const override { return TMoveComponent; }

  void LoadProperties(const rapidjson::Value &inObj) override;
  void SaveProperties(rapidjson::Document::AllocatorType &alloc,
                      rapidjson::Value &inObj) const override;

protected:
  float mAngularSpeed;
  float mForwardSpeed;
  float mStrafeSpeed;
  float mUpSpeed;
};
} // namespace Knuckles
