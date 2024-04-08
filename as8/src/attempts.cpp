// // Mason Haines as8
// #include <raylib-cpp.hpp>
// #include <cstddef>
// #include <cassert>
// #include <cstdint>
// #include <limits>
// // #include <string>
// #include <vector>
// #include "rlgl.h"
// #include "skybox.hpp"
// #include "raylib.h"
// #include <ranges>
// #include <memory>
// #include <iostream>
// // #include "Counter.cpp"

// template<typename T>
// concept Transformer = requires( T t, raylib::Transform m) {
//     {t.operator()(m)}->std::convertible_to<raylib::Transform>;
// };

// using Entity = uint8_t;

// struct ComponentStorage {
//     size_t elementSize = -1;
//     std::vector<std::byte> data; // buffer that is 4 bytes long 

//     ComponentStorage() : elementSize(-1), data(1, std::byte{0}) {};
//     ComponentStorage (size_t elementSize) : elementSize(elementSize) {data.resize(5 * elementSize);}

//     template<typename Tcomponent> 
//     ComponentStorage(Tcomponent reference = {}) : ComponentStorage(sizeof(Tcomponent)) {}

//     template<typename Tcomponent> 
//     Tcomponent& Get(Entity e) {
//         assert(sizeof(Tcomponent) == elementSize);
//         assert(e < (data.size() / elementSize));
//         return *(Tcomponent*)(data.data() + e * elementSize);
//     }

//     template<typename Tcomponent> 
//     std::pair<Tcomponent&, Entity> Allocate(size_t count = 1) {
//         assert(sizeof(Tcomponent) == elementSize);
//         assert (count < 255);
//         auto originalEnd = data.size();
//         data.insert(data.end(), elementSize * count, std::byte{0});
//         for (size_t i = 0; i < count - 1; i ++) 
//             new(data.data() + originalEnd + i * elementSize) Tcomponent();
//         return {
//             *new(data.data() + data.size() - elementSize) Tcomponent(), 
//             data.size() / elementSize
//         };
//     }

//     template<typename Tcomponent> 
//     Tcomponent& GetOrAllocate(Entity e) {

        
//         assert(sizeof(Tcomponent) == elementSize); // Assumption that it should be true at this point. If it is not, edge case will fail during runtime.
//         size_t size = data.size() / elementSize;

//         if (size <= e) {
//             Allocate<Tcomponent>(std::max<int64_t>(int64_t(e) - size, 1));
//         }

//         return Get<Tcomponent>(e);
//     }
// };

// extern size_t globalComponentCounter;

// template<typename T> // mapping a type to an integer 
// size_t getComponentID(T reference = {}) {
//     static size_t id = globalComponentCounter++;
//     return id;
// }

// struct Scene {
//     std::vector<std::vector<bool>> entityMasks;
//     std::vector<ComponentStorage> storages = {ComponentStorage()};

//     template<typename Tcomponent>
//     ComponentStorage& GetStorage() {
//         size_t id = getComponentID<Tcomponent>();
//         if (storages.size() <= id) {
//             storages.insert(storages.cend(), std::max<int64_t>(id - storages.size(), 1), {});
//         }
//         if (storages[id].elementSize == std::numeric_limits<size_t>::max()) 
//             storages[id] = ComponentStorage(Tcomponent{});
//         return storages[id];
//     }

//     Entity CreateEntity( ) {
//         Entity e = entityMasks.size();
//         entityMasks.emplace_back(std::vector<bool>{false});
//         return e;

//     }

//     template< typename Tcomponent> 
//     Tcomponent& AddComponent(Entity e) {
//         size_t id = getComponentID<Tcomponent>();
//         auto& eMask = entityMasks[e];
        
//         if (eMask.size() <= id) {
//             eMask.resize(id + 1, false);
//         }
//         eMask[id] = true;
//         return GetStorage<Tcomponent>().template GetOrAllocate<Tcomponent>(e);
//     }

//     template<typename Tcomponent>
//     void RemoveComponent(Entity e) {
//         size_t id = getComponentID<Tcomponent>(); // Get ID
//         auto& eMask = entityMasks[e]; // Access the entity mask for the given entity
        
