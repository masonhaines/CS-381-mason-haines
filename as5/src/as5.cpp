// Mason Haines Volume GUI

#include "raylib-cpp.hpp"
#include "rlgl.h"
#include "skybox.hpp"
#include "raylib.h"
#include <iostream>

void PingButton() {
    std::cout << "ping!" << std::endl;
}

#include "VolumeControl.h"
#define GUI_VOLUMECONTROL_IMPLEMENTATION


int main() {
    // Set configuration of window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    int monitor = GetCurrentMonitor();
    raylib::Window window(500, 400, "CS381 - Assignment 5");
    

    while (!window.ShouldClose()) {

        
        
        window.BeginDrawing();
        {   
            

        }
        window.EndDrawing();
    }
}


/// Feb 28 lecture at 39 minutes as of 3/5 at 5:25pm