cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 17)

project(Knuckles)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${SDL2_INCLUDE_DIRS})

file(GLOB SOURCES src/*.cpp)

add_library(${PROJECT_NAME} ${SOURCES})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/Glew/build/cmake)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL2_ttf/lib/cmake/SDL2_ttf)
if(UNIX)
add_subdirectory(vendor/SOIL)
add_subdirectory(vendor/SDL2)
add_subdirectory(vendor/SDL2_ttf)
add_subdirectory(vendor/imgui imgui)
endif(UNIX)

if(WIN32)
ADD_LIBRARY( soil SHARED
  vendor/soil/src/image_helper.c
  vendor/soil/src/stb_image_aug.c
  vendor/soil/src/image_DXT.c
  vendor/soil/src/SOIL.c
  )
endif(WIN32)



if(UNIX)
add_compile_definitions(
KE_PLATFORM_LINUX 
)
endif(UNIX)

if(WIN32)
add_compile_definitions(KE_PLATFORM_WINDOWS  _CRT_SECURE_NO_WARNINGS)
endif(WIN32)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy_directory 
  "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
  "${CMAKE_CURRENT_BINARY_DIR}/Assets" 
    COMMENT "Copying assets to output directory")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy_directory 
  "${CMAKE_CURRENT_SOURCE_DIR}/Shaders"
  "${CMAKE_CURRENT_BINARY_DIR}/Shaders" 
    COMMENT "Copying Shader to output directory")

target_include_directories(${PROJECT_NAME}
  PUBLIC include
  PUBLIC vendor/SDL2_ttf
  PUBLIC vendor/SDL2/include
  PUBLIC vendor/Glew/include
  PUBLIC vendor/SOIL/inc
  PUBLIC vendor/rapidjson/include
  PUBLIC vendor/FMOD/api/studio/inc
  PUBLIC vendor/FMOD/api/lowlevel/inc
  PUBLIC vendor/spdlog/include
  PUBLIC vendor/imgui
  )



if(UNIX)
  target_link_libraries(${PROJECT_NAME}
    ${SDL2_LIBRARIES}
    SDL2_ttf
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/FMOD/api/studio/lib/x86_64/libfmodstudioL.so
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/FMOD/api/lowlevel/lib/x86_64/libfmodL.so
    imgui
    glew
    soil
    )
endif(UNIX)

target_link_libraries(imgui ${SDL2_LIBRARIES})

if(WIN32)
  target_include_directories(soil PUBLIC vendor/soil/inc)
  target_link_libraries(soil glew)

  target_link_libraries(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL2_ttf/lib/win/SDL2_ttf.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL2/lib/win/SDL2.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/FMOD/api/studio/lib/x86_64/win/fmodstudio.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/FMOD/api/lowlevel/lib/x86_64/win/fmod.dll
    glew
    soil
    )
endif(WIN32)
