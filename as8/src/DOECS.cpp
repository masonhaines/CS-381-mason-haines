// Mason Haines as8
#include <raylib-cpp.hpp>
#include <cstddef>
#include <cassert>
#include <cstdint>
#include <limits>
// #include <string>
#include <vector>
#include "rlgl.h"
#include "skybox.hpp"
#include "raylib.h"
// #include <ranges>
// #include <memory>
#include <iostream>
// #include "Counter.cpp"

template<typename T>
concept Transformer = requires( T t, raylib::Transform m) {
    {t.operator()(m)}->std::convertible_to<raylib::Transform>;
};

using Entity = uint8_t;

// #include <memory>
// #include <concepts>
#include <vector>
// #include <deque>
#include <iostream>
// #include <ranges>
// #include <bitset>
// #include <span>
// #include <variant>
#include <cassert>

extern size_t globalComponentCounter;
// namespace cs381 {
	template<typename T>
	size_t GetComponentID(T reference = {}) {
		static size_t id = globalComponentCounter++;
		return id;
	}

	using Entity = uint8_t; 

	struct ComponentStorage {
		size_t elementSize = -1;
		std::vector<std::byte> data;

		ComponentStorage() : elementSize(-1), data(1, std::byte{0}) {}
		ComponentStorage(size_t elementSize) : elementSize(elementSize) { data.reserve(5 * elementSize); }
		
		template<typename Tcomponent>
		ComponentStorage(Tcomponent reference = {}) : ComponentStorage(sizeof(Tcomponent)) {}

		template<typename Tcomponent>
		Tcomponent& Get(Entity e) {
			assert(sizeof(Tcomponent) == elementSize);
			assert(e < (data.size() / elementSize));
			return *(Tcomponent*)(data.data() + e * elementSize);
		}

		template<typename Tcomponent>
		std::pair<Tcomponent&, size_t> Allocate(size_t count = 1) {
			assert(sizeof(Tcomponent) == elementSize);
			assert(count < 100);
			auto originalEnd = data.size();
			data.insert(data.end(), elementSize * count, std::byte{0});
			for(size_t i = 0; i < count - 1; i++) // Skip the last one
				new(data.data() + originalEnd + i * elementSize) Tcomponent();
			return {
				*new(data.data() + data.size() - elementSize) Tcomponent(),
				data.size() / elementSize
			};
		}

		template<typename Tcomponent>
		Tcomponent& GetOrAllocate(Entity e) {
			assert(sizeof(Tcomponent) == elementSize);
			size_t size = data.size() / elementSize;
			if (size <= e)
				Allocate<Tcomponent>(std::max<int64_t>(int64_t(e) - size, 1));
			return Get<Tcomponent>(e);
		}
	};


	template<typename Storage = ComponentStorage>
	struct Scene {
		std::vector<std::vector<bool>> entityMasks;
		std::vector<Storage> storages = {Storage()};

		template<typename Tcomponent>
		Storage& GetStorage() {
			size_t id = GetComponentID<Tcomponent>();
			if(storages.size() <= id)
				storages.insert(storages.cend(), std::max<int64_t>(id - storages.size(), 1), Storage());
			if (storages[id].elementSize == std::numeric_limits<size_t>::max())
				storages[id] = Storage(Tcomponent{});
			return storages[id];
		}

		Entity CreateEntity() {
			Entity e = entityMasks.size();
			entityMasks.emplace_back(std::vector<bool>{false});
			return e;
		}

		template<typename Tcomponent>
		Tcomponent& AddComponent(Entity e) {
			size_t id = GetComponentID<Tcomponent>();
			auto& eMask = entityMasks[e];
			if(eMask.size() <= id)
				eMask.resize(id + 1, false);
			eMask[id] = true;
			return GetStorage<Tcomponent>().template GetOrAllocate<Tcomponent>(e);
		}

		template<typename Tcomponent>
		void RemoveComponent(Entity e) {
			size_t id = GetComponentID<Tcomponent>();
			auto& eMask = entityMasks[e];
			if(eMask.size() > id)
				eMask[id] = false;
		}

		template<typename Tcomponent>
		Tcomponent& GetComponent(Entity e) {
			size_t id = GetComponentID<Tcomponent>();
			assert(entityMasks[e][id]);
			return GetStorage<Tcomponent>().template Get<Tcomponent>(e);
		}

		template<typename Tcomponent>
		bool HasComponent(Entity e) {
			size_t id = GetComponentID<Tcomponent>();
			return entityMasks.size() > e && entityMasks[e].size() > id && entityMasks[e][id];
		}
	};




	// Niceties!

	struct SkiplistComponentStorage {
		size_t elementSize = -1;
		std::vector<size_t> indecies;
		std::vector<std::byte> data;

		SkiplistComponentStorage() : elementSize(-1), indecies(1, -1), data(1, std::byte{0}) {}
		SkiplistComponentStorage(size_t elementSize) : elementSize(elementSize) { data.reserve(5 * elementSize); }
		
		template<typename Tcomponent>
		SkiplistComponentStorage(Tcomponent reference = {}) : SkiplistComponentStorage(sizeof(Tcomponent)) {}

		template<typename Tcomponent>
		Tcomponent& Get(Entity e) {
			assert(sizeof(Tcomponent) == elementSize);
			assert(e < indecies.size());
			assert(indecies[e] != std::numeric_limits<size_t>::max());
			return *(Tcomponent*)(data.data() + indecies[e]);
		}

		template<typename Tcomponent>
		std::pair<Tcomponent&, size_t> Allocate() {
			assert(sizeof(Tcomponent) == elementSize);
			data.insert(data.end(), elementSize, std::byte{0});
			return {
				*new(data.data() + data.size() - elementSize) Tcomponent(),
				(data.size() - 1) / elementSize
			};
		}

		template<typename Tcomponent>
		Tcomponent& Allocate(Entity e) {
			auto [ret, i] = Allocate<Tcomponent>();
			indecies[e] = i * elementSize;
			return ret;
		}

		template<typename Tcomponent>
		Tcomponent& GetOrAllocate(Entity e) {
			assert(sizeof(Tcomponent) == elementSize);
			if (indecies.size() <= e)
				indecies.insert(indecies.end(), std::max<int64_t>(int64_t(e) - indecies.size(), 1), -1);
			if (indecies[e] == std::numeric_limits<size_t>::max())
				return Allocate<Tcomponent>(e);
			return Get<Tcomponent>(e);
		}
	};


	using post_increment_t = int;

	template<typename... Tcomponents>
	struct SceneView {
		Scene<SkiplistComponentStorage>& scene;

		struct Sentinel {};
		struct Iterator {
			Scene<SkiplistComponentStorage>* scene = nullptr;
			Entity e;

			bool valid() { return (scene->HasComponent<Tcomponents>(e) && ...); }

			bool operator==(Sentinel) { return scene == nullptr || e >= scene->entityMasks.size(); }

			Iterator& operator++(post_increment_t) { 
				do {
					e++;
				} while(!valid() && e < scene->entityMasks.size());
				return *this;
			}
			Iterator operator++() {
				Iterator old;
				operator++(0);
				return old;
			}

			// Entity operator*() { return e; }
			std::tuple<std::add_lvalue_reference_t<Tcomponents>...> operator*() { return { scene->GetComponent<Tcomponents>(e)... }; }
		};

		Iterator begin() { 
			Iterator out{&scene, 0}; 
			if(!out.valid()) ++out;
			return out;
		}
		Sentinel end() { return {}; }
	};
