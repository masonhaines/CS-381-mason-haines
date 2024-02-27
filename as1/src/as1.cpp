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
    model.GetTransformedBoundingBox().Draw();
    model.transform = backupTransform;
}

int main() {

    //make window resizable
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    raylib::Window window(1400, 800, "CS381 - Assignment 1");//initialize window 

    //***************************************************//
    raylib::Camera camera(
        raylib::Vector3(0, 10, -50),
        // raylib::Vector3(0, 0, 300),
        // raylib::Vector3(4, 15, 45),
        raylib::Vector3(0, 0, 0),
        raylib::Vector3::Up(), 
        45, 
        CAMERA_PERSPECTIVE
    );
    //**************************************************//
    
    raylib::Model PolyPlane("meshes/PolyPlane.glb");
    raylib::Model tugBoat("meshes/SmitHouston_Tug.glb");
    raylib::Model car{"meshes/Doughnut.glb"};
    // raylib::Mesh model = LoadModelFromMesh("meshes/Doughnut.glb");
    while (!window.ShouldClose()) {

        

        window.BeginDrawing();//begin drawing inside window 
        {
            camera.BeginMode();
            {   
                window.ClearBackground(raylib::Color::Blue());
              
                // // [capture list](parameters) -> return_type { function_body }
                // DrawBoundedModel(PolyPlane, [](raylib::Transform t) -> raylib::Transform {
                //     return t.Translate({0, 0, 0});
                // });
                // DrawBoundedModel(PolyPlane, [](raylib::Transform t) -> raylib::Transform {
                //     return t.Translate({-100, 100, 0}).RotateY(180).Scale(1, -1, 1);
                // });
                // DrawBoundedModel(tugBoat, [](raylib::Transform t) -> raylib::Transform {
                //     return t.Translate({-200, 0, 0});
                // });
                // DrawBoundedModel(tugBoat, [](raylib::Transform t) -> raylib::Transform {
                //     return t.Translate({200, 0, 0}).RotateY(90);
                // });
                // DrawBoundedModel(tugBoat, [](raylib::Transform t) -> raylib::Transform {
                //     return t.Translate({100, 100, 0}).RotateY(270).Scale(1, 2, 1);
                // });
                // DrawBoundedModel(tugBoat, [](raylib::Transform t) -> raylib::Transform {
                //     return t.Translate({100, 100, 0}).RotateY(270).Scale(1, 2, 1);
                // });

                // // Draw the plane model
                DrawBoundedModel(car, [](raylib::Transform t) -> raylib::Transform {
                    return t.Translate({0, 0, 0}).RotateY(0).Scale(1, 1, 1);
                });
                // //barrel attempt

                // DrawModel(model, {0, 0, 0}, 5, BLUE);



            }
            camera.EndMode();

            DrawFPS(10, 10);
            
        }
        window.EndDrawing();
    }
};


    
    