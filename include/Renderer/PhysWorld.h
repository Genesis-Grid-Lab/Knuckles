#pragma once
#include "Collision/Collision.h"
#include "Math.h"
#include <functional>
#include <vector>

namespace Knuckles {
class PhysWorld {
public:
  PhysWorld(class Application *game);

  // Used to give helpful information about collision results
  struct CollisionInfo {
    // Point of collision
    Vector3 mPoint;
    // Normal at collision
    Vector3 mNormal;
    // Component collided with
    class BoxComponent *mBox;
    // Owning Object of component
    class Object *mObject;
  };

  // Test a line segment against boxes
  // Returns true if it collides against a box
  bool SegmentCast(const LineSegment &l, CollisionInfo &outColl);

  // Tests collisions using naive pairwise
  void TestPairwise(std::function<void(class Object *, class Object *)> f);
  // Test collisions using sweep and prune
  void TestSweepAndPrune(std::function<void(class Object *, class Object *)> f);

  // Add/remove box components from world
  void AddBox(class BoxComponent *box);
  void RemoveBox(class BoxComponent *box);

private:
  class Application *mApplication;
  std::vector<class BoxComponent *> mBoxes;
};
} // namespace Knuckles