// }


struct physics {

	// static constexpr
    float acceleration;
	float angularAcceleration;

	float targetSpeed;
	raylib::Degree targetHeading;
	float speed;
	raylib::Degree heading;
	float dt;

	float maxSpeed;
	float minSpeed;

    //  physics(float acc = 1.0f, float angAcc = 5.0f, float targSpeed = 5.0f, float targHeading = 0.0f, float SPEED = 0.0f, raylib::Degree HEADING = 0.0f, float deltaTime = 0.0f, float maxSpd = 0.0f, float minSpd = 0.0f)
    //     : acceleration(acc), angularAcceleration(angAcc), targetSpeed(targSpeed), targetHeading(targHeading), speed(SPEED), heading(HEADING), dt(deltaTime), maxSpeed(maxSpd), minSpeed(minSpd) {}
    // physics(float& speedRef, raylib::Degree& headingRef)
    //     : acceleration(0), angularAcceleration(0), targetSpeed(0), targetHeading(0),
    //       speed(speedRef), heading(headingRef), dt(0), maxSpeed(0), minSpeed(0) {}
};

struct Rendering {
    raylib::Model* model;
    bool drawBoundingBox = false;
};

struct transformcomp {
    raylib::Vector3 position;
    raylib::Vector3 scale;
    raylib::Quaternion rotation;
};

