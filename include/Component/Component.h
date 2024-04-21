#pragma once
#include "Math.h"
#include <rapidjson/document.h>

namespace Knuckles {
class Component {
public:
  enum TypeID {
    TComponent = 0,
    TAudioComponent,
    TBallMove,
    TBoxComponent,
    TCameraComponent,
    TFollowCamera,
    TMeshComponent,
    TMoveComponent,
    TSkeletalMeshComponent,
    TSpriteComponent,
    TMirrorCamera,
    TPointLightComponent,
    TTargetComponent,

    NUM_COMPONENT_TYPES
  };

  static const char *TypeNames[NUM_COMPONENT_TYPES];

  // Constructor
  // (the lower the update order, the earlier the component updates)
  Component(class Object *owner, int updateOrder = 100);
  // Destructor
  virtual ~Component();
  // Update this component by delta time
  virtual void Update(float deltaTime);
  // Process input for this component
  virtual void ProcessInput(const struct InputState &state) {}
  // Called when world transform changes
  virtual void OnUpdateWorldTransform();

  class Object *GetOwner() { return mOwner; }
  int GetUpdateOrder() const { return mUpdateOrder; }

  virtual TypeID GetType() const = 0;

  // Load/Save
  virtual void LoadProperties(const rapidjson::Value &inObj);
  virtual void SaveProperties(rapidjson::Document::AllocatorType &alloc,
                              rapidjson::Value &inObj) const;

  // Create a component with specified properties
  template <typename T>
  static Component *Create(class Object *Object,
                           const rapidjson::Value &inObj) {
    // Dynamically allocate component of type T
    T *t = new T(Object);
    // Call LoadProperties on new component
    t->LoadProperties(inObj);
    return t;
  }

protected:
  // Owning Object
  class Object *mOwner;
  // Update order of component
  int mUpdateOrder;
};
} // namespace Knuckles
