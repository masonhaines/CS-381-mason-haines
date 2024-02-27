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

int main() {
    // Make window resizable
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    int monitor = GetCurrentMonitor();
    raylib::Window window(GetMonitorWidth(monitor), GetMonitorHeight(monitor), "CS381 - Assignment 3");//initialize window 
    

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
    //***************************************************//
    // // Initialize text object
    raylib::Text text;
    float textSize = 25;
    const char *start = {"Press LEFT on your MOUSE to begin flying "};
    const char *brake = {"Press SPACE-BAR to reset plane velocity "};
    const char *tab = {"Press TAB to switch planes"};
    const char *reset = {"Press P to reset all planes locations"};
    // const char *shift = {"Press SHIFT to accelerate"};
    const char *velo = {"Speed in Z-axis:"};
    const char *camChange = {"Press C to change camera perspective"};
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
    float speed, speed1, speed2, speed3 = 0; 
    float UpDown, UpDown1, UpDown2, UpDown3 = 0;

    float TopVelocity = 40;
    raylib::Vector3 controlPOS = {0, 3.1, 0}; // for changing control to the bounded model 
    raylib::Degree controlY = 0;
    raylib::Degree controlX = 0;
    int planeIndex = 1;

    raylib::Model PolyPlane("meshes/PolyPlane.glb");
    raylib::Vector3 position1 = {0, 3.1, 0};
    raylib::Degree Yaxis1 = 180;
    raylib::Degree Xaxis1 = 0;
    
    raylib::Model PolyPlane2("meshes/PolyPlane.glb");
    raylib::Vector3 position2 = {100 , 3.1, 50};
    raylib::Degree Yaxis2 = 180;
    raylib::Degree Xaxis2 = 0;

    raylib::Model PolyPlane3("meshes/PolyPlane.glb");
    raylib::Vector3 position3 = {-100 , 3.1, -50};
    raylib::Degree Yaxis3 = 180;
    raylib::Degree Xaxis3 = 0;

    bool changeCamera = false;

    //***************************************************//
    while (!window.ShouldClose()) {
        int height = window.GetHeight();
        int width = window.GetWidth();

        window.BeginDrawing();
        {   
            // Clear the window background with a blue color
            window.ClearBackground(raylib::Color::Blue());

            // plane velocities 
            raylib::Vector3 velocity1 = {speed1 * cos(Yaxis1.RadianValue()), UpDown1, -speed1 * sin(Yaxis1.RadianValue())};
            raylib::Vector3 velocity2 = {speed2 * cos(Yaxis2.RadianValue()), UpDown2, -speed2 * sin(Yaxis2.RadianValue())};
            raylib::Vector3 velocity3 = {speed3 * cos(Yaxis3.RadianValue()), UpDown3, -speed3 * sin(Yaxis3.RadianValue())};
            raylib::Vector3 controlVelo = {speed * cos(controlY.RadianValue()), UpDown, -speed * sin(controlY.RadianValue())};

            if (IsKeyReleased(KEY_C)) {
                if (changeCamera == false) changeCamera = true;
                else changeCamera = false;
            }

            if (IsKeyReleased(KEY_P)) {
                raylib::Vector3 RESETPlane = {0, 3.1, 0};
                raylib::Vector3 RESETplane2 = {100 , 3.1, 50};
                raylib::Vector3 RESETplane3 = {-100, 0, -50};
                position1 = RESETPlane;
                position2 = RESETplane2;
                position3 = RESETplane3;
            }

            if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
                speed = 15;
                speed1 = 10;
                speed2 = 10;
                speed3 = 10;
            } else if (IsKeyDown(KEY_SPACE)) {
                raylib::Vector3 RESETvelocity = {0, 0, 0};
                controlVelo = RESETvelocity;
            } 
            else if (IsKeyPressed(KEY_TAB)) {
                planeIndex += 1;
                if (planeIndex > 3) {
                    planeIndex = 1;
                }
                if (planeIndex == 2) {
                    controlVelo = velocity1;
                    controlPOS = position1;
                    controlX = Xaxis1;
                    controlY = Yaxis1;
                }
                if (planeIndex == 2) {
                    controlVelo = velocity2;
                    controlPOS = position2;
                    controlX = Xaxis2;
                    controlY = Yaxis2;
                }
                if (planeIndex == 3) {
                    controlVelo = velocity3;
                    controlPOS = position3;
                    controlX = Xaxis3;
                    controlY = Yaxis3;
                }
            }

            // Ensure the Y-axis rotation is within the range of 0 to 360 degrees
            while(controlY > 360 || controlY < 0) {
                if (controlY > 360 ) {
                    controlY -= 360;
                }
                if (controlY < 0 ) {
                    controlY += 360;
                }
            }

            // Begin camera
            camera.BeginMode();
            {   
                if (changeCamera == true) {
                    // Update camera position and target to follow the model
                    camera.target = controlPOS;
                    int intYaxis = controlY;
                    intYaxis %= 360;

                    // Adjust camera position based on the Y-axis rotation
                    if (controlPOS.z < -400){
                        camera.position = {0, 120, 500};
                    } else {
                        camera.position = {0, 120, -500};  
                    }
                } 
                
                // Draw skybox, ground, and models
                skybox.Draw();
                ground.Draw({});

                if (planeIndex == 1) {
                    // Draw the plane model
                    DrawBoundedModel(PolyPlane3, [&controlPOS, &controlY, &controlX](raylib::Transform t) -> raylib::Transform {
                        return t.Translate(controlPOS).RotateY(controlY).RotateX(controlX).Scale(1, 2, 1);
                    });
                    DrawModel(PolyPlane2, [&position2, &Yaxis2, &Xaxis2](raylib::Transform t) -> raylib::Transform {
                        return t.Translate(position2).RotateY(Yaxis2).RotateX(Xaxis2).Scale(1, 2, 1);
                    });
                    DrawModel(PolyPlane3, [&position3, &Yaxis3, &Xaxis3](raylib::Transform t) -> raylib::Transform {
                        return t.Translate(position3).RotateY(Yaxis3).RotateX(Xaxis3).Scale(1, 2, 1);
                    });
                    // velocity1 = controlVelo;
                    // controlPOS = position1;
                    // Xaxis1 = controlX;
                    // Yaxis1 = controlY;

                    // controlVelo = velocity1;
                    // controlPOS = position1;
                    // controlX = Xaxis1;
                    // controlY = Yaxis1;
                    
                } else if (planeIndex == 2) {
                    // Draw the plane model
        
                    DrawModel(PolyPlane, [&position1, &Yaxis1, &Xaxis1](raylib::Transform t) -> raylib::Transform {
                        return t.Translate(position1).RotateY(Yaxis1).RotateX(Xaxis1).Scale(1, 2, 1);
                    });
                    DrawBoundedModel(PolyPlane3, [&controlPOS, &controlY, &controlX](raylib::Transform t) -> raylib::Transform {
                        return t.Translate(controlPOS).RotateY(controlY).RotateX(controlX).Scale(1, 2, 1);
                    });
                    DrawModel(PolyPlane3, [&position3, &Yaxis3, &Xaxis3](raylib::Transform t) -> raylib::Transform {
                        return t.Translate(position3).RotateY(Yaxis3).RotateX(Xaxis3).Scale(1, 2, 1);
                    });
                    // velocity2 = controlVelo;
                    // controlPOS = position2;
                    // Xaxis2 = controlX;
                    // Yaxis2 = controlY;

                    // controlVelo = velocity2;
                    // controlPOS = position2;
                    // controlX = Xaxis2;
                    // controlY = Yaxis2;
                } else if (planeIndex == 3) {
                    // Draw the plane model
                    
                    DrawModel(PolyPlane, [&position1, &Yaxis1, &Xaxis1](raylib::Transform t) -> raylib::Transform {
                        return t.Translate(position1).RotateY(Yaxis1).RotateX(Xaxis1).Scale(1, 2, 1);
                    });
                    DrawModel(PolyPlane2, [&position2, &Yaxis2, &Xaxis2](raylib::Transform t) -> raylib::Transform {
                        return t.Translate(position2).RotateY(Yaxis2).RotateX(Xaxis2).Scale(1, 2, 1);
                    });
                    DrawBoundedModel(PolyPlane3, [&controlPOS, &controlY, &controlX](raylib::Transform t) -> raylib::Transform {
                        return t.Translate(controlPOS).RotateY(controlY).RotateX(controlX).Scale(1, 2, 1);
                    });
                    // velocity3 = controlVelo;
                    // controlPOS = position3;
                    // Xaxis3 = controlX;
                    // Yaxis3 = controlY;

                    // controlVelo = velocity3;
                    // controlPOS = position3;
                    // controlX = Xaxis3;
                    // controlY = Yaxis3;
                }
            }
            // End camera
            camera.EndMode();

            // Check collision with the ground for each plane
            if (planeIndex == 1 && position1.y <= 3.1) {
                position1.y = 3.1;  // Prevent plane from going through the ground
            } else if (planeIndex == 2 && position2.y <= 3.1) {
                position2.y = 3.1;  // Prevent plane from going through the ground
            } else if (planeIndex == 3 && position3.y <= 3.1) {
                position3.y = 3.1;  // Prevent plane from going through the ground
            }
            // Check collision with the wall for each plane
            if (planeIndex == 1) {
                if (position1.x > 500) position1.x = 500; // Right boundary
                else if (position1.x < -500) position1.x = -500; // Left boundary
                if (position1.z > 500) position1.z = 500; // Top boundary
                else if (position1.z < -500) position1.z = -500; // Bottom boundary
            } else if (planeIndex == 2) {
                if (position2.x > 500) position2.x = 500;
                else if (position2.x < -500) position2.x = -500;
                if (position2.z > 500) position2.z = 500;
                else if (position2.z < -500) position2.z = -500;
            } else if (planeIndex == 3) {
                if (position3.x > 500) position3.x = 500;
                else if (position3.x < -500) position3.x = -500;
                if (position3.z > 500) position3.z = 500;
                else if (position3.z < -500) position3.z = -500;
            }
           
            // Control plane movement along different axes
            if(IsKeyPressed(KEY_Q)){
                if (controlX != -25) {
                    controlX -= 5;
                }

                UpDown += 5;
            }
            if(IsKeyPressed(KEY_E)){
                if (controlX != 25) {
                    controlX += 5; 
                }

                UpDown -= 5;
            }
            //**************************************************************//
            if (IsKeyPressed(KEY_W)) {
                speed += 5;
            }
            if (IsKeyPressed(KEY_S)) {
                speed -= 3;
            }
            if (IsKeyPressed(KEY_A)) {
                controlY += 10;
            }
            if (IsKeyPressed(KEY_D)) {
                controlY -= 10;
            }
           
            controlPOS += (velocity1) * window.GetFrameTime();
            position1 += (velocity1) * window.GetFrameTime();
            position2 += (velocity2) * window.GetFrameTime();
            position3 += (velocity3) * window.GetFrameTime();
            
            // Draw UI elements
            DrawFPS(10, 10);
            text.Draw(start, (width / 2.5), height * .15, textSize * 1.25, raylib::Color::Black());
            text.Draw(brake, (width * .005), height * .15, textSize * .75, raylib::Color::Black());
            text.Draw(tab, (width * .005), height * .2, textSize * .75, raylib::Color::Black());
            text.Draw(camChange, (width * .005), height * .25, textSize * .75, raylib::Color::Black());
            text.Draw(velo, (width * .005), height * .1, textSize, raylib::Color::Black());
            text.Draw(std::to_string(controlVelo.z), (width * .15), height * .1 , textSize , raylib::Color::Black());
        }

        window.EndDrawing();

    } // end while loop
}