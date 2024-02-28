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
    raylib::Window window(monitor, monitor, "CS381 - Assignment 2");

    InitAudioDevice();
    Music music = LoadMusicStream("../sounds/clocktown.mp3");
    Sound naviWatch = LoadSound("../sounds/OOT_Navi_WatchOut3.wav");
    Sound linkGrunt = LoadSound("../sounds/MM_Link_Attack4.wav");
    
    PlayMusicStream(music);

    Texture2D foreground = LoadTexture("../textures/newground.png");
    Texture2D wallpaper = LoadTexture("../textures/backgroundtrees.jpg");
    Texture2D characterLink = LoadTexture("../textures/linkpng.png");
    

    raylib::Text text;
    float textSize = 25;

    float speed = 10; 
    int jump = 10;
    float jumpSpeed = 20; // Initial jump velocity
    float gravity = 0.8;  // Gravity to bring character back to the ground
    bool isJumping = false; // Flag to track if the character is currently jumping
    raylib::Vector2 position = {0, 0};
    int rotate = 0;

   

    if (characterLink.id <= 0) {
        std::cout << "------------------------------ * did not load in character * ------------------------------" << std::endl;
    }
    if (foreground.id <= 0) {
        std::cout << "------------------------------ * did not load in foreground * ------------------------------" << std::endl;
    }
    if (wallpaper.id <= 0) {
        std::cout << "------------------------------ * did not load in wallpaper * ------------------------------" << std::endl;
    }

    while (!window.ShouldClose()) {
        float height = window.GetHeight();
        float width = window.GetWidth();

        raylib::Vector2 velocity = {position.x * speed, position.y * speed *2};
        UpdateMusicStream(music);
        
        SetTargetFPS(60);      
        window.BeginDrawing();
        {   
            window.ClearBackground(GREEN);
            // Background and foreground for link
            DrawTextureEx(wallpaper, (Vector2){0, 0}, 0, 2.0, WHITE);
            DrawTextureEx(foreground, (Vector2){ - 50, 70}, 0, .5, WHITE);
            DrawTextureEx(foreground, (Vector2){ 600, 70}, 0, .5, WHITE);
            // Character creation
            Rectangle linkSource = (Rectangle){0, 0, 350, 467};
            Rectangle linkDest = (Rectangle){static_cast<float>(width / 2), 658, linkSource.width, linkSource.height};
            DrawTexturePro(characterLink, linkSource, linkDest, position, rotate, WHITE);
            
            if (IsKeyDown(KEY_D)) {
                position.x -= 4;
            }
            if (IsKeyDown(KEY_A)) {
                position.x += 4;
            }
            if (IsKeyPressed(KEY_W) && !isJumping) {
                // Start jumping only if not already jumping
                isJumping = true;
                PlaySound(linkGrunt);
            }
            // Jump physics
            if (isJumping) {
                position.y += 2.5;
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
            
            
            

        }
        window.EndDrawing();
    }

    UnloadMusicStream(music);
    UnloadTexture(characterLink);
    // UnloadTexture(background);  
    // UnloadTexture(midground);   
    UnloadTexture(foreground);  
    CloseAudioDevice();
}
