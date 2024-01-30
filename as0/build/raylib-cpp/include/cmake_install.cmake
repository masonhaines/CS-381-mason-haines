# Install script for directory: /mnt/c/UNR CSE/cs381/as0/raylib-cpp/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/AudioDevice.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/AudioStream.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/BoundingBox.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Camera2D.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Camera3D.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Color.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Degree.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Font.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Functions.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Gamepad.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Image.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Material.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Matrix.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Mesh.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Model.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/ModelAnimation.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Mouse.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Music.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Ray.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/RayCollision.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/RaylibException.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/raylib-cpp-utils.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/raylib-cpp.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/raylib.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/raymath.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Rectangle.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/RenderTexture.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Shader.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Sound.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Text.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Texture.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/TextureUnmanaged.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Touch.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Vector2.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Vector3.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Vector4.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/VrStereoConfig.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Wave.hpp"
    "/mnt/c/UNR CSE/cs381/as0/raylib-cpp/include/Window.hpp"
    )
endif()

