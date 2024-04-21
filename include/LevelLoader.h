#pragma once
#include "Math.h"
#include <functional>
#include <rapidjson/document.h>
#include <string>
#include <unordered_map>

namespace Knuckles {
using ObjectFunc = std::function<class Object *(class Application *, std::string title,
                                                const rapidjson::Value &)>;
using ComponentFunc =
    std::function<class Component *(class Object *, const rapidjson::Value &)>;

using MusicFunc = std::function<class SoundEvent*(const rapidjson::Value &)>;

class LevelLoader {
public:
  // Load the level -- returns true if successful
  static bool LoadLevel(class Application *game, const std::string &fileName);
  // Loads a JSON file into a RapidJSON document
  static bool LoadJSON(const std::string &fileName,
                       rapidjson::Document &outDoc);
  // Save the level
  static void SaveLevel(class Application *game, const std::string &fileName);

protected:
  // Helper to load global properties
  static void LoadGlobalProperties(class Application *game,
                                   const rapidjson::Value &inObject);
  // Helper to load in Objects
  static void LoadObjects(class Application *game,
                          const rapidjson::Value &inArray);
  // Helper to load in components
  static void LoadComponents(class Object *Object,
                             const rapidjson::Value &inArray);

  static void LoadMusic(class SoundEvent *Sound, const rapidjson::Value &inArray);

  // Maps for data
  static std::unordered_map<std::string, ObjectFunc> sObjectFactoryMap;
  static std::unordered_map<std::string, std::pair<int, ComponentFunc>>
      sComponentFactoryMap;
  // Helper to save global properties
  static void SaveGlobalProperties(rapidjson::Document::AllocatorType &alloc,
                                   class Application *game,
                                   rapidjson::Value &inObject);
  // Helper to save actors
  static void SaveObjects(rapidjson::Document::AllocatorType &alloc,
                          class Application *game, rapidjson::Value &inArray);
  // Helper to save components
  static void SaveComponents(rapidjson::Document::AllocatorType &alloc,
                             const class Object *Object,
                             rapidjson::Value &inArray);

  static void SaveMusic(rapidjson::Document::AllocatorType &alloc, rapidjson::Value &inArray);

};

class JsonHelper {
public:
  // Helpers - Return true if successful, and also sets out parameter to parsed
  // value For each function, the first parameter is the containing JSON object,
  // the second is the name of the property in the containing object, and the
  // third is the value you acquire. Furthermore, if the property is not found,
  // each function is guaranteed not to modify the return value.
  static bool GetInt(const rapidjson::Value &inObject, const char *inProperty,
                     int &outInt);
  static bool GetFloat(const rapidjson::Value &inObject, const char *inProperty,
                       float &outFloat);
  static bool GetString(const rapidjson::Value &inObject,
                        const char *inProperty, std::string &outStr);
  static bool GetBool(const rapidjson::Value &inObject, const char *inProperty,
                      bool &outBool);
  static bool GetVector3(const rapidjson::Value &inObject,
                         const char *inProperty, Vector3 &outVector);
  static bool GetQuaternion(const rapidjson::Value &inObject,
                            const char *inProperty, Quaternion &outQuat);

  // Setter functions
  static void AddInt(rapidjson::Document::AllocatorType &alloc,
                     rapidjson::Value &inObject, const char *name, int value);
  static void AddFloat(rapidjson::Document::AllocatorType &alloc,
                       rapidjson::Value &inObject, const char *name,
                       float value);
  static void AddString(rapidjson::Document::AllocatorType &alloc,
                        rapidjson::Value &inObject, const char *name,
                        const std::string &value);
  static void AddBool(rapidjson::Document::AllocatorType &alloc,
                      rapidjson::Value &inObject, const char *name, bool value);
  static void AddVector3(rapidjson::Document::AllocatorType &alloc,
                         rapidjson::Value &inObject, const char *name,
                         const Vector3 &value);
  static void AddQuaternion(rapidjson::Document::AllocatorType &alloc,
                            rapidjson::Value &inObject, const char *name,
                            const Quaternion &value);
};
} // namespace Knuckles
