// Mason Haines Volume GUI

#include "raylib-cpp.hpp"
#include "rlgl.h"
#include "raylib.h"
#include <iostream>
#include "delegate.hpp"
#include "BufferedInput.hpp"

cs381::Delegate<void()> PingButton;

#define GUI_VOLUMECONTROL_IMPLEMENTATION
#include "VolumeControl.h"


int main() {

    PingButton += []()-> void {
        std::cout << "ping!" << std::endl;
    };

    // Set configuration of window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    int monitor = GetCurrentMonitor();
    raylib::Window window(500, 400, "CS381 - Assignment 5");
    raylib::AudioDevice audio;
    // InitAudioDevice();
    
    raylib::Sound ping("../sounds/OOT_Navi_WatchOut3.wav");
    PingButton += [&ping]()-> void {
        if (!ping.IsPlaying()) {
            ping.Play();      
        }
        
    };


    raylib::Music music("../sounds/clocktown.mp3");

    auto guiState = InitGuiVolumeControl();

    while (!window.ShouldClose()) {

        
        
        window.BeginDrawing();
        {   
            GuiVolumeControl(&guiState);
            if (IsKeyPressed(KEY_SPACE)) {
                PingButton();
            }
            
            music.Update();
            music.SetVolume(25);

        }
        window.EndDrawing();
    }
    // ping.Unload();
    // music.Unload();
}