void DrawBoundedModel(raylib::Model& model, Transformer auto transformer);
void DrawModel(raylib::Model& model, Transformer auto transformer);
void PhysicsSystem(Scene<ComponentStorage>& scene, float DT);
void DrawSystem(Scene<ComponentStorage>& scene);
bool ProcessInputSystem(raylib::Degree& planeTargetHeading, float& planeTargetSpeed, size_t& selectedPlane);



void DrawSystem(Scene<ComponentStorage>& scene) {

    // raylib::Color color;

    for(Entity e = 0; e < scene.entityMasks.size(); e++) {
        if(!scene.HasComponent<Rendering>(e)) continue;
        if(!scene.HasComponent<transformcomp>(e)) continue;
        auto & rendering = scene.GetComponent<Rendering>(e);
        auto & transformComponent = scene.GetComponent<transformcomp>(e);

        auto[axis, angle] = scene.GetComponent<transformcomp>(e).rotation.ToAxisAngle();
        rendering.model->transform = raylib::Transform(rendering.model->transform).Rotate(axis, angle);
                std::cout << "axis.x" << transformComponent.rotation.ToEuler().x << std::endl;
                std::cout << "axis.y" << transformComponent.rotation.ToEuler().y << std::endl;
                std::cout << "axis.z" << transformComponent.rotation.ToEuler().z << std::endl;
                
        // Vector3 Eul = scene.getComponent<transformcomp>(e).rotation.ToEuler();

        auto Transformer = [&transformComponent](raylib::Transform t) -> raylib::Transform {
            // auto[axis, angle] = scene.getComponent<transformcomp>(e).rotation.ToAxisAngle();
            // rendering.model->transform = raylib::Transform(rendering.model->transform).Rotate(axis, angle);

            return t.
            Translate(transformComponent.position).
            Scale(transformComponent.scale.x, transformComponent.scale.y, transformComponent.scale.z).
            // RotateXYZ(axis);
            RotateXYZ(transformComponent.rotation.x, transformComponent.rotation.y, transformComponent.rotation.z); // q1 is word xyz  q2 is incremetn 
        };
        
        if (rendering.drawBoundingBox) {
            DrawBoundedModel(*rendering.model, Transformer);
        } else {
            DrawModel(*rendering.model, Transformer);
        }
    }
}

