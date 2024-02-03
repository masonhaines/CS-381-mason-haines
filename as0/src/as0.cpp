#include "raylib-cpp.hpp"
#include <string>
#include <iostream>

using namespace std;

//function prototype
void textFun (int& red, int& green, int& blue);//passing by reference 

//make 2D texture object
Texture2D mask;
Texture2D rupee;

int main() {

    //make window resizable
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    raylib::Window window(800, 800, "CS381 - Assignment 0");//initialize window 

    raylib::Color colors;//colors object 
    raylib::Text text;//text object 

    //load image as texture into texture object mask 
    mask = LoadTexture("../majorasMask.png");
    rupee = LoadTexture("../green_rupee_small.png");

    //create RGBA values and initialize 
    int red = 0;
    int green = 0;
    int blue = 0; 
    int alpha = 255;
    
    float textSize = 1;//TEXT SIZE 
    const char *Label = {"Oi! ZELDA! Get out of my sandwich!"};

    //variable to change image POS
    int BOUNCE = 0;
    bool bounceFlag = true;

    while(!window.ShouldClose()){

        int height = window.GetHeight();
        int width = window.GetWidth();

        /**************************************************/
        //This is to make those rupees bounce!!
        if (bounceFlag) {
            BOUNCE++;
        } else {
            BOUNCE--;
        }

        if (BOUNCE == 250) {
            bounceFlag = false;
        } else if (BOUNCE == -250) {
            bounceFlag = true;
        }
        /**************************************************/

        //increment text size and and RGB values 
        red += 4;
        green += 1;
        blue += 11;
        textSize += .025;
        //reset text size as it gets to 30 
        if (textSize >= 45) {
            textSize = 10;
        }
       
        textFun(red, green, blue);//check if RGB values need to be reset

        window.BeginDrawing();//begin drawing inside window 
        {
            //as window size is adjusted the colors of the window will change 
            if (width % 2 != 0 || height % 2 != 0) {
                window.ClearBackground(colors.Black());
            } else if (width % 3 != 0 || height % 3 != 0){
                window.ClearBackground(colors.DarkPurple());
            } else if (width % 4 != 0 || height % 4 != 0){
                window.ClearBackground(colors.Red());
            }
            
            //make a bouncing rupee
            Rectangle rupeeSource = (Rectangle){0, 0, 200, 200};
            // I dont like to wrap gifts or functions correctly //
            /**/Rectangle rupeeDest = (Rectangle){static_cast<float>(width / 2) - (rupeeSource.width) / 2, 
            /**/static_cast<float>(height / 2) - ((rupeeSource.height) / 10), 
            /**/rupeeSource.width, rupeeSource.height};
            DrawTexturePro(rupee, rupeeSource, rupeeDest, (Vector2){BOUNCE, 0}, 0, WHITE);

            //Make a rotating majoras mask 
            Rectangle maskSource = (Rectangle){0, 0, 675, 593};
            Rectangle maskDest = (Rectangle){static_cast<float>(width / 2), static_cast<float>(height / 2), maskSource.width, maskSource.height};
            DrawTexturePro(mask, maskSource, maskDest, (Vector2){0, 0}, 45 * GetTime(), RED);

            //Create a text
            //gets larger over time based off of textsize incrementing and RGB values are changing colors
            text.Draw(Label, ((width / 2) - (MeasureText(Label, textSize) / 2)), height / 2, textSize, raylib::Color{red,green,blue,alpha});
        }

        window.EndDrawing();
    }
};

//function made to change colors of the text that is using RGB colors 
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
    
    