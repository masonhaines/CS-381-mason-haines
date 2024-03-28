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

// #ifndef PHYSICSCOMPONENT_HPP
// #define PHYSICSCOMPONENT_HPP

// #include "component.hpp"
// #include "entity.hpp" 
// #include "transformComponent.hpp" 
// #include <iostream>


// struct PhysicsComponent : public Component {
//     raylib::Vector3 velocity = {0, 0, 0};
//     float speed = 5;
//     float targetSpeed = 0;
//     raylib::Degree targetHeading = 0;
//     raylib::Degree heading = 0; 

//     float minSpeed = 0;
//     float maxSpeed = 50;

//     static constexpr float acceleration = 10;
//     static constexpr float angularAcceleration = 30;



//     PhysicsComponent(Entity& en/tity, const raylib::Vector3& initVELO = {0, 0, 0}, float initSPEED = 0, float HEADING = 0, float targSpeed = 0, float targHEADING = 0 )
//         : Component(entity), velocity(initVELO), speed(initSPEED), heading(HEADING), targetHeading(targHEADING), targetSpeed(targSpeed) {}

//     void tick(float dt) override {

//         static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
//             int intPart = angle;
//             float floatPart = float(angle) - intPart;
//             intPart %= 360;
//             intPart += (intPart < 0) * 360;
//             return intPart + floatPart;
//         };


//         auto ref = object->GetComponent<TransformComponent>(); // get optional reference to transform component 
//         if (!ref) return; // does it exist 
//         auto& transform = ref->get(); // get values stored in reference if it exists

//         float target = Clamp(targetSpeed, minSpeed, maxSpeed);
//         if(speed < target) speed += acceleration * dt;
//         else if(speed > target) speed -= acceleration * dt;
//         speed = Clamp(speed, minSpeed, maxSpeed);

//         std::cout << speed << "<< speed >> " << std::endl;
//         std::cout << targetSpeed << "<< target speed >> " << std::endl;

//         target = AngleClamp(targetHeading);
//         float difference = abs(target - heading);
//         if(target > heading) {
//             if(difference < 180) heading += angularAcceleration * dt;
//             else if(difference > 180) heading -= angularAcceleration * dt;
//         } else if(target < heading) {
//             if(difference < 180) heading -= angularAcceleration * dt;
//             else if(difference > 180) heading += angularAcceleration * dt;
//         } 
//         if(difference < .0001) heading = target; // If the heading is really close to correct 
//         heading = AngleClamp(heading);
//         raylib::Radian angle = raylib::Degree(heading);



//         velocity.x = speed * cos(angle);
//         velocity.z = -speed * sin(angle);

//         // Update position based on velocity
//         transform.position.x += velocity.x * dt;
//         transform.position.y += velocity.y * dt;
//         transform.position.z += velocity.z * dt;

//         transform.rotation.y = angle;

//         // Debug output
//         std::cout << "PhysicsComponent tick function called." << std::endl;
//         // std::cout << "Position: (" << transform.position.x << ", " << transform.position.y << ", " << transform.position.z << ")" << std::endl;
//         std::cout << "axis: (" << transform.rotation.x << ", " << transform.rotation.y << ", " << transform.rotation.z << ")" << std::endl;
//         // std::cout << "Velocity mag: (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;
//     }
// };

// #endif 
