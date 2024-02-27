#include "raylib-cpp.hpp"
#include "rlgl.h"
#include "skybox.hpp"
#include "raylib.h"
#include <iostream>

template<typename T>
concept Transformer = requires( T t, raylib::Transform m) {
    {t.operator()(m)}->std::convertible_to<raylib::Transform>;
};

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    int monitor = GetCurrentMonitor();
    raylib::Window window(GetMonitorWidth(monitor - 400), GetMonitorHeight(monitor - 400), "CS381 - Assignment 2");

    InitAudioDevice();
    Music music = LoadMusicStream("../Clock Town, First Day.mp3");
    Sound naviWatch = LoadSound("../OOT_Navi_WatchOut3.wav");
    Sound linkGrunt = LoadSound("../MM_Link_Attack4.wav");
    
    PlayMusicStream(music);

    raylib::Text text;
    float textSize = 25;

    float speed = 10; 
    int jump = 10;
    float jumpSpeed = 20; // Initial jump velocity
    float gravity = 0.8;  // Gravity to bring character back to the ground
    bool isJumping = false; // Flag to track if the character is currently jumpin
    raylib::Vector2 position = {0, 0};
    int rotate = 0;

    Texture2D characterLink = LoadTexture("../linkpng.png");
    Rectangle frameRec = { 0.0f, 0.0f, 350, 467 };
    if (characterLink.id <= 0) {
        std::cout << "------------------------------ * did not work * ------------------------------" << std::endl;
    }

    while (!window.ShouldClose()) {
        float height = window.GetHeight();
        float width = window.GetWidth();

        raylib::Vector2 velocity = {position.x * speed, position.y * speed *2};

        UpdateMusicStream(music);

        window.BeginDrawing();
        {   
            window.ClearBackground(GREEN);
            // if (IsKeyDown(KEY_SPACE)) {
            //     rotate += 5 * GetTime(); // Update rotation with frame time
            // }
            if (IsKeyDown(KEY_D)) {
                position.x -= 1;
            }
            if (IsKeyDown(KEY_A)) {
                position.x += 1;
            }
            if (IsKeyPressed(KEY_W) && !isJumping) {
                // Start jumping only if not already jumping
                isJumping = true;
                PlaySound(linkGrunt);
            }

            // Jump physics
            if (isJumping) {
                
                position.y += 2.5;
                // jumpSpeed -= gravity; // Apply gravity
            } else if (!isJumping) {
                position.y -= 4;
            }
            // Check if the character has landed
            if (position.y >= 200) {
                
                isJumping = false;
            } else if (position.y <= -2.5) {
                position.y = 0;
                isJumping = false;
            }
            
            // DrawTextureRec(characterLink, frameRec, position, WHITE);
            Rectangle linkSource = (Rectangle){0, 0, 350, 467};
            Rectangle linkDest = (Rectangle){static_cast<float>(width / 2), static_cast<float>(height / 2), linkSource.width, linkSource.height};
            DrawTexturePro(characterLink, linkSource, linkDest, position, rotate, WHITE);

        }
        window.EndDrawing();
    }

    UnloadMusicStream(music);
    CloseAudioDevice();
}