//         if (eMask.size() > id) { // if amount of masks is greater than number of components
//             eMask[id] = false; // Set entity mask at component index to false, swap bit set 
//         }
//     }

//     template<typename Tcomponent>
//     Tcomponent& getComponent(Entity e) {
//         size_t id = getComponentID<Tcomponent>();
//         assert(entityMasks[e][id]);
//         return GetStorage<Tcomponent>().template Get<Tcomponent>(e);
//     }

//     template<typename Tcomponent>
//     bool hasComponent(Entity e) {
//         size_t id = getComponentID<Tcomponent>();
//         return entityMasks.size() > e && entityMasks[e].size() > id && entityMasks[e][id];
//     }

// };

// struct physics {

// 	// static constexpr
//     float acceleration;
// 	float angularAcceleration;

// 	float targetSpeed;
// 	raylib::Degree targetHeading;
// 	float* speed;
// 	raylib::Degree* heading;
// 	float dt;

// 	float maxSpeed;
// 	float minSpeed;


//     physics() : speed(nullptr), heading(nullptr) {
//         speed = new float(0.0f); // Initialize to some default value
//         heading = new raylib::Degree(0.0f); // Initialize to some default value
//     }

//     ~physics() {
//         delete speed;
//         delete heading;
//     }
//     // }physics() : speed(nullptr), heading(nullptr) {}
// };

// struct Rendering {
//     raylib::Model* model;
//     bool drawBoundingBox = false;
// };

// struct transformcomp {
//     raylib::Vector3 position;
//     raylib::Vector3 scale;
//     raylib::Quaternion rotation;
// };

// void DrawBoundedModel(raylib::Model& model, Transformer auto transformer);
// void DrawModel(raylib::Model& model, Transformer auto transformer);
// void PhysicsSystem(Scene& scene);
// void DrawSystem(Scene& scene);



// void DrawSystem(Scene& scene) {

//     // raylib::Color color;

//     for(Entity e = 0; e < scene.entityMasks.size(); e++) {
//         if(!scene.hasComponent<Rendering>(e)) continue;
//         if(!scene.hasComponent<transformcomp>(e)) continue;
//         auto & rendering = scene.getComponent<Rendering>(e);
//         auto & transformComponent = scene.getComponent<transformcomp>(e);

//         auto[axis, angle] = scene.getComponent<transformcomp>(e).rotation.ToAxisAngle();
//         rendering.model->transform = raylib::Transform(rendering.model->transform).Rotate(axis, angle);
//                 // std::cout << "axis.x" << transformComponent.rotation.ToEuler().x << std::endl;
//                 // std::cout << "axis.y" << transformComponent.rotation.ToEuler().y << std::endl;
//                 // std::cout << "axis.z" << transformComponent.rotation.ToEuler().z << std::endl;
                
//         // Vector3 Eul = scene.getComponent<transformcomp>(e).rotation.ToEuler();

//         auto Transformer = [&transformComponent, &axis, &angle](raylib::Transform t) -> raylib::Transform {
//             // auto[axis, angle] = scene.getComponent<transformcomp>(e).rotation.ToAxisAngle();
//             // rendering.model->transform = raylib::Transform(rendering.model->transform).Rotate(axis, angle);

//             return t.
//             Translate(transformComponent.position).
//             Scale(transformComponent.scale.x, transformComponent.scale.y, transformComponent.scale.z).
//             // RotateXYZ(axis);
//             RotateXYZ(transformComponent.rotation.x, transformComponent.rotation.y, transformComponent.rotation.z); // q1 is word xyz  q2 is incremetn 
//         };
        
//         if (rendering.drawBoundingBox) {
//             DrawBoundedModel(*rendering.model, Transformer);
//         } else {
//             DrawModel(*rendering.model, Transformer);
//         }
//     }
// }

