cmake_minimum_required(VERSION 3.18)
project(as0 LANGUAGES CXX)

add_subdirectory(raylib-cpp)

file(GLOB sources "src/*.cpp" "src/*.c")
add_executable(as1 ${sources})
target_link_libraries(as1 PUBLIC raylib raylib_cpp)
set_property(TARGET as1 PROPERTY CXX_STANDARD 20)

configure_file(textures/skybox.png textures/skybox.png COPYONLY)
configure_file(textures/grass.jpg textures/grass.jpg COPYONLY)
configure_file(meshes/PolyPlane.glb meshes/PolyPlane.glb COPYONLY)
configure_file(meshes/OrientExplorer.glb meshes/OrientExplorer.glb COPYONLY)
configure_file(meshes/OilTanker.glb meshes/OilTanker.glb COPYONLY)
configure_file(meshes/SmitHouston_Tug.glb meshes/SmitHouston_Tug.glb COPYONLY)
configure_file(meshes/ddg51.glb meshes/ddg51.glb COPYONLY)
configure_file(meshes/Barrel1.glb meshes/Barrel1.glb COPYONLY)
configure_file(meshes/Doughnut.glb meshes/Doughnut.glb COPYONLY)




include(includeable.cmake)

make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/skybox.vs generated/skybox.vs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/skybox.fs generated/skybox.fs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/cubemap.vs generated/cubemap.vs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/cubemap.fs generated/cubemap.fs)