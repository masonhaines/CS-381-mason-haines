# Plane Takeoff! README

This program is designed to create a simple window featuring a 3D plane plane taking off. After taking off user can control the plane as it flies.

# Question   
Can we simplify the problem by remapping camera directions to specific world axes (e.g. W moves along say -Z)? Why or why not can we use this simplification in this assignment?

Yes it is possible. we can simplify th e problem by doing so with the pre made camera perspectives like CameraObj.target and CameraObj.position. 
Using these would give a full access to the camera's functionality in Raylib. 
We would not be able to use this functionality completely on this assignment because each control needs to reflect a direction with the perspective to the camera. 

 
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

    ./as2

# 6. Controls

- Pressing LEFT CLICK on mouse begins plane movement
- Pressing SPACE-BAR to stop plane
- Pressing LEFT SHIFT to accelerate plane
- Pressing W to go forward
- Pressing S to go backwards
- Pressing A to turn left 
- Pressing D to turn right 
- Pressing Q to go upwards
- Pressing E to go downwards