// int main() {
// 	// Create window
// 	const int screenWidth = 900 * 2;
// 	const int screenHeight = 450 * 2;
// 	raylib::Window window(screenWidth, screenHeight, "CS381 - Assignment 8");

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
// 	auto mesh = raylib::Mesh::Plane(10000, 10000, 50, 50, 125);
// 	raylib::Model ground = ((raylib::Mesh*)&mesh)->LoadModelFrom();
// 	raylib::Texture water("textures/water.jpg");
// 	water.SetFilter(TEXTURE_FILTER_BILINEAR);
// 	water.SetWrap(TEXTURE_WRAP_REPEAT);
// 	ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = water;

//     raylib::Model plane("meshes/PolyPlane.glb");
//     Scene scene;
//     auto e = scene.CreateEntity();
//     // Add a transform component to the entity
//     scene.AddComponent<Rendering>(e) = {&plane, false}; // Plane with no bounding box, ie false 
//     scene.AddComponent<transformcomp>(e).position = {(Vector3){0, 90, 1}};
//     scene.AddComponent<transformcomp>(e).scale = {(Vector3){5,5,5}};
//     scene.AddComponent<transformcomp>(e).rotation = {(Quaternion){0,90 * DEG2RAD,0 * DEG2RAD,1}};
//     scene.AddComponent<physics>(e).acceleration = 5;
//     scene.AddComponent<physics>(e).angularAcceleration = 15;
//     scene.AddComponent<physics>(e).dt = window.GetFrameTime();
//     // scene.AddComponent<physics>(e).heading = nullptr;
//     scene.AddComponent<physics>(e).targetHeading = 0; //.RadianValue();
//     // scene.AddComponent<physics>(e).speed = nullptr;
//     scene.AddComponent<physics>(e).targetSpeed = 5;
//     scene.AddComponent<physics>(e).minSpeed = 0;
//     scene.AddComponent<physics>(e).maxSpeed = 25;
//     // scene.AddComponent<transformcomp>(e).rotation = raylib::Quaternion::FromAxisAngle({0, 0, 0}, 1);
//     // raylib::Quaternion rotationQuat = raylib::Quaternion::FromAxisAngle(raylib::Vector3{0, 1, 0}, 0);
//     // Create a quaternion for rotating around the Y-axis by 90 degrees
//     //   scene.AddComponent<transformcomp>(e).rotation = raylib::Quaternion::ToEuler({1,1,1});
//     //    scene.AddComponent<transformcomp>(e).rotation = 
//     // scene.AddComponent<transformcomp>(e).rotation = raylib::Quaternion::Normalize({1, 1, 1, 1});
//     // scene.AddComponent<transformcomp>(e).rotation.ToEuler() = {(Vector3){5,5,5}};
//     // scene.AddComponent<transformcomp>(e) = {raylib::Vector3{1, 90, 1}, 
//     // raylib::Vector3{1, 90, 1}, 
//     // raylib::Quaternion{1, 1, 1, 1}};



    

//     size_t selectedPlane = 0;

// 	SetTargetFPS(60);

// 	// Main loop
// 	bool keepRunning = true;
// 	while(!window.ShouldClose() && keepRunning) {
		
// 		// Rendering
// 		window.BeginDrawing();
// 		{
// 			// Clear screen
// 			window.ClearBackground(BLACK);

// 			camera.BeginMode();
// 			{	
				
				
// 				// Render skybox and ground
// 				skybox.Draw();
// 				ground.Draw({});

                
				
// 			}
//             PhysicsSystem(scene);
//             DrawSystem(scene);
// 			camera.EndMode();
            

// 			int height = window.GetHeight();
//         	int width = window.GetWidth();

// 			DrawFPS(10, 10);
// 		}
// 		window.EndDrawing();
// 	}

// 	return 0;
// }

// // Function to draw a bounded model with a specified transformation
// void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
//     raylib::Transform backupTransform = model.transform; // Save the model's transform
//     model.transform = transformer(backupTransform); // Update the model's transform based on the transformation function
//     model.Draw({});
//     model.GetTransformedBoundingBox().Draw();
//     model.transform = backupTransform; // Restore the original transform
// }

