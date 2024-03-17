// #include <concepts>
#include <cstddef>
// #include <functional>

#include <raylib-cpp.hpp>

// #include <utility>
#include <vector>
// #include <memory>
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
	const int screenWidth = 800 * 2;
	const int screenHeight = 450 * 2;
	raylib::Window window(screenWidth, screenHeight, "CS381 - Assignment 6");
	// cs381::Inputs inputs(window);

	raylib::BufferedInput inputs; // Manager for actions 

	// Create vector of entities
	std::vector<Entity> entities;
	int numberOfPlanes = 1;
	int counter = 0;

	inputs["forward"] = raylib::Action::key(KEY_W).move();
	inputs["backwards"] = raylib::Action::key(KEY_S).move();
	inputs["right"] = raylib::Action::key(KEY_D).move();
	inputs["left"] = raylib::Action::key(KEY_A).move();

	inputs["change"] = raylib::Action::key(KEY_TAB).SetPressedCallback([&entities, &counter](){
		counter = (counter + 1) % entities.size();
		for (int i = 0; i < entities.size(); i++) {
			auto ref = entities[i].GetComponent<bufferedComponent>(); // get optional reference to transform component 
			if (!ref) return; // does it exist 
			auto& INPUT = ref->get(); // get values stored in reference if it exists
			INPUT.selected = counter == i;
		}
	}).move();

	///-------------------------------------------------------------------------------------------------------------------------------
	for (int i = 0; i < numberOfPlanes; i++) {
		Entity& entityObjects = entities.emplace_back(); // Add anew entity to to back of entities vector create refernce e to the newly made entity object 

		entityObjects.AddComponent<RenderingComponent>
		(raylib::Model("meshes/PolyPlane.glb"));
		entityObjects.GetComponent<TransformComponent>()->get().position.x = i * 25;
		entityObjects.GetComponent<TransformComponent>()->get().scale = {1,2,3};
		entityObjects.GetComponent<TransformComponent>()->get().shade = GREEN;
		// entityObjects.GetComponent<TransformComponent>()->get().rotation.z = 180 * DEG2RAD; // this makes it wiggle?
		entityObjects.AddComponent<PhysicsComponent>(Vector3{}, 5);
		entityObjects.AddComponent<bufferedComponent>(&inputs);


	}
	///-------------------------------------------------------------------------------------------------------------------------------
	// DrawPlaneModels(numberOfPlanes, entities);

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


	// size_t selectedPlane = 0;

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

// // Input handling
// bool ProcessInput(raylib::Degree& planeTargetHeading, float& planeTargetSpeed, size_t& selectedPlane) {
// 	static bool wPressedLastFrame = false, sPressedLastFrame = false;
// 	static bool aPressedLastFrame = false, dPressedLastFrame = false;
// 	static bool tabPressedLastFrame = false;

// 	// If we hit escape... shutdown
// 	if(IsKeyDown(KEY_ESCAPE))
// 		return false;

// 	// WASD updates plane velocity
// 	if(IsKeyDown(KEY_W) && !wPressedLastFrame)
// 		planeTargetSpeed += 1;
// 	if(IsKeyDown(KEY_S) && !sPressedLastFrame)
// 		planeTargetSpeed -= 1;
// 	if(IsKeyDown(KEY_A) && !aPressedLastFrame)
// 		planeTargetHeading += 5;
// 	if(IsKeyDown(KEY_D) && !dPressedLastFrame)
// 		planeTargetHeading -= 5;  

// 	// Space sets velocity to 0!
// 	if(IsKeyDown(KEY_SPACE))
// 		planeTargetSpeed = 0;

// 	// Tab selects the next plane
// 	if(IsKeyDown(KEY_TAB) && !tabPressedLastFrame)
// 		selectedPlane = (selectedPlane + 1) % 3;

// 	// Save the state of the key for next frame
// 	wPressedLastFrame = IsKeyDown(KEY_W);
// 	sPressedLastFrame = IsKeyDown(KEY_S);
// 	aPressedLastFrame = IsKeyDown(KEY_A);
// 	dPressedLastFrame = IsKeyDown(KEY_D);

// 	tabPressedLastFrame = IsKeyDown(KEY_TAB);

// 	return true;
// }

// raylib::Vector3 CalculateVelocity(const CalculateVelocityParams& data) {
// 	static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
// 		float decimal = float(angle) - int(angle);
// 		int whole = int(angle) % 360;
// 		whole += (whole < 0) * 360;
// 		return decimal + whole;
// 	};

// 	float target = Clamp(data.targetSpeed, data.minSpeed, data.maxSpeed);
// 	if(data.speed < target) data.speed += data.acceleration * data.dt;
// 	else if(data.speed > target) data.speed -= data.acceleration * data.dt;
// 	data.speed = Clamp(data.speed, data.minSpeed, data.maxSpeed);

// 	target = AngleClamp(data.targetHeading);
// 	float difference = abs(target - data.heading);
// 	if(target > data.heading) {
// 		if(difference < 180) data.heading += data.angularAcceleration * data.dt;
// 		else if(difference > 180) data.heading -= data.angularAcceleration * data.dt;
// 	} else if(target < data.heading) {
// 		if(difference < 180) data.heading -= data.angularAcceleration * data.dt;
// 		else if(difference > 180) data.heading += data.angularAcceleration * data.dt;
// 	} 
// 	if(difference < .5) data.heading = target; // If the heading is really close to correct 
// 	data.heading = AngleClamp(data.heading);
// 	raylib::Radian angle = raylib::Degree(data.heading);

// 	return {cos(angle) * data.speed, 0, -sin(angle) * data.speed};
// }

// void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
// 	raylib::Transform backupTransform = model.transform;
// 	model.transform = transformer(backupTransform);
// 	model.Draw({});
// 	model.GetTransformedBoundingBox().Draw();
// 	model.transform = backupTransform;
// }

// void DrawModel(raylib::Model& model, Transformer auto transformer) {
// 	raylib::Transform backupTransform = model.transform;
// 	model.transform = transformer(backupTransform);
// 	model.Draw({});
// 	// model.GetTransformedBoundingBox()
// 	model.transform = backupTransform;
// }

// void DrawPlaneModels(int numberOfPlanes, std::vector<Entity>& entities) {
// 	for (int i = 0; i < numberOfPlanes; i++) {
// 		Entity& entityObjects = entities.emplace_back(); // Add anew entity to to back of entities vector create refernce e to the newly made entity object 
// 		entityObjects.AddComponent<RenderingComponent>
// 		(raylib::Model("meshes/PolyPlane.glb"));
// 		entityObjects.AddComponent<TransformComponent>
// 		(Vector3{i * 5.0f, 200, 5}, 45 * i, Vector3{5, 5, 5}, GREEN);
// 	}
// }

void updateEntities(std::vector<Entity>& entities, float dt) {
    for (Entity& e : entities) {
        e.tick(dt);
    }
}