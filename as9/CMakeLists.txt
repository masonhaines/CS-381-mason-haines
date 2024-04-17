cmake_minimum_required(VERSION 3.18)
project(as0 LANGUAGES CXX)

add_subdirectory(raylib-cpp)

file(GLOB sources "src/*.cpp" "src/*.c" "src/*hpp")
# this is to help manage the new files with structs
# set(sources
#     src/as8.cpp
#     src/component.hpp
#     src/transformComponent.hpp
#     src/renderingComponent.hpp
#     src/physicsComponent.hpp
#     src/bufferedInputComponent.hpp
#     src/delegate.hpp
#     src/entity.hpp
#     src/skybox.hpp
#     src/skybox.cpp
# )
add_executable(as8 ${sources})
target_link_libraries(as8 PUBLIC raylib raylib_cpp raylib::buffered)
set_property(TARGET as8 PROPERTY CXX_STANDARD 20)

configure_file(textures/skybox.png textures/skybox.png COPYONLY)
configure_file(textures/floor.jpg textures/floor.jpg COPYONLY)
configure_file(textures/water.jpg textures/water.jpg COPYONLY)

configure_file(meshes/Duck.glb meshes/Duck.glb COPYONLY)
configure_file(meshes/pickle.glb meshes/pickle.glb COPYONLY)
configure_file(meshes/Island.glb meshes/Island.glb COPYONLY)


include(includeable.cmake)

make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/skybox.vs generated/skybox.vs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/skybox.fs generated/skybox.fs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/cubemap.vs generated/cubemap.vs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/cubemap.fs generated/cubemap.fs)