// // Function to draw a model with a specified transformation
// void DrawModel(raylib::Model& model, Transformer auto transformer) {
//     raylib::Transform backupTransform = model.transform; // Save the model's transform
//     model.transform = transformer(backupTransform); // Update the model's transform based on the transformation function
//     model.Draw({});
//     model.transform = backupTransform; // Restore the original transform
// }

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

// // raylib::Vector3 CaclulateVelocity(const CalculateVelocityParams& data) {
// // 	static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
// // 		float decimal = float(angle) - int(angle);
// // 		int whole = int(angle) % 360;
// // 		whole += (whole < 0) * 360;
// // 		return decimal + whole;
// // 	};
// // 	float target = Clamp(data.targetSpeed, data.minSpeed, data.maxSpeed);
// // 	if(data.speed < target) data.speed += data.acceleration * data.dt;
// // 	else if(data.speed > target) data.speed -= data.acceleration * data.dt;
// // 	data.speed = Clamp(data.speed, data.minSpeed, data.maxSpeed);
// // 	target = AngleClamp(data.targetHeading);
// // 	float difference = abs(target - data.heading);
// // 	if(target > data.heading) {
// // 		if(difference < 180) data.heading += data.angularAcceleration * data.dt;
// // 		else if(difference > 180) data.heading -= data.angularAcceleration * data.dt;
// // 	} else if(target < data.heading) {
// // 		if(difference < 180) data.heading -= data.angularAcceleration * data.dt;
// // 		else if(difference > 180) data.heading += data.angularAcceleration * data.dt;
// // 	} 
// // 	if(difference < .5) data.heading = target; // If the heading is really close to correct 
// // 	data.heading = AngleClamp(data.heading);
// // 	raylib::Radian angle = raylib::Degree(data.heading);
// // 	return {cos(angle) * data.speed, 0, -sin(angle) * data.speed};
// // }

// // Physics System
// // void PhysicsSystem(Scene& scene) {
// //     static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
// // 		float decimal = float(angle) - int(angle);
// // 		int whole = int(angle) % 360;
// // 		whole += (whole < 0) * 360;
// // 		return decimal + whole;
// // 	};
// //     // Iterate over all entities in the scene
// //     for (Entity e = 0; e < scene.entityMasks.size(); e++) {
// //         if (!scene.hasComponent<transformcomp>(e)) continue;
// //         if (!scene.hasComponent<physics>(e)) continue;
// //         // Check if the entity has transform and physics components
// //         // if (!scene.hasComponent<transformcomp>(e) || !scene.hasComponent<physics>(e))
// //         //     continue;
// //         // Get references to the transform and physics components
// //         auto& transformComponent = scene.getComponent<transformcomp>(e);
// //         auto& physicsComponent = scene.getComponent<physics>(e);
// //         // Calculate velocity based on physics parameters
// //         float acceleration = physicsComponent.acceleration;
// //         float angularAcceleration = physicsComponent.angularAcceleration;
// //         float targetSpeed = physicsComponent.targetSpeed;
// //         raylib::Degree targetHeading = physicsComponent.targetHeading;
// //         float maxSpeed = physicsComponent.maxSpeed;
// //         float minSpeed = physicsComponent.minSpeed;
// //         float* speedPtr = physicsComponent.speed;
// //         raylib::Degree* headingPtr = physicsComponent.heading;
// //         float speed = *speedPtr;
// //         raylib::Degree heading = *headingPtr;
// //         float dt = physicsComponent.dt;
// //         // Calculate new speed and heading using physics calculations
// //         float target = Clamp(targetSpeed, minSpeed, maxSpeed);
// //         if (speed < target)
// //             speed += acceleration * dt;
// //         else if (speed > target)
// //             speed -= acceleration * dt;
// //         speed = Clamp(speed, minSpeed, maxSpeed);
// //         targetHeading = AngleClamp(targetHeading);
// //         float difference = abs(targetHeading - heading);
// //         if (targetHeading > heading) {
// //             if (difference < 180)
// //                 heading += angularAcceleration * dt;
// //             else if (difference > 180)
// //                 heading -= angularAcceleration * dt;
// //         } else if (targetHeading < heading) {
// //             if (difference < 180)
// //                 heading -= angularAcceleration * dt;
// //             else if (difference > 180)
// //                 heading += angularAcceleration * dt;
// //         }
// //         if (difference < .5)
// //             heading = targetHeading;

