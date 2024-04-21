#pragma once

#if KE_PLATFORM_WINDOWS
#include <windows.h>
#endif

//-------------SDL-----------------------
#include "SDL.h"

//-------- Animation --------------------

//--------------Input--------------------
#include "Input/InputSystem.h"

//-------Components----------------------
#include "Component/BoxComponent.h"
#include "Component/SpriteComponent.h"

//------------- Audio --------------------
#include "Audio/AudioSystem.h"
#include "Audio/SoundEvent.h"

//-------------- Renderer ---------------
#include "Component/MeshComponent.h"
#include "Renderer/Mesh.h"
#include "Renderer/Renderer.h"
#include "Renderer/Texture.h"

//------------------ Core -----------------
#include "Core/Application.h"
#include "GL/glew.h"
#include "LevelLoader.h"
#include "Log/Log.h"
#include "SDL.h"
#include "SDL_ttf.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "rapidjson/document.h"

//---------------Object------------------
#include "Object/BallMove.h"
#include "Object/BallObject.h"
#include "Object/CameraObject.h"
#include "Object/FollowObject.h"
#include "Object/Object.h"
#include "Object/PlaneObject.h"

//------------Log------------------------
#include "Log/Log.h"

//-----------------Entry Point-----------
#include "Core/Entry.h"
