
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
	const int screenWidth = 800* 2;
	const int screenHeight = 450 * 2;
	raylib::Window window(screenWidth, screenHeight, "CS381 - Assignment 6");
	// cs381::Inputs inputs(window);

	raylib::BufferedInput inputs; // Manager for actions 

	// Create vector of entities
	std::vector<Entity> entities;
	int numberOfPlanes = 1;
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

	///-------------------------------------------------------------------------------------------------------------------------------
	for (int i = 0; i < numberOfPlanes; i++) {
		Entity& entityPlanes = entities.emplace_back(); // Add anew entity to to back of entities vector create reference e to the newly made entity object 
		
		entityPlanes.AddComponent<RenderingComponent>
		(raylib::Model("meshes/PolyPlane.glb"));
		// DrawBoundingBox(entityPlanes.GetComponent<RenderingComponent>()->get().box, PINK);
		entityPlanes.GetComponent<TransformComponent>()->get().position.x = i * 50 - 100;
		entityPlanes.GetComponent<TransformComponent>()->get().position.y = 50;
		entityPlanes.GetComponent<TransformComponent>()->get().scale = raylib::Vector3(1.0f, 1.0f, 1.0f);
		entityPlanes.GetComponent<TransformComponent>()->get().shade = RED;
		entityPlanes.GetComponent<TransformComponent>()->get().rotation = raylib::Vector3(0, 180 * DEG2RAD, 0); // these are in radians 
		// entityPlanes.GetComponent<TransformComponent>()->get().heading = 5 * DEG2RAD;
		entityPlanes.AddComponent<PhysicsComponent>(Vector3{0, 0, 0}, 0);
		entityPlanes.AddComponent<bufferedComponent>(&inputs, false);


	}

	// for (int i = 0; i < numberOfPlanes; i++) {
		Entity& entityShips = entities.emplace_back(); // Add anew entity to to back of entities vector create reference e to the newly made entity object 
		
		entityShips.AddComponent<RenderingComponent>
		(raylib::Model("meshes/SmitHouston_Tug.glb"));
		// DrawBoundingBox(entityShips.GetComponent<RenderingComponent>()->get().box, PINK);
		entityShips.GetComponent<TransformComponent>()->get().position.x = 50 - 100;
		// entityShips.GetComponent<TransformComponent>()->get().position.y = 50;
		entityShips.GetComponent<TransformComponent>()->get().scale = raylib::Vector3( 1.0f, 1.0f, 1.0f);
		entityShips.GetComponent<TransformComponent>()->get().shade = RED;
		entityShips.GetComponent<TransformComponent>()->get().rotation = raylib::Vector3(0.0f, 90 * DEG2RAD, 0.0f);
		// entityShips.GetComponent<TransformComponent>()->get().heading = 0;
		entityShips.AddComponent<PhysicsComponent>(Vector3{0, 0, 0}, 0);
		entityShips.AddComponent<bufferedComponent>(&inputs, false);


	// }
	///-------------------------------------------------------------------------------------------------------------------------------

	// Create camera
	auto camera = raylib::Camera(
		raylib::Vector3(0, 120, -300), // Position
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
	raylib::Texture grass("textures/grass.jpg");
	grass.SetFilter(TEXTURE_FILTER_BILINEAR);
	grass.SetWrap(TEXTURE_WRAP_REPEAT);
	ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grass;

	for (Entity& e: entities) e.setup();

	SetTargetFPS(15);

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

