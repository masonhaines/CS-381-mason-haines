#include "raylib-cpp.hpp"
#include <string>
#include <iostream>

using namespace std;

void textFun (int& red, int& green, int& blue);

int main() {

    raylib::Window window(800, 800, "CS381 - Assignment 0");
    raylib::Color colors;
    raylib::Text text;

    int red = 0;
    int green = 0;
    int blue = 0; 
    int alpha = 255;

    float textSize = 1;
    const char *Label = {"I'm coming to get yah!"};

    
    
    while(!window.ShouldClose()){

        int height = window.GetHeight();
        int width = window.GetWidth();

        red += 3;
        green += 3;
        blue += 3;
        textSize += .05;
        
        // if (textSize >= 10) {
        //     textSize += 1;
        // }
        if (textSize >= 30) {
            textSize = 10;
        }
       
        textFun(red, green, blue);

        window.BeginDrawing();
        {
            if (width % 2 != 0 || height % 2 != 0) {
                window.ClearBackground(colors.Yellow());
            }else {
                window.ClearBackground(colors.Green());
            }
            
            text.Draw(Label, ((width * .5) - (MeasureText(Label, textSize) * .5)), .5 * height, textSize, raylib::Color{red,green,blue,alpha});
            // const Vector2 &position, raylib::Radian rotation, const Vector2 &origin = {(0), (0)}) const
            // text.Draw((1,1), raylib::Radian(2.75), {(0), (0)});
            // text.Draw()
            
        }

        window.EndDrawing();
    }
};

void textFun (int& red, int& green, int& blue){
    if (red == 245) {
        red = 0;
    }
    else if (green == 245) {
        green = 0;
    }
    else if (blue = 245) {
        blue = 0;
    }
}