int main() {
	// Create window
	const int screenWidth = 900 * 2;
	const int screenHeight = 450 * 2;
	raylib::Window window(screenWidth, screenHeight, "CS381 - Assignment 8");

    float DT = 0;

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
	auto mesh = raylib::Mesh::Plane(10000, 10000, 50, 50, 125);
	raylib::Model ground = ((raylib::Mesh*)&mesh)->LoadModelFrom();
	raylib::Texture water("textures/water.jpg");
	water.SetFilter(TEXTURE_FILTER_BILINEAR);
	water.SetWrap(TEXTURE_WRAP_REPEAT);
	ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = water;

    raylib::Model plane("meshes/PolyPlane.glb");
    Scene scene;
    auto e = scene.CreateEntity();
    // Add a transform component to the entity
    scene.AddComponent<Rendering>(e) = {&plane, true}; // Plane with no bounding box, ie false 
    scene.AddComponent<transformcomp>(e).position = {(Vector3){0, 90, 1}};
    scene.AddComponent<transformcomp>(e).scale = {(Vector3){5,5,5}};
    scene.AddComponent<transformcomp>(e).rotation = {(Quaternion){0,90 * DEG2RAD,0 * DEG2RAD,1}};
    scene.AddComponent<physics>(e).acceleration = 5;
    scene.AddComponent<physics>(e).angularAcceleration = 15;
    scene.AddComponent<physics>(e).dt = window.GetFrameTime();
    scene.AddComponent<physics>(e).heading = {};
    scene.AddComponent<physics>(e).targetHeading = 0; 
    scene.AddComponent<physics>(e).speed = 1;
    scene.AddComponent<physics>(e).targetSpeed = 5;
    scene.AddComponent<physics>(e).minSpeed = 0;
    scene.AddComponent<physics>(e).maxSpeed = 25;
    // scene.AddComponent<transformcomp>(e).rotation = raylib::Quaternion::FromAxisAngle({0, 0, 0}, 1);
    // raylib::Quaternion rotationQuat = raylib::Quaternion::FromAxisAngle(raylib::Vector3{0, 1, 0}, 0);
    // Create a quaternion for rotating around the Y-axis by 90 degrees
    //   scene.AddComponent<transformcomp>(e).rotation = raylib::Quaternion::ToEuler({1,1,1});
    //    scene.AddComponent<transformcomp>(e).rotation = 
    // scene.AddComponent<transformcomp>(e).rotation = raylib::Quaternion::Normalize({1, 1, 1, 1});
    // scene.AddComponent<transformcomp>(e).rotation.ToEuler() = {(Vector3){5,5,5}};
    // scene.AddComponent<transformcomp>(e) = {raylib::Vector3{1, 90, 1}, 
    // raylib::Vector3{1, 90, 1}, 
    // raylib::Quaternion{1, 1, 1, 1}};

    
    size_t selectedPlane = 0;

	SetTargetFPS(60);

    ProcessInputSystem(blah, blaH, VLAH);
    

	// Main loop
	bool keepRunning = true;
	while(!window.ShouldClose() && keepRunning) {

        DT = window.GetFrameTime();
		
		// Rendering
		window.BeginDrawing();
		{
			// Clear screen
			window.ClearBackground(BLACK);

			camera.BeginMode();
			{	
				
				
				// Render skybox and ground
				skybox.Draw();
				ground.Draw({});

                
				
			}
            PhysicsSystem(scene, DT);
            DrawSystem(scene);
			camera.EndMode();
            

			int height = window.GetHeight();
        	int width = window.GetWidth();

			DrawFPS(10, 10);
		}
		window.EndDrawing();
	}

	return 0;
}

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

// Input handling
bool ProcessInputSystem(raylib::Degree& planeTargetHeading, float& planeTargetSpeed, size_t& selectedPlane) {
	static bool wPressedLastFrame = false, sPressedLastFrame = false;
	static bool aPressedLastFrame = false, dPressedLastFrame = false;
	static bool tabPressedLastFrame = false;

	// If we hit escape... shutdown
	if(IsKeyDown(KEY_ESCAPE))
		return false;

	// WASD updates plane velocity
	if(IsKeyDown(KEY_W) && !wPressedLastFrame)
		planeTargetSpeed += 1;
	if(IsKeyDown(KEY_S) && !sPressedLastFrame)
		planeTargetSpeed -= 1;
	if(IsKeyDown(KEY_A) && !aPressedLastFrame)
		planeTargetHeading += 5;
	if(IsKeyDown(KEY_D) && !dPressedLastFrame)
		planeTargetHeading -= 5;  

	// Space sets velocity to 0!
	if(IsKeyDown(KEY_SPACE))
		planeTargetSpeed = 0;

	// Tab selects the next plane
	if(IsKeyDown(KEY_TAB) && !tabPressedLastFrame)
		selectedPlane = (selectedPlane + 1) % 3;

	// Save the state of the key for next frame
	wPressedLastFrame = IsKeyDown(KEY_W);
	sPressedLastFrame = IsKeyDown(KEY_S);
	aPressedLastFrame = IsKeyDown(KEY_A);
	dPressedLastFrame = IsKeyDown(KEY_D);

	tabPressedLastFrame = IsKeyDown(KEY_TAB);

	return true;
}

// raylib::Vector3 CaclulateVelocity(const CalculateVelocityParams& data) {
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

