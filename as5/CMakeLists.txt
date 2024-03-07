cmake_minimum_required(VERSION 3.18)
project(as0 LANGUAGES CXX)

add_subdirectory(raylib-cpp)

file(GLOB sources "src/*.cpp" "src/*.c")
add_executable(as5 ${sources})
target_link_libraries(as5 PUBLIC raylib raygui raylib_cpp raylib::buffered)
set_property(TARGET as5 PROPERTY CXX_STANDARD 20)

configure_file(textures/skybox.png textures/skybox.png COPYONLY)
configure_file(textures/grass.jpg textures/grass.jpg COPYONLY)
configure_file(textures/water.jpg textures/water.jpg COPYONLY)
configure_file(textures/newground.png textures/newground.png COPYONLY)
configure_file(textures/linkpng.png textures/linkpng.png COPYONLY)
configure_file(textures/green_rupee_small.png textures/green_rupee_small.png COPYONLY)
configure_file(textures/white_rupee_small.png textures/white_rupee_small.png COPYONLY)
configure_file(textures/backgroundtrees.jpg textures/backgroundtrees.jpg COPYONLY)
configure_file(textures/arrow.png textures/arrow.png COPYONLY)
configure_file(sounds/OOT_Navi_WatchOut3.wav sounds/OOT_Navi_WatchOut3.wav COPYONLY)
configure_file(sounds/clocktown.mp3 sounds/clocktown.mp3 COPYONLY)
configure_file(sounds/MM_Link_Attack4.wav sounds/MM_Link_Attack4.wav COPYONLY)


include(includeable.cmake)
