#include "Animation/Animation.h"
#include "Animation/Skeleton.h"
#include "LevelLoader.h"
#include "Log/Log.h"
#include <rapidjson/document.h>

namespace Knuckles {
bool Animation::Load(const std::string &fileName) {
  mFileName = fileName;
  rapidjson::Document doc;
  if (!LevelLoader::LoadJSON(fileName, doc)) {
    KE_CORE_ERROR("Failed to load animation {0}", fileName.c_str());
    return false;
  }

  int ver = doc["version"].GetInt();

  // Check the metadata
  if (ver != 1) {
    KE_CORE_ERROR("Animation {0} unknown format", fileName.c_str());
    return false;
  }

  const rapidjson::Value &sequence = doc["sequence"];
  if (!sequence.IsObject()) {
    KE_CORE_ERROR("Animation {0} doesn't have a sequence.", fileName.c_str());
    return false;
  }

  const rapidjson::Value &frames = sequence["frames"];
  const rapidjson::Value &length = sequence["length"];
  const rapidjson::Value &bonecount = sequence["bonecount"];

  if (!frames.IsUint() || !length.IsDouble() || !bonecount.IsUint()) {
    KE_CORE_ERROR("Sequence {0} has invalid frames, length, or bone count.",
            fileName.c_str());
    return false;
  }

  mNumFrames = frames.GetUint();
  mDuration = length.GetDouble();
  mNumBones = bonecount.GetUint();
  mFrameDuration = mDuration / (mNumFrames - 1);

  mTracks.resize(mNumBones);

  const rapidjson::Value &tracks = sequence["tracks"];

  if (!tracks.IsArray()) {
    KE_CORE_ERROR("Sequence {0} missing a tracks array.", fileName.c_str());
    return false;
  }

  for (rapidjson::SizeType i = 0; i < tracks.Size(); i++) {
    if (!tracks[i].IsObject()) {
      KE_CORE_ERROR("Animation {0}: Track element {1} is invalid.", fileName.c_str(),
              i);
      return false;
    }

    size_t boneIndex = tracks[i]["bone"].GetUint();

    const rapidjson::Value &transforms = tracks[i]["transforms"];
    if (!transforms.IsArray()) {
      KE_CORE_ERROR("Animation %s: Track element %d is missing transforms.",
              fileName.c_str(), i);
      return false;
    }

    BoneTransform temp;

    if (transforms.Size() < mNumFrames) {
      KE_CORE_ERROR("Animation %s: Track element %d has fewer frames than expected.",
              fileName.c_str(), i);
      return false;
    }

    for (rapidjson::SizeType j = 0; j < transforms.Size(); j++) {
      const rapidjson::Value &rot = transforms[j]["rot"];
      const rapidjson::Value &trans = transforms[j]["trans"];

      if (!rot.IsArray() || !trans.IsArray()) {
        KE_CORE_ERROR("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
        return false;
      }

      temp.mRotation.x = rot[0].GetDouble();
      temp.mRotation.y = rot[1].GetDouble();
      temp.mRotation.z = rot[2].GetDouble();
      temp.mRotation.w = rot[3].GetDouble();

      temp.mTranslation.x = trans[0].GetDouble();
      temp.mTranslation.y = trans[1].GetDouble();
      temp.mTranslation.z = trans[2].GetDouble();

      mTracks[boneIndex].emplace_back(temp);
    }
  }

  return true;
}

void Animation::GetGlobalPoseAtTime(std::vector<Matrix4> &outPoses,
                                    const Skeleton *inSkeleton,
                                    float inTime) const {
  if (outPoses.size() != mNumBones) {
    outPoses.resize(mNumBones);
  }

  // Figure out the current frame index and next frame
  // (This assumes inTime is bounded by [0, AnimDuration]
  size_t frame = static_cast<size_t>(inTime / mFrameDuration);
  size_t nextFrame = frame + 1;
  // Calculate fractional value between frame and next frame
  float pct = inTime / mFrameDuration - frame;

  // Setup the pose for the root
  if (mTracks[0].size() > 0) {
    // Interpolate between the current frame's pose and the next frame
    BoneTransform interp = BoneTransform::Interpolate(
        mTracks[0][frame], mTracks[0][nextFrame], pct);
    outPoses[0] = interp.ToMatrix();
  } else {
    outPoses[0] = Matrix4::Identity;
  }

  const std::vector<Skeleton::Bone> &bones = inSkeleton->GetBones();
  // Now setup the poses for the rest
  for (size_t bone = 1; bone < mNumBones; bone++) {
    Matrix4 localMat; // (Defaults to identity)
    if (mTracks[bone].size() > 0) {
      BoneTransform interp = BoneTransform::Interpolate(
          mTracks[bone][frame], mTracks[bone][nextFrame], pct);
      localMat = interp.ToMatrix();
    }

    outPoses[bone] = localMat * outPoses[bones[bone].mParent];
  }
}
} // namespace Knuckles