// Physics System
// void PhysicsSystem(Scene& scene) {
//     static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
// 		float decimal = float(angle) - int(angle);
// 		int whole = int(angle) % 360;
// 		whole += (whole < 0) * 360;
// 		return decimal + whole;
// 	};
//     // Iterate over all entities in the scene
//     for (Entity e = 0; e < scene.entityMasks.size(); e++) {
//         if (!scene.hasComponent<transformcomp>(e)) continue;
//         if (!scene.hasComponent<physics>(e)) continue;
//         // Check if the entity has transform and physics components
//         // if (!scene.hasComponent<transformcomp>(e) || !scene.hasComponent<physics>(e))
//         //     continue;
//         // Get references to the transform and physics components
//         auto& transformComponent = scene.getComponent<transformcomp>(e);
//         auto& physicsComponent = scene.getComponent<physics>(e);
//         // Calculate velocity based on physics parameters
//         float acceleration = physicsComponent.acceleration;
//         float angularAcceleration = physicsComponent.angularAcceleration;
//         float targetSpeed = physicsComponent.targetSpeed;
//         raylib::Degree targetHeading = physicsComponent.targetHeading;
//         float maxSpeed = physicsComponent.maxSpeed;
//         float minSpeed = physicsComponent.minSpeed;
//         float* speedPtr = physicsComponent.speed;
//         raylib::Degree* headingPtr = physicsComponent.heading;
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

//         heading = AngleClamp(heading);
//         raylib::Radian angle = raylib::Degree(heading);
//         raylib::Vector3 velocity = {cos(angle) * speed, 0, -sin(angle) * speed};
//         // Update the entity's position based on velocity
//         transformComponent.position += velocity * dt;
//         transformComponent.rotation.y = heading;
//     }
// }

void PhysicsSystem(Scene<ComponentStorage>& scene, float DT) {
    
    
    // Iterate over all entities in the scene
    for (Entity e = 0; e < scene.entityMasks.size(); e++) {

        static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
            float decimal = float(angle) - int(angle);
            int whole = int(angle) % 360;
            whole += (whole < 0) * 360;
            return decimal + whole;
        };

        std::cout << "i have physics " << std::endl;
        if (!scene.HasComponent<transformcomp>(e)) continue;
        if (!scene.HasComponent<physics>(e)) continue;
        std::cout << "i am in physics " << std::endl;
        // Get references to the transform and physics components
        auto& transformComponent = scene.GetComponent<transformcomp>(e);
        auto& physicsComponent = scene.GetComponent<physics>(e); // Declare physicsComponent here

        // Calculate velocity based on physics parameters
        float acceleration = physicsComponent.acceleration;
        float angularAcceleration = physicsComponent.angularAcceleration;
        float targetSpeed = physicsComponent.targetSpeed;
        raylib::Degree targetHeading = physicsComponent.targetHeading;
        float maxSpeed = physicsComponent.maxSpeed;
        float minSpeed = physicsComponent.minSpeed;
        float& speed = physicsComponent.speed;
        raylib::Degree& heading = physicsComponent.heading;
        // float dt = physicsComponent.dt;

        
        float target = Clamp(targetSpeed, minSpeed, maxSpeed);
        if (speed < target)
            speed += acceleration * DT;
        else if (speed > target)
            speed -= acceleration * DT;
        speed = Clamp(speed, minSpeed, maxSpeed);

        targetHeading = AngleClamp(targetHeading);
        float difference = abs(targetHeading - heading);
        if (targetHeading > heading) {
            if (difference < 180)
                heading += angularAcceleration * DT;
            else if (difference > 180)
                heading -= angularAcceleration * DT;
        } else if (targetHeading < heading) {
            if (difference < 180)
                heading -= angularAcceleration * DT;
            else if (difference > 180)
                heading += angularAcceleration * DT;
        }
        if (difference < .5)
            heading = targetHeading;

        // Calculate velocity based on updated speed and heading
        heading = AngleClamp(heading);
        raylib::Radian angle = raylib::Degree(heading);
        raylib::Vector3 velocity = {cos(angle) * speed, 0, -sin(angle) * speed};

        // Update the entity's position based on velocity
        transformComponent.position.x += speed * DT;
        // transformComponent.rotation.y += speed * .05;


        std::cout << DT << " <--- this is the current dt from the physics." << std::endl;
        std::cout << angle << " <--- this is the current angle from the physics." << std::endl;
        std::cout << speed << " <--- this is the current speed from the physics." << std::endl;
        std::cout << heading << " <--- this is the current heading from the physics." << std::endl;

        // transformComponent.position += velocity * dt;
        // transformComponent.rotation.y = heading;

    }
}

