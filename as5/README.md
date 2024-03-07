# Assignment AS5 Audio GUI CS 381

### Instructions for Installation

Please follow the steps below to install AUDIO GUI

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
cd CS-381-mason-haines/as5/
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
./as5
```


## 6. Controls

- **Volume Sliders**: 
  - o adjust the volume, with the mouse left-click and hold on the blue bar, then drag right to increase the volume or left to decrease it.
- **Ping**: 
  - Press the 'Space' key to play the ping sound. Alternatively, you can use the mouse to left-click on the ping button.
- **Change dialogue**: 
  - Press 'c' to switch to a different dialogue audio while the current one is playing.