// //         heading = AngleClamp(heading);
// //         raylib::Radian angle = raylib::Degree(heading);
// //         raylib::Vector3 velocity = {cos(angle) * speed, 0, -sin(angle) * speed};
// //         // Update the entity's position based on velocity
// //         transformComponent.position += velocity * dt;
// //         transformComponent.rotation.y = heading;
// //     }
// // }

// void PhysicsSystem(Scene& scene) {
//     static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
//         float decimal = float(angle) - int(angle);
//         int whole = int(angle) % 360;
//         whole += (whole < 0) * 360;
//         return decimal + whole;
//     };
    
//     // Iterate over all entities in the scene
//     for (Entity e = 0; e < scene.entityMasks.size(); e++) {
//         std::cout << "i have physics " << std::endl;
//         if (!scene.hasComponent<transformcomp>(e)) continue;
//         if (!scene.hasComponent<physics>(e)) continue;
//         std::cout << "i am in physics " << std::endl;
//         // Get references to the transform and physics components
//         auto& transformComponent = scene.getComponent<transformcomp>(e);
//         auto& physicsComponent = scene.getComponent<physics>(e);

//         float speedValue = 15.0f;
//         raylib::Degree headingValue(15.0f);

//         // Assigning values to speed and heading pointers
//         physicsComponent.speed = &speedValue;
//         physicsComponent.heading = &headingValue;

//         // Calculate velocity based on physics parameters
//         float acceleration = physicsComponent.acceleration;
//         float angularAcceleration = physicsComponent.angularAcceleration;
//         float targetSpeed = physicsComponent.targetSpeed;
//         raylib::Degree targetHeading = physicsComponent.targetHeading;
//         float maxSpeed = physicsComponent.maxSpeed;
//         float minSpeed = physicsComponent.minSpeed;
//         float* speedPtr = physicsComponent.speed;
//         raylib::Degree* headingPtr = physicsComponent.heading;

//         if (speedPtr == nullptr || headingPtr == nullptr) {
//         std::cerr << "Error: Invalid pointers!" << std::endl;
//         continue; // or handle the error appropriately
// }

//         // Dereference pointers to obtain speed and heading
//         float speed = *speedPtr;
//         raylib::Degree heading = *headingPtr;

//         float dt = physicsComponent.dt;

//         // Calculate new speed and heading using physics calculations
//         float target = Clamp(targetSpeed, minSpeed, maxSpeed);
//         if (speed < target)
//             speed += acceleration * dt;
//         else if (speed > target)
//             speed -= acceleration * dt;
//         speed = Clamp(speed, minSpeed, maxSpeed);

//         targetHeading = AngleClamp(targetHeading);
//         float difference = abs(targetHeading - heading);
//         if (targetHeading > heading) {
//             if (difference < 180)
//                 heading += angularAcceleration * dt;
//             else if (difference > 180)
//                 heading -= angularAcceleration * dt;
//         } else if (targetHeading < heading) {
//             if (difference < 180)
//                 heading -= angularAcceleration * dt;
//             else if (difference > 180)
//                 heading += angularAcceleration * dt;
//         }
//         if (difference < .5)
//             heading = targetHeading;
        
//         std::cout << "current speed: " << speed << std::endl;
//         heading = AngleClamp(heading);
//         raylib::Radian angle = raylib::Degree(heading);
//         raylib::Vector3 velocity = {cos(angle) * speed, 0, -sin(angle) * speed};

//         // Update the entity's position based on velocity
//         transformComponent.position += velocity * dt;
//         transformComponent.rotation.y = heading;

//         // Update the values of speed and heading in the physics component
//         *speedPtr = speed;
//         *headingPtr = heading;
//     }
// }

