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
    const char *totalRupeesTitle = {"Rupee Wallet "};
    const char *title = {"The Story of"};
    const char *title1 = {"Z3LDA"};

    InitAudioDevice();
    Music music = LoadMusicStream("../sounds/clocktown.mp3");
    Sound naviWatch = LoadSound("../sounds/OOT_Navi_WatchOut3.wav");
    Sound linkGrunt = LoadSound("../sounds/MM_Link_Attack4.wav");
    
    PlayMusicStream(music);

    Texture2D foreground = LoadTexture("../textures/newground.png");
    Texture2D wallpaper = LoadTexture("../textures/backgroundtrees.jpg");
    Texture2D characterLink = LoadTexture("../textures/linkpng.png");
    Texture2D Rupee = LoadTexture("../textures/white_rupee_small.png");

    float height = window.GetHeight();
    float width = window.GetWidth();
    
    unsigned int seed = GetFrameTime();
    // srand(seed);
    std::random_device randomSet;
    std::uniform_int_distribution<int> dist(0, 1855);
    float random1 = dist(randomSet);
    float random2 = dist(randomSet);
    float random3 = dist(randomSet);
    float random4 = dist(randomSet);
    float random5 = dist(randomSet);


    // Game play and physics variables 
    int counter = 0;
    float speed = 5; // Initial speed 
    int jump = 10; // Jump multiplier 
    float jumpSpeed = 20; // Initial jump velocity
    float gravity = .9;  // Gravity to bring character back to the ground
    bool isJumping = false; // Flag to track if the character is currently jumping

    // 0 , 0 is the top left of screen, y+ is down into the screen and X+ is to the right 
    Vector2 linkPos = {width / 2, 675};
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
        

        std::random_device randomRunTime;
        std::uniform_int_distribution<int> dist(0, 1855);
        float randomized1 = dist(randomRunTime);
        float randomized2 = dist(randomRunTime);
        float randomized3 = dist(randomRunTime);
        float randomized4 = dist(randomRunTime);
        float randomized5 = dist(randomRunTime);

        Vector2 rupeePos = {random5, -90}; // Initializer for items in game 
        UpdateMusicStream(music);

        // raylib::Vector2 velocity = {linkPos.x * speed, linkPos.y * speed *2};

        rupeePos1.y += gravity * .6; // RED
        rupeePos2.y += gravity * 1; // GREEN
        rupeePos3.y += gravity * .1; // GOLD
        rupeePos4.y += gravity * .9; // YELLOW 
        rupeePos5.y += gravity * .8; // BLUE 

        // Stop rupees when they reach y linkPos 885
        if (rupeePos1.y >= 875) rupeePos1.y = 880;
        if (rupeePos2.y >= 875) rupeePos2.y = 880;
        if (rupeePos3.y >= 875) rupeePos3.y = 880;
        if (rupeePos4.y >= 875) rupeePos4.y = 880;
        if (rupeePos5.y >= 875) rupeePos5.y = 880;

        // Reset rupee linkPoss if they are ±5 away from the initial linkPos
        // Check and reset rupee linkPos individually
        // RED
        if (((rupeePos1.y - linkPos.y) < 200 && (rupeePos1.y - linkPos.y) > 105) && ((rupeePos1.x - linkPos.x) < 160 && (rupeePos1.x - linkPos.x) > 90)) {
            rupeePos1 = {randomized1, -90};
            counter += 20;
        } 
        // GREEN
        if (((rupeePos2.y - linkPos.y) < 200 && (rupeePos2.y - linkPos.y) > 105) && ((rupeePos2.x - linkPos.x) < 160 && (rupeePos2.x - linkPos.x) > 90)) {
            rupeePos2 = {randomized2, -90};
            counter += 1;
        }
        // GOLD
        if (((rupeePos3.y - linkPos.y) < 200 && (rupeePos3.y - linkPos.y) > 105) && ((rupeePos3.x - linkPos.x) < 160 && (rupeePos3.x - linkPos.x) > 90)) {
            rupeePos3 = {randomized3, -90};
            counter += 100;
        }
        // YELLOW
        if (((rupeePos4.y - linkPos.y) < 200 && (rupeePos4.y - linkPos.y) > 105) && ((rupeePos4.x - linkPos.x) < 160 && (rupeePos4.x - linkPos.x) > 90)) {
            rupeePos4 = {randomized4, -90};
            counter += 10;
        }
        // BLUE
        if (((rupeePos5.y - linkPos.y) < 200 && (rupeePos5.y - linkPos.y) > 105) && ((rupeePos5.x - linkPos.x) < 160 && (rupeePos5.x - linkPos.x) > 90)) {
            rupeePos5 = {randomized5, -90};
            counter += 5;
        }

        // Wall boundary 
        if (linkPos.x > 1775) {// Right boundary
            PlaySound(naviWatch);
            linkPos.x = 1774;
        } 
        else if (linkPos.x < -190) {// Left boundary
            PlaySound(naviWatch);
            linkPos.x = -189; 
        }
        
        
        window.BeginDrawing();
        {   

            
            window.ClearBackground(GREEN);
            // Background and foreground for link
            DrawTextureEx(wallpaper, (Vector2){0, -150}, 0, 2, WHITE);
            DrawTextureEx(foreground, (Vector2){ - 50, 70}, 0, .5, WHITE);
            DrawTextureEx(foreground, (Vector2){ 600, 70}, 0, .5, WHITE);
            text.Draw(title, (width / 2.5), height * .15, textSize * 1, raylib::Color::Black());
            text.Draw(title1, (width / 2.5), height * .18, textSize * 3.25, raylib::Color::Black());
            // Character creation
            // Rectangle linkSource = (Rectangle){0, 0, 350, 467};
            // Rectangle linkDest = (Rectangle){static_cast<float>(width / 2), 658, linkSource.width, linkSource.height};
            // DrawTexturePro(characterLink, linkSource, linkDest, linkPos, 0, WHITE);
            DrawTextureEx(characterLink, linkPos, 0, 1, WHITE);

            // Rupee Creation
            DrawTextureEx(Rupee, rupeePos1, 0, .5, RED);
            DrawTextureEx(Rupee, rupeePos2, 0, .5, GREEN);
            DrawTextureEx(Rupee, rupeePos3, 0, .5, GOLD);
            DrawTextureEx(Rupee, rupeePos4, 0, .5, YELLOW);
            DrawTextureEx(Rupee, rupeePos5, 0, .5, DARKBLUE);

            if (IsKeyDown(KEY_SPACE)) {gravity = 0;}
            if (IsKeyDown(KEY_LEFT_SHIFT)) {gravity = 4.9;}
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {speed = 10;} 
            else speed = 1;
            if (IsKeyDown(KEY_D)) {linkPos.x += 12 * speed;}
            if (IsKeyDown(KEY_A)) {linkPos.x -= 12 * speed;}
            if (IsKeyPressed(KEY_W) && !isJumping) {
                // Start jumping only if not already jumping
                isJumping = true;
                PlaySound(linkGrunt);
            }
            // Jump physics
            if (isJumping) {
                linkPos.y -= 2.5;
            } 
            else if (linkPos.y <= 682 && !isJumping) {
                linkPos.y += 4;
            }
            // Check if the character has landed
            if (linkPos.y <= 535) {
                isJumping = false;
            } else if (linkPos.y >= 682) {
                linkPos.y = 682;
                isJumping = false;
            }
            
            // Draw UI elements
            DrawFPS(10, 10);
            text.Draw(totalRupeesTitle, (width / 100), height * .91, textSize * 3.0, raylib::Color::RayWhite());
            text.Draw(std::to_string(counter), (width / 3.6), height * .91, textSize * 3.25, raylib::Color::White());
            // text.Draw(std::to_string(linkPos.y), (width / 1.5), height * .15, textSize * 1.25, raylib::Color::Black());
            // text.Draw(std::to_string(linkPos.x), (width / 2.5), height * .15, textSize * 1.25, raylib::Color::Black());
            // text.Draw(std::to_string((rupeePos1.x - linkPos.x)), (width / 1.5), height * .5, textSize * 1.25, raylib::Color::Black());
            // text.Draw(std::to_string((rupeePos1.y - linkPos.y)), (width / 2.5), height * .5, textSize * 1.25, raylib::Color::Black());
            

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
