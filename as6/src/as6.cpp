
// Mason Haines CS 381 3/17
#include <raylib-cpp.hpp>
#include <vector>
#include "rlgl.h"
#include "skybox.hpp"
#include "raylib.h"
#include "entity.hpp"
#include "renderingComponent.hpp"
#include "physicsComponent.hpp"
#include "bufferedInputComponent.hpp"
#include "transformComponent.hpp"

int main() {
	// Create window
	const int screenWidth = 400 * 2;
	const int screenHeight = 450 * 2;
	raylib::Window window(screenWidth, screenHeight, "CS381 - Assignment 6");
	// cs381::Inputs inputs(window);

	raylib::BufferedInput inputs; // Manager for actions 

	// Create vector of entities
	std::vector<Entity> entities;
	int numberOfPlanes = 6;
	int counter = -1;

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


	// offsetROT(OFFSET), velocity(initVELO), speed(initSPEED), targetSpeed(targSpeed), acceleration(ACC), angularAcceleration(angAcc), heading(HEADING), targetHeading(targHEADING){}
	//-------------------------------------------------------------------------------------------------------------------------------
	for (float i = 0; i < numberOfPlanes; i++) {
		Entity& entityPlanes = entities.emplace_back(); // Add anew entity to to back of entities vector create reference e to the newly made entity object 

		entityPlanes.GetComponent<TransformComponent>()->get().position.x = i * 50 - 100;
		entityPlanes.GetComponent<TransformComponent>()->get().position.y = 150;
		entityPlanes.GetComponent<TransformComponent>()->get().scale = raylib::Vector3(.65, .65, .65);
		entityPlanes.GetComponent<TransformComponent>()->get().rotation = raylib::Vector3(0, 0, 0); 
		entityPlanes.AddComponent<RenderingComponent>(raylib::Model("meshes/PolyPlane.glb"));
		entityPlanes.AddComponent<PhysicsComponent>(Vector3{0, 0, 0}, Vector3{}, .025, 1, 10, 45); //offset - velo - speed - target speed - acceleration - ang acc- heading - target heading
		// entityPlanes.AddComponent<PhysicsComponent>(Vector3{0, 0, 0}, Vector3{}, 5, 0); //offset - velo - speed - target speed - acceleration - ang acc- heading - target heading
		entityPlanes.AddComponent<bufferedComponent>(&inputs, false);
	}

		Entity& ship1 = entities.emplace_back(); // Add anew entity to to back of entities vector create reference e to the newly made entity object 
		ship1.GetComponent<TransformComponent>()->get().position.x = -250;
		ship1.GetComponent<TransformComponent>()->get().position.z = -100;
		ship1.GetComponent<TransformComponent>()->get().scale = raylib::Vector3(.5, 1.0, .5);
		ship1.GetComponent<TransformComponent>()->get().rotation = Vector3{0, 90 * DEG2RAD, 0};
		ship1.AddComponent<RenderingComponent>(raylib::Model("meshes/SmitHouston_Tug.glb"));
		ship1.AddComponent<PhysicsComponent>(Vector3{0, 90 * DEG2RAD, 0}, Vector3{}, .5, 1, 2.5f, 5); //offset - velo - speed - target speed - acceleration - ang acc- heading - target heading
		ship1.AddComponent<bufferedComponent>(&inputs, false);

		Entity& ship2 = entities.emplace_back(); // Add anew entity to to back of entities vector create reference e to the newly made entity object 
		ship2.GetComponent<TransformComponent>()->get().position.x = -250;
		ship2.GetComponent<TransformComponent>()->get().position.z = -50;
		ship2.GetComponent<TransformComponent>()->get().scale = raylib::Vector3(1.0, 1.0, 1.0);
		ship2.GetComponent<TransformComponent>()->get().rotation = Vector3{0, 90 * DEG2RAD, 0};
		ship2.AddComponent<RenderingComponent>(raylib::Model("meshes/SmitHouston_Tug.glb"));
		ship2.AddComponent<PhysicsComponent>(Vector3{0, 90 * DEG2RAD, 0}, Vector3{}, .5, 1, 2.5f, 5); //offset - velo - speed - target speed - acceleration - ang acc- heading - target heading
		ship2.AddComponent<bufferedComponent>(&inputs, false);

		Entity& ship3 = entities.emplace_back(); // Add anew entity to to back of entities vector create reference e to the newly made entity object 
		ship3.GetComponent<TransformComponent>()->get().position.x = -250;
		ship3.GetComponent<TransformComponent>()->get().scale = raylib::Vector3(5, 1.35, 1.35);
		ship3.GetComponent<TransformComponent>()->get().rotation = Vector3{0, 90 * DEG2RAD, 0};
		ship3.AddComponent<RenderingComponent>(raylib::Model("meshes/SmitHouston_Tug.glb"));
		ship3.AddComponent<PhysicsComponent>(Vector3{0, 90 * DEG2RAD, 0}, Vector3{}, .5, 1, 2.5f, 5); //offset - velo - speed - target speed - acceleration - ang acc- heading - target heading
		ship3.AddComponent<bufferedComponent>(&inputs, false);

		Entity& ship4 = entities.emplace_back(); // Add anew entity to to back of entities vector create reference e to the newly made entity object 
		ship4.GetComponent<TransformComponent>()->get().position.x = -250;
		ship4.GetComponent<TransformComponent>()->get().position.z = 150;
		ship4.GetComponent<TransformComponent>()->get().scale = raylib::Vector3(3.0, 2.0, .5);
		ship4.GetComponent<TransformComponent>()->get().rotation = Vector3{0, 90 * DEG2RAD, 0};
		ship4.AddComponent<RenderingComponent>(raylib::Model("meshes/SmitHouston_Tug.glb"));
		ship4.AddComponent<PhysicsComponent>(Vector3{0, 90 * DEG2RAD, 0}, Vector3{}, .5, 1, 2.5f, 5); //offset - velo - speed - target speed - acceleration - ang acc- heading - target heading
		ship4.AddComponent<bufferedComponent>(&inputs, false);

		Entity& ship5 = entities.emplace_back(); // Add anew entity to to back of entities vector create reference e to the newly made entity object 
		ship5.GetComponent<TransformComponent>()->get().position.x = -250;
		ship5.GetComponent<TransformComponent>()->get().position.z = 250;
		ship5.GetComponent<TransformComponent>()->get().scale = raylib::Vector3(2.0, 6.0, 2.0);
		ship5.GetComponent<TransformComponent>()->get().rotation = Vector3{0, 90 * DEG2RAD, 0};
		ship5.AddComponent<RenderingComponent>(raylib::Model("meshes/SmitHouston_Tug.glb"));
		ship5.AddComponent<PhysicsComponent>(Vector3{0, 90 * DEG2RAD, 0}, Vector3{}, .5, 1, 2.5f, 5); //offset - velo - speed - target speed - acceleration - ang acc- heading - target heading
		ship5.AddComponent<bufferedComponent>(&inputs, false);

		

		// Entity& ship2 = entities.emplace_back(); // Add anew entity to to back of entities vector create reference e to the newly made entity object 
		// ship2.GetComponent<TransformComponent>()->get().position.x = 100;
		// ship2.GetComponent<TransformComponent>()->get().position.z = 300;
		// ship2.GetComponent<TransformComponent>()->get().scale = raylib::Vector3(.015, .015, .015);
		// ship2.GetComponent<TransformComponent>()->get().rotation = Vector3{0, 0, 0};

		// // ship2.GetComponent<TransformComponent>()->get().rotation.SetX(0);
		// // ship2.GetComponent<TransformComponent>()->get().rotation.SetY(0);
		// // ship2.GetComponent<TransformComponent>()->get().rotation.SetZ(0);

		// ship2.AddComponent<RenderingComponent>(raylib::Model("meshes/OrientExplorer.glb"));
		// ship2.AddComponent<PhysicsComponent>(Vector3{}, Vector3{}, 1, 0, 10.5f, 100); //offset - velo - speed - target speed - acceleration - ang acc- heading - target heading
		// ship2.AddComponent<bufferedComponent>(&inputs, false);


		

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

	for (Entity& e: entities) e.setup();

	SetTargetFPS(85);

	// Main loop
	bool keepRunning = true;
	while(!window.ShouldClose() && keepRunning) {
		inputs.PollEvents();
	


		// Rendering
		window.BeginDrawing();
		{
			// Clear screen
			window.ClearBackground(BLACK);

			camera.BeginMode();
			{
				////////////////////
				

				///-------------------------------------------------------------------------------------------------------------------------------	
				/////////////////////////////////////////////
				// Render skybox and ground
				skybox.Draw();
				ground.Draw({});
				for (Entity& e: entities) e.tick(window.GetFrameTime());
				///-------------------------------------------------------------------------------------------------------------------------------
				
				// updateEntities(entities, window.GetFrameTime());

				
			}
			camera.EndMode();

			// Measure our FPS
			DrawFPS(10, 10);
		}
		window.EndDrawing();
	}

	return 0;
}

