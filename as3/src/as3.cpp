#include "raylib-cpp.hpp"
#include "rlgl.h"
#include "skybox.hpp"
#include "raylib.h"

// Concept for a Transformer that transforms models
template<typename T>
concept Transformer = requires( T t, raylib::Transform m) {
    {t.operator()(m)}->std::convertible_to<raylib::Transform>;
};

// Function to draw a bounded model with a specified transformation
void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Transform backupTransform = model.transform; // Save the model's transform
    model.transform = transformer(backupTransform); // Update the model's transform based on the transformation function
    model.Draw({});
    model.GetTransformedBoundingBox().Draw();
    model.transform = backupTransform; // Restore the original transform
}

// Function to draw a model with a specified transformation
void DrawModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Transform backupTransform = model.transform; // Save the model's transform
    model.transform = transformer(backupTransform); // Update the model's transform based on the transformation function
    model.Draw({});
    model.transform = backupTransform; // Restore the original transform
}

// void boundModel(raylib::Model& model, Vector3 pos, raylib::Degree y, raylib::Degree x) {
//     model.GetTransformedBoundingBox().Draw();
//     DrawBoundedModel(model, [&pos, &y, &x](raylib::Transform t) -> raylib::Transform {
//         return t.Translate(pos).RotateY(y).RotateX(x).Scale(2, 5, 2);
//     });
// }

void controls() {
    
}

