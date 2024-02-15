# Plane Takeoff! README

This program is designed to create a simple window featuring a 3D plane plane taking off. After taking off user can control the plane as it flies.

# Instructions 
## Please open your terminal and..


# 1. Update System and Install Dependencies

    sudo apt update

    sudo apt install xorg-dev

    sudo apt install cmake

# 2. Clone the Repository Into Preferred Directory

    git clone https://github.com/masonhaines/CS-381-mason-haines.git

    cd CS-381-mason-haines/as2/

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
To exit program/window press X on top right of window and program will terminate. 

    ./as0

# 6. Controls

• Pressing W to go forward
• Pressing S to go backwards
• Pressing A to turn left 
• Pressing D to turn right 
• Pressing Q to go upwards
• Pressing E to go downwards