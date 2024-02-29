#include "raylib-cpp.hpp"
#include "rlgl.h"
#include "skybox.hpp"
#include "raylib.h"
#include <iostream>
#include <random>
#include <string>

template<typename T>
concept Transformer = requires( T t, raylib::Transform m) {
    {t.operator()(m)}->std::convertible_to<raylib::Transform>;
};

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    int monitor = GetCurrentMonitor();
    raylib::Window window(GetMonitorWidth(monitor), GetMonitorHeight(monitor), "CS381 - Assignment 2");
    
    raylib::Text text;
    float textSize = 25;
    const char *title = {"ZELDA"};

    InitAudioDevice();
    Music music = LoadMusicStream("../sounds/clocktown.mp3");
    Sound naviWatch = LoadSound("../sounds/OOT_Navi_WatchOut3.wav");
    Sound linkGrunt = LoadSound("../sounds/MM_Link_Attack4.wav");
    
    PlayMusicStream(music);

    Texture2D foreground = LoadTexture("../textures/newground.png");
    Texture2D wallpaper = LoadTexture("../textures/backgroundtrees.jpg");
    Texture2D characterLink = LoadTexture("../textures/linkpng.png");
    Texture2D Rupee = LoadTexture("../textures/green_rupee_small.png");

    float height = window.GetHeight();
    float width = window.GetWidth();
    
    unsigned int seed = GetFrameTime();
    srand(seed);
    std::random_device randomSet;
    std::uniform_int_distribution<int> dist(0, 1855);
    float random1 = dist(randomSet);
    float random2 = dist(randomSet);
    float random3 = dist(randomSet);
    float random4 = dist(randomSet);
    float random5 = dist(randomSet);


    // Game play and physics variables 
    float speed = 19; // Initial speed 
    int jump = 10; // Jump multiplier 
    float jumpSpeed = 20; // Initial jump velocity
    float gravity = 4.9;  // Gravity to bring character back to the ground
    bool isJumping = false; // Flag to track if the character is currently jumping

    // 0 , 0 is the top left of screen, y+ is down into the screen and X+ is to the right 
    Vector2 position = {width / 2, 500};
    Vector2 rupeePos1 = {random1, -90};
    Vector2 rupeePos2 = {random2, -90};
    Vector2 rupeePos3 = {random3, -90};
    Vector2 rupeePos4 = {random4, -90};
    Vector2 rupeePos5 = {random5, -90};    


    if (characterLink.id <= 0) {
        std::cout << "------------------------------ * did not load in character * ------------------------------" << std::endl;
    }
    if (foreground.id <= 0) {
        std::cout << "------------------------------ * did not load in foreground * ------------------------------" << std::endl;
    }
    if (wallpaper.id <= 0) {
        std::cout << "------------------------------ * did not load in wallpaper * ------------------------------" << std::endl;
    }

    
    SetTargetFPS(60);  
    while (!window.ShouldClose()) {
        

        Vector2 rupeePos = {random5, -90}; // Initializer for items in game 
        UpdateMusicStream(music);

        // raylib::Vector2 velocity = {position.x * speed, position.y * speed *2};

        rupeePos1.y += gravity;
        rupeePos2.y += gravity;
        rupeePos3.y += gravity;
        rupeePos4.y += gravity;
        rupeePos5.y += gravity;

        // Stop rupees when they reach y position 885
        if (rupeePos1.y >= 885) rupeePos1.y = 890;
        if (rupeePos2.y >= 885) rupeePos2.y = 890;
        if (rupeePos3.y >= 885) rupeePos3.y = 890;
        if (rupeePos4.y >= 885) rupeePos4.y = 890;
        if (rupeePos5.y >= 885) rupeePos5.y = 890;

        // Reset rupee positions if they are ±5 away from the initial position
        if (std::abs(rupeePos1.x - position.x) < 5 && std::abs(rupeePos1.y - position.y) < 10) {
            // std::cout << "Rupee 1 position reset!" << std::endl;
            rupeePos1 = {static_cast<float>(rand() % 1856), -90};
        }
        if (std::abs(rupeePos2.x - position.x) < 5 || std::abs(rupeePos2.y - position.y) < 10) {
            rupeePos2 = {static_cast<float>(rand() % 1856), -90};
        }
        if (std::abs(rupeePos3.x - position.x) < 5 || std::abs(rupeePos3.y - position.y) < 10) {
            rupeePos3 = {static_cast<float>(rand() % 1856), -90};
        }
        if (std::abs(rupeePos4.x - position.x) < 5 || std::abs(rupeePos4.y - position.y) < 10) {
            rupeePos4 = {static_cast<float>(rand() % 1856), -90};
        }
        if (std::abs(rupeePos5.x - position.x) < 5 || std::abs(rupeePos5.y - position.y) < 10) {
            rupeePos5 = {static_cast<float>(rand() % 1856), -90};
        }


        // Wall boundary 
        if (position.x > 1775) {// Right boundary
            PlaySound(naviWatch);
            position.x = 1760;
        } 
        else if (position.x < -190) {// Left boundary
            PlaySound(naviWatch);
            position.x = -175; 
        }
        
        
        window.BeginDrawing();
        {   
            window.ClearBackground(GREEN);
            // Background and foreground for link
            DrawTextureEx(wallpaper, (Vector2){0, -150}, 0, 2, YELLOW);
            DrawTextureEx(foreground, (Vector2){ - 50, 70}, 0, .5, WHITE);
            DrawTextureEx(foreground, (Vector2){ 600, 70}, 0, .5, WHITE);
            // Character creation
            // Rectangle linkSource = (Rectangle){0, 0, 350, 467};
            // Rectangle linkDest = (Rectangle){static_cast<float>(width / 2), 658, linkSource.width, linkSource.height};
            // DrawTexturePro(characterLink, linkSource, linkDest, position, 0, WHITE);
            DrawTextureEx(characterLink, position, 0, 1, WHITE);

            // Rupee Creation
            DrawTextureEx(Rupee, rupeePos1, 0, .5, RED);
            DrawTextureEx(Rupee, rupeePos2, 0, .5, GREEN);
            DrawTextureEx(Rupee, rupeePos3, 0, .5, GOLD);
            DrawTextureEx(Rupee, rupeePos4, 0, .5, DARKPURPLE);
            DrawTextureEx(Rupee, rupeePos5, 0, .5, DARKBLUE);


            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {speed = 10;} 
            else speed = 1;
            if (IsKeyDown(KEY_D)) {position.x += 15 * speed;}
            if (IsKeyDown(KEY_A)) {position.x -= 15 * speed;}
            if (IsKeyPressed(KEY_W) && !isJumping) {
                // Start jumping only if not already jumping
                isJumping = true;
                PlaySound(linkGrunt);
            }
            // Jump physics
            if (isJumping) {
                position.y -= 2.5;
            } else if (position.y <= 558 && !isJumping) {
                position.y += 4;
            }
            // Check if the character has landed
            if (position.y <= 0) {
                isJumping = false;
            } else if (position.y <= 678) {
                position.y = 675;
                isJumping = false;
            }
            
            // Draw UI elements
            DrawFPS(10, 10);
            text.Draw(std::to_string(position.y), (width / 2.5), height * .15, textSize * 1.25, raylib::Color::Black());
            // text.Draw(std::to_string(position.x), (width / 2.5), height * .15, textSize * 1.25, raylib::Color::Black());
            text.Draw(std::to_string(position.x - rupeePos1.x), (width / 2.5), height * .5, textSize * 1.25, raylib::Color::Black());
            text.Draw(title, (width / 2.5), height * .25, textSize * 2.25, raylib::Color::Black());

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