int main() {
    // Make window resizable
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    int monitor = GetCurrentMonitor();
    raylib::Window window(GetMonitorWidth(monitor), GetMonitorHeight(monitor), "CS381 - Assignment 3");//initialize window 

    // Initialize audio
    InitAudioDevice();
    Music music = LoadMusicStream("../metroid.wav");
    PlayMusicStream(music);

    //***************************************************//
    // Initialize camera
    raylib::Camera camera(
        raylib::Vector3(0, 120, -500),
        raylib::Vector3(0, 0, 0),
        raylib::Vector3::Up(), 
        45, 
        CAMERA_PERSPECTIVE
    );
    //***************************************************//
    // Initialize skybox and models
    cs381::SkyBox skybox{"textures/skybox.png"};
    raylib::Model PolyPlane("meshes/PolyPlane.glb");
    raylib::Model PolyPlane2("meshes/PolyPlane.glb");
    raylib::Model PolyPlane3("meshes/PolyPlane.glb");
    //***************************************************//
    // Initialize text object
    raylib::Text text;
    float textSize = 25;
    const char *start = {"Press LEFT on your MOUSE to begin flying "};
    const char *brake = {"Press SPACE-BAR to reset plane velocity "};
    const char *shift = {"Press and hold LEFT-SHIFT to accelerate"};
    const char *reset = {"Press P to reset all planes locations"};
    const char *velo = {"Speed:"};
    //***************************************************//
    // Initialize ground model
    auto mesh = raylib::Mesh::Plane(10'000, 10'000, 50, 50, 25);
    raylib::Model ground = ((raylib::Mesh*)&mesh)->LoadModelFrom();
    raylib::Texture grass("textures/water.jpg");
    grass.SetFilter(TEXTURE_FILTER_BILINEAR);
    grass.SetWrap(TEXTURE_WRAP_REPEAT);
    ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grass;
    //***************************************************//
    // Initialize speed and positions
    float speed = 0; 
    raylib::Vector3 position = {0, 3.1, 0};
    raylib::Vector3 positionTwo = {100 , 3.1, 50};
    raylib::Vector3 positionThree= {-100 , 3.1, -50};
    raylib::Degree Yaxis = 180;
    raylib::Degree Xaxis = 0;
    int planeIndex = 1;

    //***************************************************//
    while (!window.ShouldClose()) {
        int height = window.GetHeight();
        int width = window.GetWidth();

        UpdateMusicStream(music);

        window.BeginDrawing();
        {   
            // Clear the window background with a blue color
            window.ClearBackground(raylib::Color::Blue());

            // Control plane movement
            raylib::Vector3 velocity = {speed * cos(Yaxis.RadianValue()), 0, -speed * sin(Yaxis.RadianValue())};
            raylib::Vector3 controlPOS = {0, 0, 0};

            // Y-axis rotation is within the range of 0 to 360 degrees
            while(Yaxis > 360 || Yaxis < 0) {
                if (Yaxis > 360 ) {
                    Yaxis -= 360;
                }
                if (Yaxis < 0 ) {
                    Yaxis += 360;
                }
            }

            // integer and fractional parts of the Y-axis rotation
            int intYaxis = Yaxis;
            float floatYaxis = float(Yaxis) - intYaxis;
            intYaxis %= 360;

            if (IsKeyPressed(KEY_TAB)) {
                planeIndex += 1;
                if (planeIndex > 3) {
                    planeIndex = 1;
                }
            }

            // Begin camera
            camera.BeginMode();
            {   
                // // Update camera position and target to follow the model
                // camera.target = position;
                // // Adjust camera position based on the Y-axis rotation
                // if (intYaxis > 0 && intYaxis < 180){
                //     camera.position = {position.x, position.y + 120, position.z + 500};
                // } else {
                //     camera.position = {position.x, position.y + 120, position.z - 500};  
                // }

                // Draw skybox, ground, and models
                skybox.Draw();
                ground.Draw({});

                if (planeIndex == 1) {
                    // Draw the plane model
                    DrawBoundedModel(PolyPlane, [&controlPOS, &Yaxis, &Xaxis](raylib::Transform t) -> raylib::Transform {
                        return t.Translate(controlPOS).RotateY(Yaxis).RotateX(Xaxis).Scale(1, 2, 1);
                    });
                    DrawModel(PolyPlane2, [&positionTwo, &Yaxis, &Xaxis](raylib::Transform t) -> raylib::Transform {
                        return t.Translate(positionTwo).RotateY(Yaxis).RotateX(Xaxis).Scale(1, 2, 1);
                    });
                    DrawModel(PolyPlane3, [&positionThree, &Yaxis, &Xaxis](raylib::Transform t) -> raylib::Transform {
                        return t.Translate(positionThree).RotateY(Yaxis).RotateX(Xaxis).Scale(1, 2, 1);
                    });
                } else if (planeIndex == 2) {
                    // Draw the plane model
                    DrawModel(PolyPlane, [&position, &Yaxis, &Xaxis](raylib::Transform t) -> raylib::Transform {
                        return t.Translate(position).RotateY(Yaxis).RotateX(Xaxis).Scale(1, 2, 1);
                    });
                    DrawBoundedModel(PolyPlane2, [&controlPOS, &Yaxis, &Xaxis](raylib::Transform t) -> raylib::Transform {
                        return t.Translate(controlPOS).RotateY(Yaxis).RotateX(Xaxis).Scale(1, 2, 1);
                    });
                    DrawModel(PolyPlane3, [&positionThree, &Yaxis, &Xaxis](raylib::Transform t) -> raylib::Transform {
                        return t.Translate(positionThree).RotateY(Yaxis).RotateX(Xaxis).Scale(1, 2, 1);
                    });
                } else if (planeIndex == 3) {
                    // Draw the plane model
                    DrawModel(PolyPlane, [&position, &Yaxis, &Xaxis](raylib::Transform t) -> raylib::Transform {
                        return t.Translate(position).RotateY(Yaxis).RotateX(Xaxis).Scale(1, 2, 1);
                    });
                    DrawModel(PolyPlane2, [&positionTwo, &Yaxis, &Xaxis](raylib::Transform t) -> raylib::Transform {
                        return t.Translate(positionTwo).RotateY(Yaxis).RotateX(Xaxis).Scale(1, 2, 1);
                    });
                    DrawBoundedModel(PolyPlane3, [&controlPOS, &Yaxis, &Xaxis](raylib::Transform t) -> raylib::Transform {
                        return t.Translate(controlPOS).RotateY(Yaxis).RotateX(Xaxis).Scale(1, 2, 1);
                    });
                }

                
            }
            // End camera
            camera.EndMode();

            
		
            // Control plane speed and direction
            if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
                speed = 2.5;
            } else if (IsKeyDown(KEY_SPACE)) {
                raylib::Vector3 RESETvelocity = {0, 0, 0};
                velocity = RESETvelocity;
            } else if (speed < 10 && IsKeyDown(KEY_LEFT_SHIFT)) {
                speed += .2;
            }
	
	        if (IsKeyReleased(KEY_P)) {
                raylib::Vector3 RESETPlane = {0, 3.1, 0};
                raylib::Vector3 RESETplane2 = {100 , 3.1, 50};
                raylib::Vector3 RESETplane3 = {-100, 0, -50};
                position = RESETPlane;
                positionTwo = RESETplane2;
                positionThree = RESETplane3;
            }

            // Collision detection with ground
            if (controlPOS.y <= 3) {
                controlPOS.y = 3.1;
            }

            // Control plane movement along different axes
            if(IsKeyDown(KEY_Q)){
                if (Xaxis != -25) {
                    Xaxis -= .5;
                }
                velocity += raylib::Vector3::Up() * speed;
                controlPOS += (velocity) * window.GetFrameTime();
            }
            if(IsKeyDown(KEY_E)){
                if (Xaxis != 25) {
                    Xaxis += .5;
                }
                velocity += raylib::Vector3::Down() * speed;
                controlPOS += (velocity) * window.GetFrameTime();
            }
            if (IsKeyDown(KEY_W)) {
                if (intYaxis > 0 && intYaxis < 180){
                    velocity -= raylib::Vector3::Forward() * speed;
                    controlPOS -= (velocity) * window.GetFrameTime();
                } else {
                    velocity += raylib::Vector3::Forward() * speed;
                    controlPOS += (velocity) * window.GetFrameTime();
                }
            }
            if (IsKeyDown(KEY_A)) {
                velocity += raylib::Vector3::Left() * speed;
                Yaxis += 2;
                controlPOS += (velocity) * window.GetFrameTime();
            }
            if (IsKeyDown(KEY_D)) {
                velocity += raylib::Vector3::Right() * speed;
                Yaxis -= 2;
                controlPOS += (velocity) * window.GetFrameTime();
            }
            if (IsKeyDown(KEY_S)) {
                velocity += raylib::Vector3::Back() * speed;
                controlPOS += (velocity) * window.GetFrameTime();
            }

            // Update controlPOSs and using velocity and speed 
            controlPOS += (velocity * speed) * window.GetFrameTime();
            positionTwo += (velocity * speed) * window.GetFrameTime();
            positionThree += (velocity * speed) * window.GetFrameTime();
            

            // Draw UI elements
            DrawFPS(10, 10);
            text.Draw(start, (width * .005), height * .15, textSize * .75, raylib::Color::Black());
            text.Draw(brake, (width * .005), height * .2, textSize * .75, raylib::Color::Black());
            text.Draw(shift, (width * .005), height * .25, textSize * .75, raylib::Color::Black());
            text.Draw(velo, (width * .005), height * .1, textSize, raylib::Color::Black());
            text.Draw(std::to_string(speed), (width * .15), height * .1 , textSize , raylib::Color::Black());
        }
        window.EndDrawing();

    } // end while loop

    UnloadMusicStream(music);
    CloseAudioDevice(); // Close the audio device
}
