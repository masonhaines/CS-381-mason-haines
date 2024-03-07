// Mason Haines Volume GUI

#include "raylib-cpp.hpp"
#include "rlgl.h"
#include "raylib.h"
#include "delegate.hpp"
#include <BufferedInput.hpp>

cs381::Delegate<void()> PingButton;

#define GUI_VOLUMECONTROL_IMPLEMENTATION
#include "VolumeControl.h"

int main() {

    // Set configuration of window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    raylib::Window window(310, 350, "CS381 - Assignment 5");
    float height = window.GetHeight(); // window height variable 
    float width = window.GetWidth(); // window width variable 

    raylib::Text text; // Create text object 
    float textSize = 15; // Text size generic size
    const char *change = {"Press C to change dialogue audio"}; // Create array of chars for text 
    bool dialogueSwap = true; // Swap The the dialogue audio boolean

    raylib::BufferedInput inputs; // Manager for actions 
    raylib::AudioDevice audio; // Initialize audio 

    raylib::Sound ping("../sounds/OOT_Navi_WatchOut3.wav"); // Load sound object ping with sound byte
    PingButton += [&ping]()-> void {
        if (!ping.IsPlaying()) { // If sound is not playing start playing sound
            ping.Play();      
        }
    };

    inputs["ping"] = // Naming action 
    raylib::Action::key(KEY_SPACE) // When space key is hit action happens
    .SetPressedCallback([]{ // Delegate is calling delegate 
        PingButton();
    }).move(); // Moving action into input manager 

    
    // Create Music objects and load with mp3 and wav files
    raylib::Music music("../sounds/clocktown.mp3");
    raylib::Music dialogue("../audio/crowd.wav");
    raylib::Music dialogue2("../sounds/roll.mp3");
    // start playing music objects 
    music.Play();
    dialogue.Play();
    
    auto guiState = InitGuiVolumeControl(); // Initialize gui for volume controls

    while (!window.ShouldClose()) {
        inputs.PollEvents(); // Event tree of what events have happened since last polled events, old events not processed again

        window.BeginDrawing();
        {   
            window.ClearBackground(BLACK); // Set color for GUI
            GuiVolumeControl(&guiState); // Give GUi control state object 

            ping.SetVolume(guiState.SFXSliderValue / 100); // Set ping sound to volume on SFX slider inside Gui
            
            music.Update();
            music.SetVolume(guiState.MusicSliderValue / 100); // Set music volume to value on MusicSlider inside Gui

            if(dialogueSwap) {
                dialogue.Update();
                dialogue.SetVolume(guiState.DialogueSliderValue / 100); // Set volume of active dialogue to value on DialogueSlider inside Gui
            } else {
                dialogue2.Update();
                dialogue2.SetVolume(guiState.DialogueSliderValue / 100); // Set volume of alternative dialogue to value on DialogueSlider inside Gui
            }

            // Key to toggle between different dialogue audio 
            if (IsKeyPressed(KEY_C)) {
                if (dialogueSwap == false) {
                    dialogueSwap = true; 
                    dialogue2.Stop();
                    dialogue.Play(); // Play active dialogue when 'C' key is pressed
                }
                else {
                    dialogueSwap = false; 
                    dialogue.Stop();
                    dialogue2.Play(); // Play alternative dialogue when 'C' key is pressed
                }
            }

            // little side bar instructions for changing dialogue audio
            raylib::DrawTextPro(GetFontDefault(), change, {width, height / 15},
            {0, 0}, 90, textSize , 2, GREEN);

        }
        window.EndDrawing();
    }
}


