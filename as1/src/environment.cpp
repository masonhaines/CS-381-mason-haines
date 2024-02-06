#include "raylib-cpp.hpp"
#include "rlgl.h"


int main() {

    raylib::Window window(800, 800, "CS381 - Assignment 1");//initialize window 
    
    raylib::Model bad("bad.obj");
    


    //***************************************************//
    raylib::Camera camera(
        // raylib::Vector3(0, 120, -500),
        // raylib::Vector3(0, 0, 300), 
        raylib::Vector3(0, 0, -10),
        raylib::Vector3(0, 0, 0),
        raylib::Vector3::Up(), 
        45, 
        CAMERA_PERSPECTIVE
    );
    //**************************************************//
    raylib::Text text;//text object 

    // skyBox skyBox;
    // skyBox.Load("../textures/skybox.png");
    
    raylib::Model defaultCube("bad.obj");

    float speed = 10;
    Vector3 position = {0, 0, 0};
    
    while(!window.ShouldClose()){

        int height = window.GetHeight();
        int width = window.GetWidth();

        window.BeginDrawing();//begin drawing inside window 
        {
            window.ClearBackground(RAYWHITE);
            text.Draw("Hello World", 0, 100, 20, raylib::Color::DarkPurple());
        
            // camera.BeginMode();
            // {
            //     // defaultCube.Draw({0, 0, 0});
            //     // skyBox.Draw();
            // }
            // camera.EndMode();
            
            // position += raylib::Vector3::Right() * speed * window.GetFrameTime();
        }
        window.EndDrawing();

    }
};


    