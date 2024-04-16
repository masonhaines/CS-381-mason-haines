# AS8 - Object Oriented Entity Component Ships and Planes - README

This program is designed to create a simple window featuring 5 3D planes and 5 3d Ships allowing you to interact with each one! PLanes have 3D physics and interaction while the boats only have 2D.

### Instructions for Installation

Please follow the steps below to install

### 1. Update System and Install Dependencies

```bash
sudo apt update
sudo apt install xorg-dev
sudo apt install pulseaudio
sudo apt install cmake
```

### 2. Clone the Repository Into Preferred Directory

```bash
git clone https://github.com/masonhaines/CS-381-mason-haines.git
cd CS-381-mason-haines/as8/
```

### 3. Fetch Git Submodules

```bash
git submodule update --init --recursive
```

### 3.5. Additional Setup (Optional)

If you're using Visual Studio Code, please install the following extensions:

- C/C++ for Visual Studio Code
- CMake for Visual Studio Code
- Visual Studio Code C++ Extension Pack

### 4. Build

```bash
mkdir build
cd build
cmake ..
make
```

### 5. Executable

To exit the program/window, press 'X' on the top right of the window, and the program will terminate.

```bash
./as8
```


## 6. Controls


  - Press 'W' to go forward

  - Press 'S' to go slowdown

  - Press 'A' to turn left 

  - Press 'D' to turn right 

  - Press 'tab' to cycle between all plane and ship models

  - Press 'R' to tilt plane right

  - Press 'F' to tilt plane left

  - Press 'Q' to tilt plane forward

  - Press 'E' to tilt plane backwards

  - Press 'C' to change camera view


## 7. Extra Credit 

Can change camera view after selecting model and pressing c