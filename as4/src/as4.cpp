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
    Music music = LoadMusicStream("../Clock Town, First Day.mp3");
    Sound naviWatch = LoadSound("../OOT_Navi_WatchOut3.wav");
    Sound linkGrunt = LoadSound("../MM_Link_Attack4.wav");
    
    PlayMusicStream(music);

    Texture2D foreground = LoadTexture("../newground.png");
    // Texture2D midground = LoadTexture("../ground.png");
    // Texture2D background = LoadTexture("../ground.png");
    Texture2D wallpaper = LoadTexture("../backgroundtrees.jpg");
    Texture2D characterLink = LoadTexture("../linkpng.png");
    

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


    float scrollingBack = 0.0f;
    float scrollingMid = 0.0f;
    float scrollingFore = 0.0f;

    while (!window.ShouldClose()) {
        float height = window.GetHeight();
        float width = window.GetWidth();

        raylib::Vector2 velocity = {position.x * speed, position.y * speed *2};
        UpdateMusicStream(music);

        scrollingBack -= 0.1;
        scrollingMid -= 0.5;
        scrollingFore -= 1.0;
        
        // SetTargetFPS(60);      
        window.BeginDrawing();
        {   
            window.ClearBackground(GREEN);
            
            

            // // Draw background image twice
            // // NOTE: Texture is scaled twice its size
            // DrawTextureEx(background, (Vector2){ scrollingBack, 20 }, 0.0f, 2.0f, WHITE);
            // DrawTextureEx(background, (Vector2){ background.width*2 + scrollingBack, 20 }, 0.0f, 2.0f, WHITE);
            // // Draw midground image twice
            // DrawTextureEx(midground, (Vector2){ scrollingMid, 20 }, 0.0f, 2.0f, WHITE);
            // DrawTextureEx(midground, (Vector2){ midground.width*2 + scrollingMid, 20 }, 0.0f, 2.0f, WHITE);

            // Rectangle foreSource = (Rectangle){0, 0, 1024, 1024};
            // Rectangle foreDest = (Rectangle){static_cast<float>(width/5), static_cast<float>(height/5), foreSource.width, foreSource.height};
            // DrawTexturePro(wallpaper, foreSource, foreDest, position, rotate, WHITE);

            DrawTextureEx(foreground, (Vector2){ scrollingFore, 40}, 0, .5, WHITE);
            DrawTextureEx(foreground, (Vector2){ foreground.width*2 + scrollingFore, 70 }, 0.0f, 2.0f, WHITE);
            

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
            
            
            Rectangle linkSource = (Rectangle){0, 0, 350, 467};
            Rectangle linkDest = (Rectangle){static_cast<float>(width / 2), static_cast<float>(height / 2), linkSource.width, linkSource.height};
            DrawTexturePro(characterLink, linkSource, linkDest, position, rotate, WHITE);

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
