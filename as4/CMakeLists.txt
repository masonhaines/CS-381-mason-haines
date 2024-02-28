cmake_minimum_required(VERSION 3.18)
project(as0 LANGUAGES CXX)

add_subdirectory(raylib-cpp)

file(GLOB sources "src/*.cpp" "src/*.c")
add_executable(as4 ${sources})
target_link_libraries(as4 PUBLIC raylib raylib_cpp raygui)
set_property(TARGET as4 PROPERTY CXX_STANDARD 20)

configure_file(textures/skybox.png textures/skybox.png COPYONLY)
configure_file(textures/grass.jpg textures/grass.jpg COPYONLY)
configure_file(textures/water.jpg textures/water.jpg COPYONLY)
configure_file(textures/newground.png textures/newground.png COPYONLY)
configure_file(textures/linkpng.png textures/linkpng.png COPYONLY)
configure_file(textures/green_rupee_small.png textures/green_rupee_small.png COPYONLY)
configure_file(textures/backgroundtrees.jpg textures/backgroundtrees.jpg COPYONLY)
configure_file(sounds/OOT_Navi_WatchOut3.wav sounds/OOT_Navi_WatchOut3.wav COPYONLY)
configure_file(sounds/clocktown.mp3 sounds/clocktown.mp3 COPYONLY)
configure_file(sounds/MM_Link_Attack4.wav sounds/MM_Link_Attack4.wav COPYONLY)


include(includeable.cmake)

make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/skybox.vs generated/skybox.vs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/skybox.fs generated/skybox.fs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/cubemap.vs generated/cubemap.vs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/cubemap.fs generated/cubemap.fs)