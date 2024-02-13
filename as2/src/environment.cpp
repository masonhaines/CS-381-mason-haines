#include "raylib-cpp.hpp"
#include "rlgl.h"
#include "skybox.hpp"


// Some object that can be used as a function, and takes a transform in, and returns a transform 
template<typename T>
concept Transformer = requires( T t, raylib:: Transform m) {
    {t.operator()(m)}->std::convertible_to<raylib::Transform>;
};

void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Transform backupTransform = model.transform; // save the models transform 
    model.transform = transformer(backupTransform); // update the models transform based on our tranform function
    model.Draw({});
    // model.GetTransformedBoundingBox().Draw();
    model.transform = backupTransform;
}

int main() {

    //make window resizable
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    raylib::Window window(800, 800, "CS381 - Assignment 2");//initialize window 

    //***************************************************//
    raylib::Camera camera(
        raylib::Vector3(0, 120, -500),
        // raylib::Vector3(0, 0, 300),
        // raylib::Vector3(4, 15, 45),
        raylib::Vector3(0, 0, 0),
        raylib::Vector3::Up(), 
        45, 
        CAMERA_PERSPECTIVE
    );
    //**************************************************//

    cs381::SkyBox skybox{"textures/skybox.png"};
    raylib::Model PolyPlane("meshes/PolyPlane.glb");
    raylib::Model tugBoat("meshes/SmitHouston_Tug.glb");

    raylib::Text text;//text object 
    float textSize = 1;//TEXT SIZE 
    const char *Label = {"Press Space to begin and hold space to stop :) "};


    //for ground model
    auto mesh = raylib::Mesh::Plane(10'000, 10'000, 50, 50, 25);
    raylib::Model ground = ((raylib::Mesh*)&mesh)->LoadModelFrom();
    raylib::Texture grass("textures/grass.jpg");
    grass.SetFilter(TEXTURE_FILTER_BILINEAR);
    grass.SetWrap(TEXTURE_WRAP_REPEAT);
    ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grass;

    float speed = 0; // Adjust the speed to your preference
    raylib::Vector3 position = {0, 0, 0};
    raylib::Degree Yaxis = 270;
    raylib::Degree Xaxis = 180;

    while (!window.ShouldClose()) {

        int height = window.GetHeight();
        int width = window.GetWidth();

        window.BeginDrawing();//begin drawing inside window 
        {   
            camera.BeginMode();
            {   
                // skybox.Draw();
                // ground.Draw({});
                window.ClearBackground(raylib::Color::Blue());
              
                
                DrawBoundedModel(PolyPlane, [&position, &Yaxis, &Xaxis](raylib::Transform t) -> raylib::Transform {
                    return t.Translate(position).RotateY(Yaxis).Scale(3, 6, 3);
                });
                

                raylib::Vector3 velocity = {speed * cos(Yaxis.RadianValue()), 0, -speed * sin(Yaxis.RadianValue())};

                if(IsKeyReleased(KEY_SPACE)){
                    speed = 5;
                } else if (IsKeyDown(KEY_SPACE)) {
                    speed = 0;
                }

                
                if(IsKeyDown(KEY_Q)){
                    velocity += raylib::Vector3::Up() * speed;
                    position += (velocity) * window.GetFrameTime();
                }
                if(IsKeyDown(KEY_E)){
                    velocity += raylib::Vector3::Down() * speed;
                    position += (velocity) * window.GetFrameTime();
                }
                if (IsKeyDown(KEY_W)) {
                    // if (Yaxis > 0 && Yaxis < 180){
                    //     velocity += raylib::Vector3::Back() * speed;
                    //     position += (velocity) * window.GetFrameTime();
                    // } else {
                        velocity += raylib::Vector3::Forward() * speed;
                        position += (velocity) * window.GetFrameTime();
                    // }
                    
                }
                if (IsKeyDown(KEY_A)) {
                    velocity += raylib::Vector3::Left() * speed;
                    Yaxis += .5;
                    position += (velocity) * window.GetFrameTime();
                }
                if (IsKeyDown(KEY_D)) {
                    velocity += raylib::Vector3::Right() * speed;
                    Yaxis -= .5;
                    position += (velocity) * window.GetFrameTime();
                }
                if (IsKeyDown(KEY_S)) {
                    velocity += raylib::Vector3::Back() * speed / 5;
                    position += (velocity) * window.GetFrameTime();
                }
                

                position += (velocity * speed) * window.GetFrameTime();
            }
            camera.EndMode();

            DrawFPS(10, 10);
            text.Draw(Label, ((width / 2) - (MeasureText(Label, textSize) / 2)), height / 4, textSize, raylib::Color::Black());
            
        }
        window.EndDrawing();
    }
};


    
    