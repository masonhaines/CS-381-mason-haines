#include "raylib-cpp.hpp"

int main() {
    raylib::Window window(800, 500, "CS381 - assignment 0");

    raylib::Text text;
    text.SetText("Hello Everyone");
    
    while(!window.ShouldClose())
    {
        window.BeginDrawing();
        {
            window.ClearBackground(RAYWHITE);
            // window.GetPosition();
            text.Draw("Hello Class", 400, 250, 20, raylib::Color::Black());
        }
        window.EndDrawing();

    }
};