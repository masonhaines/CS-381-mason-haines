// // #include <concepts>
// #include <cstddef>
// // #include <functional>

// #include <raylib-cpp.hpp>
// #include "BufferedInput.hpp"
// // #include <utility>
// #include <vector>
// // #include <memory>
// #include "rlgl.h"
// #include "skybox.hpp"
// #include "raylib.h"
// #include "entity.hpp"
// #include "renderingComponent.hpp"
// // #include "transformComponent.hpp"


// template<typename T>
// concept Transformer = requires(T t, raylib::Transform m) {
// 	{ t.operator()(m) } -> std::convertible_to<raylib::Transform>;
// };

// int main() {
// 	// Create window
// 	const int screenWidth = 800 * 2;
// 	const int screenHeight = 450 * 2;
// 	raylib::Window window(screenWidth, screenHeight, "CS381 - Assignment 6");
// 	// cs381::Inputs inputs(window);


// 	std::vector<Entity> entities;
// 	Entity& e = entities.emplace_back();
// 	e.AddComponent<RenderingComponent>(raylib::Model("meshes/PolyPlane.glb"));

// 	// Create camera
// 	auto camera = raylib::Camera(
// 		raylib::Vector3(0, 120, -500), // Position
// 		raylib::Vector3(0, 0, 300), // Target
// 		raylib::Vector3::Up(), // Up direction
// 		45.0f,
// 		CAMERA_PERSPECTIVE
// 	);

// 	// Create skybox
// 	cs381::SkyBox skybox("textures/skybox.png");

// 	// Create ground
// 	auto mesh = raylib::Mesh::Plane(10000, 10000, 50, 50, 25);
// 	raylib::Model ground = ((raylib::Mesh*)&mesh)->LoadModelFrom();
// 	raylib::Texture grass("textures/grass.jpg");
// 	grass.SetFilter(TEXTURE_FILTER_BILINEAR);
// 	grass.SetWrap(TEXTURE_WRAP_REPEAT);
// 	ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grass;

	
// 	while(!window.ShouldClose() ) {
		


// 		// Rendering
// 		window.BeginDrawing();
// 		{
// 			// Clear screen
// 			window.ClearBackground(BLACK);

// 			camera.BeginMode();
// 			{
				
				
// 				/////im just a comment 
				
// 				// Render skybox and ground
// 				skybox.Draw();
// 				ground.Draw({});

// 				for (Entity& e: entities) e.tick(window.GetFrameTime());

				
// 			}
// 			camera.EndMode();

// 			// Measure our FPS
// 			DrawFPS(10, 10);
// 		}
// 		window.EndDrawing();
// 	}

// 	return 0;
// }