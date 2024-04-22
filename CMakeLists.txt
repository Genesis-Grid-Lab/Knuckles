cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# build output directories

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<0:>${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<0:>${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<0:>${CMAKE_BINARY_DIR}/lib)
set(CMAKE_PDB_OUTPUT_DIRECTORY $<0:>${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH $<0:>${CMAKE_BINARY_DIR}/bin)

# cpp standard options

set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 17)

project(Knuckles)

file(GLOB SOURCES src/*.cpp)

add_library(${PROJECT_NAME} ${SOURCES})

if(UNIX)
add_compile_definitions(
KE_PLATFORM_LINUX 
)

add_subdirectory(vendor/imgui imgui)
add_subdirectory(vendor/SOIL)
endif(UNIX)

if(WIN32)
add_compile_definitions(KE_PLATFORM_WINDOWS  _CRT_SECURE_NO_WARNINGS)
endif(WIN32)

# copy Assets
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Assets)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND 
        ${CMAKE_COMMAND} -E copy_directory 
        ${CMAKE_CURRENT_SOURCE_DIR}/Assets
        ${CMAKE_BINARY_DIR}/bin/Assets
    )
else()
    message(WARNING "[WARNING] no assets directory!")
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Shaders)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND 
        ${CMAKE_COMMAND} -E copy_directory 
        ${CMAKE_CURRENT_SOURCE_DIR}/Shaders
        ${CMAKE_BINARY_DIR}/bin/Shaders
    )
else()
    message(WARNING "[WARNING] no Shaders directory!")
endif()

target_include_directories(${PROJECT_NAME}
  PUBLIC include
  PUBLIC vendor/FMOD/api/studio/inc
  PUBLIC vendor/FMOD/api/lowlevel/inc
  PUBLIC vendor/imgui
  PUBLIC vendor/SOIL/inc
  PUBLIC ${CONAN_INCLUDE_DIRS}
  )

target_link_directories(${PROJECT_NAME} PUBLIC 
    ${CONAN_LIB_DIRS}  
)


if(UNIX)
  target_link_libraries(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/FMOD/api/studio/lib/x86_64/libfmodstudioL.so
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/FMOD/api/lowlevel/lib/x86_64/libfmodL.so
    imgui
    soil
    ${CONAN_LIBS}
    )
endif(UNIX)

if(WIN32)

  target_link_libraries(${PROJECT_NAME}
    ${CONAN_LIBS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/FMOD/api/studio/lib/x86_64/win/fmodstudio.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/FMOD/api/lowlevel/lib/x86_64/win/fmod.dll
    )
endif(WIN32)
