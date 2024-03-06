# The Story of Z3lda README

## Assignment AS4 AD HOC game CS 381

### Instructions for Installation

Please follow the steps below to install The Story of Z3lda game:

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
cd CS-381-mason-haines/as4/
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
./as4
```


## 6. Controls

- **Jumping**: 
  - Press 'W' to jump once. You have a maximum of two jumps, which reset upon landing after the second jump. You can execute a double jump by pressing 'W' twice consecutively.
- **Moving Right**: 
  - Press and hold 'D' to move the character towards the right side of the screen.
- **Moving Left**: 
  - Press and hold 'A' to move the character towards the left side of the screen.
- **Game Over**: 
  - Press 'SPACE' when the game ends. The screen will display "Game Over" and prompt you to press 'SPACE' to end the game.

## 7. Point of the Game

- **Objective**: 
  - Collect as many rupees as possible before losing all your hearts. Each hit by a regular arrow deducts 1 heart, while black arrows deduct 2 hearts.
  
### Rupees Colors and Values

- **Gold**: 
  - Value: 100 rupees
- **Red**: 
  - Value: 20 rupees
- **Green**: 
  - Value: 1 rupee
- **Blue**: 
  - Value: 5 rupees
- **Yellow**: 
  - Value: 10 rupees

Once you accumulate 1000 rupees, you gain one heart back.

# Game Assets

## Assets Created or Edited

- **Character, Castle Background, and Floor**:
  - Created using DALL·E-3 via Windows CoPilot. These assets were subsequently edited, involving cropping and detail adjustments. All assets were converted into PNG format.
- **Extra Credit**:
  - Arrows in the game were created by myself using Procreate.
