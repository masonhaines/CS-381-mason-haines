# The story of Z3lda README



# Instructions for install
## Please open your terminal and..


# 1. Update System and Install Dependencies

    sudo apt update

    sudo apt install xorg-dev

    sudo apt install pulseaudio

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

    ./as4

# 6. Controls

-   Press W to jump once. You only have two jumps and they reset after jumping twice and then hitting the ground. You can press w two times in a row and double jump.
-   Press and hold D to move the character right on the screen.
-   press and hold A to move the character left on the screen.
-   press SPACE when you have lost the game. The screen will display game over and inform you to press space to end the game.

# 7. Point of the game

- Collect as many rupees as possible before losing all your hearts. Getting hit by an arrow will cause you to lose 1 heart and the black arrow 2 hearts.
##Rupees colors and values
-   Gold: 100 rupee value and will give you a heart back as well.
-   Red: 20 rupees
-   Green: 1 rupees
-   Blue: 5 rupees
-   Yellow: 10 rupees
