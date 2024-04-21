#pragma once
#include "Component/Component.h"
#include "Math.h"
#include <cstdint>
#include <cstdlib>
#include <random>
#include <rapidjson/document.h>
#include <vector>

namespace Knuckles {
class Mesh;
class Object {
public:
  enum TypeID {
    TObject = 0,
    TBallObject,
    TFollowObject,
    TPlaneObject,
    TTargetObject,
    TCameraObject,

    NUM_OBJECT_TYPES
  };

  static const char *TypeNames[NUM_OBJECT_TYPES];

  enum State { EActive, EPaused, EDead };

  Object(class Application *game, std::string title);
  virtual ~Object();

  void Delete() { delete this; }

  // Update function called from Application (not overridable)
  void Update(float deltaTime);
  // Updates all the components attached to the Object (not overridable)
  void UpdateComponents(float deltaTime);
  // Any Object-specific update code (overridable)
  virtual void UpdateObject(float deltaTime);
  // ProcessInput function called from Application (not overridable)
  void ProcessInput(const struct InputState &state);
  // Any Object-specific input code (overridable)
  virtual void ObjectInput(const struct InputState &state);
  virtual Object *Clone(std::string title) {
    return new Object(mApplication, title);
  }

  // Getters/setters
  const Vector3 &GetPosition() const { return mPosition; }
  void SetPosition(const Vector3 &pos) {
    mPosition = pos;
    mRecomputeTransform = true;
  }
  float GetScale() const { return mScale; }
  void SetScale(float scale) {
    mScale = scale;
    mRecomputeTransform = true;
  }
  const Quaternion &GetRotation() const { return mRotation; }
  void SetRotation(const Quaternion &rotation) {
    mRotation = rotation;
    mRecomputeTransform = true;
  }

  void ComputeWorldTransform();
  const Matrix4 &GetWorldTransform() const { return mWorldTransform; }

  Vector3 GetForward() const {
    return Vector3::Transform(Vector3::UnitX, mRotation);
  }
  Vector3 GetRight() const {
    return Vector3::Transform(Vector3::UnitY, mRotation);
  }

  Vector3 GetUp() const {
    return Vector3::Transform(Vector3::UnitZ, mRotation);
  }

  void RotateToNewForward(const Vector3 &forward);

  State GetState() const { return mState; }
  void SetState(State state) { mState = state; }

  class Application *GetApplication() { return mApplication; }

  // Add/remove components
  void AddComponent(class Component *component);
  void RemoveComponent(class Component *component);

  // Load/Save
  virtual void LoadProperties(const rapidjson::Value &inObj);
  virtual void SaveProperties(rapidjson::Document::AllocatorType &alloc,
                              rapidjson::Value &inObj) const;

  // Create an Object with specified properties
  template <typename T>
  static Object *Create(class Application *game, std::string title,
                        const rapidjson::Value &inObj) {
    // Dynamically allocate Object of type T
    T *t = new T(game, title);
    // Call LoadProperties on new Object
    t->LoadProperties(inObj);
    return t;
  }

  // Search through component vector for one of type
  Component *GetComponentOfType(Component::TypeID type) {
    Component *comp = nullptr;
    for (Component *c : mComponents) {
      if (c->GetType() == type) {
        comp = c;
        break;
      }
    }
    return comp;
  }

  virtual TypeID GetType() const { return TObject; }

  const std::vector<Component *> &GetComponents() const { return mComponents; }

  std::string &GetTitle() { return mTitle; }
  void SetTitle(std::string title) { mTitle = title; }
  int GetID() { return mID; }

  virtual class Mesh *GetMesh() { return mMesh; }
  virtual class MeshComponent *GetMeshComp() { return mc; }
  virtual void SetMesh(class Mesh *mesh) { mMesh = mesh; }
  virtual void SetMeshComp(class MeshComponent *mc) { mc = mc; }

private:
  // Object's state
  State mState;

  std::string mTitle;
  int mID = rand() % 1000000;

  // Transform
  Matrix4 mWorldTransform;
  Vector3 mPosition;
  Quaternion mRotation;
  float mScale;
  bool mRecomputeTransform;

  std::vector<Component *> mComponents;
  class Application *mApplication;
  class Mesh *mMesh;
  class MeshComponent *mc;
};
} // namespace Knuckles
