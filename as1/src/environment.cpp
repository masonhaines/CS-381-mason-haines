#include "raylib-cpp.hpp"
#include "rlgl.h"
#include "skybox.hpp"

// Some object that can be used as a function, and takes a transform in, and returns a transform 
template<typename T>
concept Transformer = requires( T t, raylib:: Transform m) {
    {t.operator()(m) } -> std::convertible_to<raylib::Transform>;
};

void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Transform backupTransform = model.transform; // save the models transform 
    model.transform = transformer(backupTransform); // update the models transform based on our tranform function
    model.Draw({});
    model.GetTransformedBoundingBox().Draw();
    model.transform = backupTransform;
}

int main() {

    raylib::Window window(800, 800, "CS381 - Assignment 1");//initialize window 
    
    // raylib::Model bad("bad.obj");

    //***************************************************//
    raylib::Camera camera(
        raylib::Vector3(3, 5, 15),
        raylib::Vector3(0, 0, 0),
        raylib::Vector3::Up(), 
        45, 
        CAMERA_PERSPECTIVE
    );
    //**************************************************//
    raylib::Text text;//text object 

    cs381::SkyBox skyBox("../textures/skybox.png");
    
    // raylib::Model defaultCube("bad.obj");

    // raylib::Mesh UploadMesh

    raylib::Model oilTanker = LoadModel("../meshes/OilTanker.glb");
    // oilTanker.Load("../meshes/OilTanker.glb");

    Texture2D texture = LoadTexture("../textures/skybox.png");
    oilTanker.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;


    // float speed = 10;
    // Vector3 position = {0, 0, 0};
    
    while(!window.ShouldClose()){

        int height = window.GetHeight();
        int width = window.GetWidth();

        window.BeginDrawing();//begin drawing inside window 
        {
            // window.ClearBackground(PINK);
            // text.Draw("Hello World", 0, 100, 20, raylib::Color::DarkPurple());
        
            camera.BeginMode();
            {
                // DrawBoundedModel(oilTanker, [](raylib::Transform t) -> raylib::Transform {
                //     return t.Translate({0, 1, 0}).RotateZ(raylib::Degree(45));
                // });
                DrawModel(oilTanker, Vector3{0,5,0}, 5, WHITE);
                


                // skyBox.Draw();
                // defaultCube.Draw({0, 0, 0});
            }
            camera.EndMode();
            
            // position += raylib::Vector3::Right() * speed * window.GetFrameTime();
        }
        window.EndDrawing();

    }
    UnloadTexture(texture);
    UnloadModel(oilTanker);
};


    