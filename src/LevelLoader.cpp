#include "LevelLoader.h"
#include "Camera/FollowCamera.h"
#include "Camera/MirrorCamera.h"
#include "Component/AudioComponent.h"
#include "Component/BoxComponent.h"
#include "Component/CameraComponent.h"
#include "Component/Component.h"
#include "Component/MeshComponent.h"
#include "Component/MoveComponent.h"
#include "Component/PointLightComponent.h"
#include "Component/SkeletalMeshComponent.h"
#include "Component/SpriteComponent.h"
#include "Component/TargetComponent.h"
#include "Core/Application.h"
#include "Object/CameraObject.h"
#include "Object/Object.h"
#include "Object/BallObject.h"
#include "Object/BallMove.h"
#include "Object/FollowObject.h"
#include "Object/PlaneObject.h"
#include "Object/TargetObject.h"
#include "Renderer/Renderer.h"
#include "Log/Log.h"
#include <fstream>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <vector>

namespace Knuckles
{

const int LevelVersion = 1;

// Declare map of Objects to spawn functions
std::unordered_map<std::string, ObjectFunc> LevelLoader::sObjectFactoryMap{
    {"Object", &Object::Create<Object>},
    {"BallObject", &Object::Create<BallObject>},
    {"FollowObject", &Object::Create<FollowObject>},
    {"PlaneObject", &Object::Create<PlaneObject>},
    {"TargetObject", &Object::Create<TargetObject>},
    {"CameraObject", &Object::Create<CameraObject>},
};

std::unordered_map<std::string, std::pair<int, ComponentFunc>>
    LevelLoader::sComponentFactoryMap{
        {"AudioComponent",
         {Component::TAudioComponent, &Component::Create<AudioComponent>}},
        {"BallMove", {Component::TBallMove, &Component::Create<BallMove>}},
        {"BoxComponent",
         {Component::TBoxComponent, &Component::Create<BoxComponent>}},
        {"CameraComponent",
         {Component::TCameraComponent, &Component::Create<CameraComponent>}},
        {"FollowCamera",
         {Component::TFollowCamera, &Component::Create<FollowCamera>}},
        {"MeshComponent",
         {Component::TMeshComponent, &Component::Create<MeshComponent>}},
        {"MoveComponent",
         {Component::TMoveComponent, &Component::Create<MoveComponent>}},
        {"SkeletalMeshComponent",
         {Component::TSkeletalMeshComponent,
          &Component::Create<SkeletalMeshComponent>}},
        {"SpriteComponent",
         {Component::TSpriteComponent, &Component::Create<SpriteComponent>}},
        {"MirrorCamera",
         {Component::TMirrorCamera, &Component::Create<MirrorCamera>}},
        {"PointLightComponent",
         {Component::TPointLightComponent,
          &Component::Create<PointLightComponent>}},
        {"TargetComponent",
         {Component::TTargetComponent, &Component::Create<TargetComponent>}},
    };

bool LevelLoader::LoadLevel(Application *game, const std::string &fileName) {
  rapidjson::Document doc;
  if (!LoadJSON(fileName, doc)) {
    KE_CORE_ERROR("Failed to load level {0}", fileName.c_str());
    return false;
  }

  int version = 0;
  if (!JsonHelper::GetInt(doc, "version", version) || version != LevelVersion) {
    KE_CORE_ERROR("Incorrect level file version for {0}", fileName.c_str());
    return false;
  }

  // Handle any global properties
  const rapidjson::Value &globals = doc["globalProperties"];
  if (globals.IsObject()) {
    LoadGlobalProperties(game, globals);
  }

  // Handle any Objects
  const rapidjson::Value &Objects = doc["Objects"];
  if (Objects.IsArray()) {
    LoadObjects(game, Objects);
  }
  return true;
}

bool LevelLoader::LoadJSON(const std::string &fileName,
                           rapidjson::Document &outDoc) {
  // Load the file from disk into an ifstream in binary mode,
  // loaded with stream buffer at the end (ate)
  std::ifstream file(fileName, std::ios::in | std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    KE_CORE_ERROR("File {0} not found", fileName.c_str());
    return false;
  }

  // Get the current position in stream buffer, which is size of file
  std::ifstream::pos_type fileSize = file.tellg();
  // Seek back to start of file
  file.seekg(0, std::ios::beg);

  // Create a vector of size + 1 (for null terminator)
  std::vector<char> bytes(static_cast<size_t>(fileSize) + 1);
  // Read in bytes into vector
  file.read(bytes.data(), static_cast<size_t>(fileSize));

  // Load raw data into RapidJSON document
  outDoc.Parse(bytes.data());
  if (!outDoc.IsObject()) {
    KE_CORE_ERROR("File {0} is not valid JSON", fileName.c_str());
    return false;
  }

  return true;
}

void LevelLoader::SaveLevel(Application *game, const std::string &fileName) {
  KE_CORE_TRACE("SAVING SCENE");
  // Create the document and root object
  rapidjson::Document doc;
  doc.SetObject();

  // Write the version
  JsonHelper::AddInt(doc.GetAllocator(), doc, "version", LevelVersion);

  // Globals
  rapidjson::Value globals(rapidjson::kObjectType);
  SaveGlobalProperties(doc.GetAllocator(), game, globals);
  doc.AddMember("globalProperties", globals, doc.GetAllocator());

  // Objects
  rapidjson::Value Objects(rapidjson::kArrayType);
  SaveObjects(doc.GetAllocator(), game, Objects);
  doc.AddMember("Objects", Objects, doc.GetAllocator());

  // Save JSON to string buffer
  rapidjson::StringBuffer buffer;
  // Use PrettyWriter for pretty output (otherwise use Writer)
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  const char *output = buffer.GetString();

  // Write output to file
  std::ofstream outFile(fileName);
  if (outFile.is_open()) {
    outFile << output;
  }
  KE_CORE_TRACE("DONE SAVAING");
}

void LevelLoader::LoadGlobalProperties(Application *game,
                                       const rapidjson::Value &inObject) {
  // Get ambient light
  Vector3 ambient;
  if (JsonHelper::GetVector3(inObject, "ambientLight", ambient)) {
    game->GetRenderer()->SetAmbientLight(ambient);
  }

  // Get directional light
  const rapidjson::Value &dirObj = inObject["directionalLight"];
  if (dirObj.IsObject()) {
    DirectionalLight &light = game->GetRenderer()->GetDirectionalLight();

    // Set direction/color, if they exist
    JsonHelper::GetVector3(dirObj, "direction", light.mDirection);
    JsonHelper::GetVector3(dirObj, "color", light.mDiffuseColor);
  }
}

void LevelLoader::LoadObjects(Application *game,
                             const rapidjson::Value &inArray) {
  // Loop through array of Objects
  for (rapidjson::SizeType i = 0; i < inArray.Size(); i++) {
    const rapidjson::Value &ObjectObj = inArray[i];
    if (ObjectObj.IsObject()) {
      // Get the type
      std::string type, title;
      if (JsonHelper::GetString(ObjectObj, "type", type)) {
        // Is this type in the map?
        auto iter = sObjectFactoryMap.find(type);
        if (iter != sObjectFactoryMap.end()) {
          JsonHelper::GetString(ObjectObj, "title", title);
          // Construct with function stored in map
          Object *Object = iter->second(game,title, ObjectObj["properties"]);
          // Get the Object's components
          if (ObjectObj.HasMember("components")) {
            const rapidjson::Value &components = ObjectObj["components"];
            if (components.IsArray()) {
              LoadComponents(Object, components);
            }
          }
        } else {
          KE_CORE_ERROR("Unknown Object type {0}", type.c_str());
        }
      }
    }
  }
}

void LevelLoader::LoadComponents(Object *Object,
                                 const rapidjson::Value &inArray) {
  // Loop through array of components
  for (rapidjson::SizeType i = 0; i < inArray.Size(); i++) {
    const rapidjson::Value &compObj = inArray[i];
    if (compObj.IsObject()) {
      // Get the type
      std::string type;
      if (JsonHelper::GetString(compObj, "type", type)) {
        auto iter = sComponentFactoryMap.find(type);
        if (iter != sComponentFactoryMap.end()) {
          // Get the typeid of component
          Component::TypeID tid =
              static_cast<Component::TypeID>(iter->second.first);
          // Does the Object already have a component of this type?
          Component *comp = Object->GetComponentOfType(tid);
          if (comp == nullptr) {
            // It's a new component, call function from map
            comp = iter->second.second(Object, compObj["properties"]);
          } else {
            // It already exists, just load properties
            comp->LoadProperties(compObj["properties"]);
          }
        } else {
          KE_CORE_ERROR("Unknown component type %s", type.c_str());
        }
      }
    }
  }
}

void LevelLoader::LoadMusic(class SoundEvent *Sound, const rapidjson::Value &inArray)
{

}


void LevelLoader::SaveGlobalProperties(
    rapidjson::Document::AllocatorType &alloc, Application *game,
    rapidjson::Value &inObject) {
  // Ambient light
  JsonHelper::AddVector3(alloc, inObject, "ambientLight",
                         game->GetRenderer()->GetAmbientLight());

  // Directional light
  DirectionalLight &dirLight = game->GetRenderer()->GetDirectionalLight();
  rapidjson::Value dirObj(rapidjson::kObjectType);
  JsonHelper::AddVector3(alloc, dirObj, "direction", dirLight.mDirection);
  JsonHelper::AddVector3(alloc, dirObj, "color", dirLight.mDiffuseColor);
  inObject.AddMember("directionalLight", dirObj, alloc);
}

void LevelLoader::SaveObjects(rapidjson::Document::AllocatorType &alloc,
                             Application *game, rapidjson::Value &inArray) {
  const auto &Objects = game->GetObjects();
  for (const Object *Object : Objects) {
    // Make a JSON object
    rapidjson::Value obj(rapidjson::kObjectType);
    // Add type
    JsonHelper::AddString(alloc, obj, "type",
                          Object::TypeNames[Object->GetType()]);

    // Make object for properties
    rapidjson::Value props(rapidjson::kObjectType);
    // Save properties
    Object->SaveProperties(alloc, props);
    // Add the properties member
    obj.AddMember("properties", props, alloc);

    // Save components
    rapidjson::Value components(rapidjson::kArrayType);
    SaveComponents(alloc, Object, components);
    obj.AddMember("components", components, alloc);

    // Add Object to inArray
    inArray.PushBack(obj, alloc);
  }
}

void LevelLoader::SaveComponents(rapidjson::Document::AllocatorType &alloc,
                                 const Object *Object,
                                 rapidjson::Value &inArray) {
  const auto &components = Object->GetComponents();
  for (const Component *comp : components) {
    // Make a JSON object
    rapidjson::Value obj(rapidjson::kObjectType);
    // Add type
    JsonHelper::AddString(alloc, obj, "type",
                          Component::TypeNames[comp->GetType()]);

    // Make an object for properties
    rapidjson::Value props(rapidjson::kObjectType);
    // Save rest of properties
    comp->SaveProperties(alloc, props);
    // Add the member
    obj.AddMember("properties", props, alloc);

    // Add component to array
    inArray.PushBack(obj, alloc);
  }
}

void LevelLoader::SaveMusic(rapidjson::Document::AllocatorType &alloc, rapidjson::Value &inArray)
{
  //const auto &musics;
}

bool JsonHelper::GetInt(const rapidjson::Value &inObject,
                        const char *inProperty, int &outInt) {
  // Check if this property exists
  auto itr = inObject.FindMember(inProperty);
  if (itr == inObject.MemberEnd()) {
    return false;
  }

  // Get the value type, and check it's an integer
  auto &property = itr->value;
  if (!property.IsInt()) {
    return false;
  }

  // We have the property
  outInt = property.GetInt();
  return true;
}

bool JsonHelper::GetFloat(const rapidjson::Value &inObject,
                          const char *inProperty, float &outFloat) {
  auto itr = inObject.FindMember(inProperty);
  if (itr == inObject.MemberEnd()) {
    return false;
  }

  auto &property = itr->value;
  if (!property.IsDouble()) {
    return false;
  }

  outFloat = property.GetDouble();
  return true;
}

bool JsonHelper::GetString(const rapidjson::Value &inObject,
                           const char *inProperty, std::string &outStr) {
  auto itr = inObject.FindMember(inProperty);
  if (itr == inObject.MemberEnd()) {
    return false;
  }

  auto &property = itr->value;
  if (!property.IsString()) {
    return false;
  }

  outStr = property.GetString();
  return true;
}

bool JsonHelper::GetBool(const rapidjson::Value &inObject,
                         const char *inProperty, bool &outBool) {
  auto itr = inObject.FindMember(inProperty);
  if (itr == inObject.MemberEnd()) {
    return false;
  }

  auto &property = itr->value;
  if (!property.IsBool()) {
    return false;
  }

  outBool = property.GetBool();
  return true;
}

bool JsonHelper::GetVector3(const rapidjson::Value &inObject,
                            const char *inProperty, Vector3 &outVector) {
  auto itr = inObject.FindMember(inProperty);
  if (itr == inObject.MemberEnd()) {
    return false;
  }

  auto &property = itr->value;
  if (!property.IsArray() || property.Size() != 3) {
    return false;
  }

  for (rapidjson::SizeType i = 0; i < 3; i++) {
    if (!property[i].IsDouble()) {
      return false;
    }
  }

  outVector.x = property[0].GetDouble();
  outVector.y = property[1].GetDouble();
  outVector.z = property[2].GetDouble();

  return true;
}

bool JsonHelper::GetQuaternion(const rapidjson::Value &inObject,
                               const char *inProperty, Quaternion &outQuat) {
  auto itr = inObject.FindMember(inProperty);
  if (itr == inObject.MemberEnd()) {
    return false;
  }

  auto &property = itr->value;

  for (rapidjson::SizeType i = 0; i < 4; i++) {
    if (!property[i].IsDouble()) {
      return false;
    }
  }

  outQuat.x = property[0].GetDouble();
  outQuat.y = property[1].GetDouble();
  outQuat.z = property[2].GetDouble();
  outQuat.w = property[3].GetDouble();

  return true;
}

void JsonHelper::AddInt(rapidjson::Document::AllocatorType &alloc,
                        rapidjson::Value &inObject, const char *name,
                        int value) {
  rapidjson::Value v(value);
  inObject.AddMember(rapidjson::StringRef(name), v, alloc);
}

void JsonHelper::AddFloat(rapidjson::Document::AllocatorType &alloc,
                          rapidjson::Value &inObject, const char *name,
                          float value) {
  rapidjson::Value v(value);
  inObject.AddMember(rapidjson::StringRef(name), v, alloc);
}

void JsonHelper::AddString(rapidjson::Document::AllocatorType &alloc,
                           rapidjson::Value &inObject, const char *name,
                           const std::string &value) {
  rapidjson::Value v;
  v.SetString(value.c_str(), static_cast<rapidjson::SizeType>(value.length()),
              alloc);
  inObject.AddMember(rapidjson::StringRef(name), v, alloc);
}

void JsonHelper::AddBool(rapidjson::Document::AllocatorType &alloc,
                         rapidjson::Value &inObject, const char *name,
                         bool value) {
  rapidjson::Value v(value);
  inObject.AddMember(rapidjson::StringRef(name), v, alloc);
}

void JsonHelper::AddVector3(rapidjson::Document::AllocatorType &alloc,
                            rapidjson::Value &inObject, const char *name,
                            const Vector3 &value) {
  // Create an array
  rapidjson::Value v(rapidjson::kArrayType);
  // Push back elements
  v.PushBack(rapidjson::Value(value.x).Move(), alloc);
  v.PushBack(rapidjson::Value(value.y).Move(), alloc);
  v.PushBack(rapidjson::Value(value.z).Move(), alloc);

  // Add array to inObject
  inObject.AddMember(rapidjson::StringRef(name), v, alloc);
}

void JsonHelper::AddQuaternion(rapidjson::Document::AllocatorType &alloc,
                               rapidjson::Value &inObject, const char *name,
                               const Quaternion &value) {
  // Create an array
  rapidjson::Value v(rapidjson::kArrayType);
  // Push back elements
  v.PushBack(rapidjson::Value(value.x).Move(), alloc);
  v.PushBack(rapidjson::Value(value.y).Move(), alloc);
  v.PushBack(rapidjson::Value(value.z).Move(), alloc);
  v.PushBack(rapidjson::Value(value.w).Move(), alloc);

  // Add array to inObject
  inObject.AddMember(rapidjson::StringRef(name), v, alloc);
}
}
