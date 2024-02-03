# A Simple Window README

This program is designed to create a simple window featuring centered text that remains centered even when the window is resized. Some creative liberty has been taken to make this task more appealing.

# Instructions 
## Please open your terminal and..


# 1. Update System and Install Dependencies

    sudo apt update

    sudo apt install xorg-dev

    sudo apt install cmake

# 2. Clone the Repository Into Preferred Directory

    git clone https://github.com/masonhaines/CS-381-mason-haines.git

    cd CS-381-mason-haines/as0/

# 3. Fetch Git Submodules

    git submodule update --init --recursive

# 3.5. Additional Setup (Optional)

## If you're using Visual Studio Code, please install the following extensions:

-   C/C++ for Visual Studio Code
-   CMake for Visual Studio Code
-   Visual Studio Code C++ Extension Pack

# 4. Build

    mkdir build

    cd build

    cmake ..
    
    make

# 5. Executable

    ./as0

# 6. Controls

No applicable controls*