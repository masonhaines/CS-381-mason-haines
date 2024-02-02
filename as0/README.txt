A Simple Window README

This program is designed to create a simple window that features centered text that remains centered even when the window is resized. Some creative liberty has been taken to make this task more appealing.
Instructions
1. Update System and Install Dependencies

bash

    sudo apt update
    sudo apt install xorg-dev
    sudo apt install cmake

2. Clone the Repository Into Preferred Directory

bash

    git clone https://github.com/masonhaines/CS-381-mason-haines.git
    cd CS-381-mason-haines/as0/

3. Fetch Git Submodules

bash

    git submodule update --init --recursive

4. Build

bash

    mkdir build
    cd build
    cmake ..
    make

5. Executable

bash(inside terminal)

    ./as0

Additional Setup (Optional)

If you're using Visual Studio Code, please install the following extensions:

-C/C++ for Visual Studio Code
-CMake for Visual Studio Code
-Visual Studio Code C++ Extension Pack