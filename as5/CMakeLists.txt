cmake_minimum_required(VERSION 3.18)
project(as0 LANGUAGES CXX)

add_subdirectory(raylib-cpp)

file(GLOB sources "src/*.cpp" "src/*.c")
add_executable(as5 ${sources})
target_link_libraries(as5 PUBLIC raylib raygui raylib_cpp raylib::buffered)
set_property(TARGET as5 PROPERTY CXX_STANDARD 20)


configure_file(sounds/OOT_Navi_WatchOut3.wav sounds/OOT_Navi_WatchOut3.wav COPYONLY)
configure_file(sounds/clocktown.mp3 sounds/clocktown.mp3 COPYONLY)
configure_file(sounds/roll.mp3 sounds/roll.mp3 COPYONLY)
configure_file(sounds/banana_bread_at_work.mp3 sounds/banana_bread_at_work.mp3 COPYONLY)
configure_file(audio/crowd.wav audio/crowd.wav COPYONLY)


include(includeable.cmake)
