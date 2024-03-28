// Mason Haines CS 381 3/17
#include <raylib-cpp.hpp>
#include <string>
#include <vector>
#include "rlgl.h"
#include "skybox.hpp"
#include "raylib.h"
#include "entity.hpp"
#include "renderingComponent.hpp"
#include "physicsComponent.hpp"
#include "bufferedInputComponent.hpp"
#include "transformComponent.hpp"
#include <iostream>

int main() {
	// Create window
	const int screenWidth = 900 * 2;
	const int screenHeight = 450 * 2;
	raylib::Window window(screenWidth, screenHeight, "CS381 - Assignment 6");

	raylib::BufferedInput inputs; // Manager for actions 

	// Create vector of entities
	std::vector<Entity> entities;
	int numberOfPlanes = 5;
	int counter = -1;

	// Buffered input actions for setup in buffered input component
	inputs["forward"] = raylib::Action::key(KEY_W).move();
	inputs["backwards"] = raylib::Action::key(KEY_S).move();
	inputs["right"] = raylib::Action::key(KEY_D).move();
	inputs["left"] = raylib::Action::key(KEY_A).move();
	inputs["space"] = raylib::Action::key(KEY_SPACE).move();

	inputs["change"] = raylib::Action::key(KEY_TAB).SetPressedCallback([&entities, &counter](){
		counter = (counter + 1) % entities.size();
		for (int i = 0; i < entities.size(); i++) {
			// cycle through entities vector 
			auto ref = entities[i].GetComponent<bufferedComponent>(); // get optional reference to transform component 
			if (!ref) return; // does it exist 
			auto& INPUT = ref->get(); // get values stored in reference if it exists
			INPUT.selected = counter == i;
		}
	}).move();


	//offset ------- velo ------- speed ------- target speed ------- acceleration ------- ang acc------- min speed ------- max speed 
	//-------------------------------------------------------------------------------------------------------------------------------
	for (float i = 0; i < numberOfPlanes; i++) {
		Entity& entityPlanes = entities.emplace_back(); // Add a new entity to to back of entities vector create reference e to the newly made entity object 

		entityPlanes.GetComponent<TransformComponent>()->get().position.x = i * 50 - 250;
		entityPlanes.GetComponent<TransformComponent>()->get().position.y = 150;
		entityPlanes.GetComponent<TransformComponent>()->get().position.z = i * 50;
		entityPlanes.GetComponent<TransformComponent>()->get().scale = raylib::Vector3(.65, .65, .65);
		entityPlanes.GetComponent<TransformComponent>()->get().rotation = raylib::Vector3(0, 0, 0); 
		entityPlanes.AddComponent<RenderingComponent>(raylib::Model("meshes/PolyPlane.glb"), GRAY);
		entityPlanes.AddComponent<PhysicsComponent>(Vector3{0, 0, 0}, Vector3{0, 0, 0}, 1, 1.0f, 10, 20, 0, 50); 
		entityPlanes.AddComponent<bufferedComponent>(&inputs, false);
	}

	Entity& ship1 = entities.emplace_back(); // Add a new entity to to back of entities vector create reference e to the newly made entity object 
	ship1.GetComponent<TransformComponent>()->get().position.x = -250;
	ship1.GetComponent<TransformComponent>()->get().position.z = -100;
	ship1.GetComponent<TransformComponent>()->get().scale = raylib::Vector3(.5, .5, .5);
	ship1.GetComponent<TransformComponent>()->get().rotation = Vector3{0, 90 * DEG2RAD, 0};
	ship1.AddComponent<RenderingComponent>(raylib::Model("meshes/SmitHouston_Tug.glb"), GRAY);
	ship1.AddComponent<PhysicsComponent>(Vector3{0, 90 * DEG2RAD, 0}, Vector3{0, 0, 0}, 1, 1, 2.5f, 7, 0, 15); 
	ship1.AddComponent<bufferedComponent>(&inputs, false);

			Entity& ship2 = entities.emplace_back(); // Add a new entity to to back of entities vector create reference e to the newly made entity object 
			ship2.GetComponent<TransformComponent>()->get().position.x = -250;
			ship2.GetComponent<TransformComponent>()->get().position.z = -50;
			ship2.GetComponent<TransformComponent>()->get().scale = raylib::Vector3(1.0, 1.0, 1.0);
			ship2.GetComponent<TransformComponent>()->get().rotation = Vector3{0, 90 * DEG2RAD, 0};
			ship2.AddComponent<RenderingComponent>(raylib::Model("meshes/SmitHouston_Tug.glb"), RED);
			ship2.AddComponent<PhysicsComponent>(Vector3{0, 90 * DEG2RAD, 0}, Vector3{0, 0, 0}, 1, 1, 2.0f, 5, 0, 12);  
			ship2.AddComponent<bufferedComponent>(&inputs, false);

	Entity& ship3 = entities.emplace_back(); // Add a new entity to to back of entities vector create reference e to the newly made entity object 
	ship3.GetComponent<TransformComponent>()->get().position.x = -250;
	ship3.GetComponent<TransformComponent>()->get().scale = raylib::Vector3(5, 1.35, 1.35);
	ship3.GetComponent<TransformComponent>()->get().rotation = Vector3{0, 90 * DEG2RAD, 0};
	ship3.AddComponent<RenderingComponent>(raylib::Model("meshes/SmitHouston_Tug.glb"), ORANGE);
	ship3.AddComponent<PhysicsComponent>(Vector3{0, 90 * DEG2RAD, 0}, Vector3{0, 0, 0}, 1, 1, .25f, 2, 0, 10);  
	ship3.AddComponent<bufferedComponent>(&inputs, false);

			Entity& ship4 = entities.emplace_back(); // Add a new entity to to back of entities vector create reference e to the newly made entity object 
			ship4.GetComponent<TransformComponent>()->get().position.x = -250;
			ship4.GetComponent<TransformComponent>()->get().position.z = 150;
			ship4.GetComponent<TransformComponent>()->get().scale = raylib::Vector3(3.0, 2.0, .5);
			ship4.GetComponent<TransformComponent>()->get().rotation = Vector3{0, 90 * DEG2RAD, 0};
			ship4.AddComponent<RenderingComponent>(raylib::Model("meshes/SmitHouston_Tug.glb"), YELLOW);
			ship4.AddComponent<PhysicsComponent>(Vector3{0, 90 * DEG2RAD, 0}, Vector3{0, 0, 0}, 1, 1, 1.5f, 4, 0, 7.5);  
			ship4.AddComponent<bufferedComponent>(&inputs, false);

	Entity& ship5 = entities.emplace_back(); // Add a new entity to to back of entities vector create reference e to the newly made entity object 
	ship5.GetComponent<TransformComponent>()->get().position.x = -250;
	ship5.GetComponent<TransformComponent>()->get().position.z = 250;
	ship5.GetComponent<TransformComponent>()->get().scale = raylib::Vector3(2.0, 6.0, 2.0);
	ship5.GetComponent<TransformComponent>()->get().rotation = Vector3{0, 90 * DEG2RAD, 0};
	ship5.AddComponent<RenderingComponent>(raylib::Model("meshes/SmitHouston_Tug.glb"), SKYBLUE);
	ship5.AddComponent<PhysicsComponent>(Vector3{0, 90 * DEG2RAD, 0}, Vector3{0, 0, 0}, 1, 1, .5f, 10, 0, 5);  
	ship5.AddComponent<bufferedComponent>(&inputs, false);

	for (Entity& e: entities) e.setup(); // Initialize input for entities

	// Create camera
	auto camera = raylib::Camera(
		raylib::Vector3(0, 120, -500), // Position
		raylib::Vector3(0, 0, 300), // Target
		raylib::Vector3::Up(), // Up direction
		45.0f,
		CAMERA_PERSPECTIVE
	);

	// Create skybox
	cs381::SkyBox skybox("textures/skybox.png");

	// Create ground
	auto mesh = raylib::Mesh::Plane(10000, 10000, 50, 50, 25);
	raylib::Model ground = ((raylib::Mesh*)&mesh)->LoadModelFrom();
	raylib::Texture water("textures/water.jpg");
	water.SetFilter(TEXTURE_FILTER_BILINEAR);
	water.SetWrap(TEXTURE_WRAP_REPEAT);
	ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = water;

	raylib::Text text;
    float textSize = 25;
    const char *speed = {"Speed: "};
	const char *start = {"Press TAB begin simulation"};

	SetTargetFPS(60);
	bool changeCamera = false; // camera change

	// Main loop
	bool keepRunning = true;
	while(!window.ShouldClose() && keepRunning) {

		inputs.PollEvents(); // Poll buffered input from user and 
		if (IsKeyReleased(KEY_C)) { // press C to chnage camera veiw
                if (changeCamera == false) changeCamera = true;
                else changeCamera = false;
            }
		
		// Rendering
		window.BeginDrawing();
		{
			// Clear screen
			window.ClearBackground(BLACK);

			camera.BeginMode();
			{	
				if (counter == 10) camera.fovy = 65.0; // Change feild of veiw for largest ship
				if (changeCamera) { // Target is now position of current selected model
					camera.target = (Vector3){
						entities[counter].GetComponent<TransformComponent>()->get().position.x, 
						entities[counter].GetComponent<TransformComponent>()->get().position.y, 
						entities[counter].GetComponent<TransformComponent>()->get().position.z
					};
					camera.position = (Vector3){ // Camera position
						0, 
						entities[counter].GetComponent<TransformComponent>()->get().position.y + 105, 
						0
					};  
					camera.up = raylib::Vector3::Up(); // Up direction         
					if (counter == 9) camera.fovy = 60.0;
					else camera.fovy = 35.0f; // Camera field-of-view Y
					camera.projection = CAMERA_PERSPECTIVE;
				} else { // revert to old camera settings 
					camera.target = raylib::Vector3(0, 0, 300), // Target
					camera.position = raylib::Vector3(0, 120, -500), // Position
					camera.up = raylib::Vector3::Up(), 
					camera.fovy = 50.0f; // Camera field-of-view Y
					camera.projection = CAMERA_PERSPECTIVE;
				}
				
				// Render skybox and ground
				skybox.Draw();
				ground.Draw({});
				for (Entity& e: entities) e.tick(window.GetFrameTime());
			}
			camera.EndMode();

			int height = window.GetHeight();
        	int width = window.GetWidth();

			float activeSpeed;
			if (counter < 0) {
				activeSpeed = 0; // If there is no plane selected set current speed to zero
				// Start 
				text.Draw(start, (width / 2.5), height * .15, textSize * 1.25, raylib::Color::Black());
			}
			else activeSpeed = round((int)entities[counter].GetComponent<PhysicsComponent>()->get().speed); // Round active speed for more well rounded visual speed
			
			// Speedometer for screen 
			text.Draw(speed, (width * .005), height * .1, textSize, raylib::Color::Black());  
			// Updated speed value
            text.Draw(std::to_string(activeSpeed), (width * .05), height * .1 , textSize , raylib::Color::Black());
			// Measure our FPS
			DrawFPS(10, 10);
		}
		window.EndDrawing();
	}

	return 0;
}

