// Mason Haines as8
#include <raylib-cpp.hpp>
#include <cstddef>
#include <cassert>
#include <cstdint>
#include <limits>
#include <vector>
#include "rlgl.h"
#include "skybox.hpp"
#include "raylib.h"
#include <iostream>
#include <BufferedInput.hpp>
#include <vector>
#include <iostream>
#include <cassert>

template<typename T>
concept Transformer = requires( T t, raylib::Transform m) {
    {t.operator()(m)}->std::convertible_to<raylib::Transform>;
};

extern size_t globalComponentCounter;

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
		// std::cout << globalComponentCounter << "global counter for components" << std::endl;
		assert(e < (data.size() / elementSize));

		// std::cout << "assertion error value : e is " << static_cast<unsigned int>(e) << " data size divided by element size " << data.size() / elementSize << std::endl;
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



struct veloKinematics {
	float acceleration;
	
	raylib::Vector3 velocity;

	float targetSpeed;
    float speed;

	float maxSpeed;
    float minSpeed;
};

struct physics {
    
    float angularAcceleration;

    raylib::Quaternion rotation; // Using quaternion for rotation
    raylib::Quaternion targetRotation; // Target rotation 

	bool _2d = false;
};

struct TwoDphysics {};
// struct TwoDphysics {
    
//     float angularAcceleration;

//     raylib::Degree heading; 
//     raylib::Degree targetHeading; 
// };

struct Rendering {
    raylib::Model* model;
    bool drawBoundingBox = false;

	raylib::Color color;
};

struct transformcomp {
    raylib::Vector3 position;
    raylib::Vector3 scale;
    raylib::Quaternion rotation;
    bool jumping = false;
};

struct bufferedComponent {
	raylib::BufferedInput* inputs; // Manager for actions 
    bool selected = false;
	
};

void DrawBoundedModel(raylib::Model& model, const raylib::Color& color, Transformer auto transformer);
void DrawModel(raylib::Model& model, const raylib::Color& color, Transformer auto transformer);
void VelocitySystem(Scene<ComponentStorage>& scene, float dt);
void TwoDPhysicsSystem(Scene<ComponentStorage>& scene, float dt); // no used but functions
void ThreeDPhysicsSystem(Scene<ComponentStorage>& scene, float dt);
void DrawSystem(Scene<ComponentStorage>& scene);
void ProcessInputSystem(Scene<ComponentStorage>& scene);
void BoatProcessInputSystem(Scene<ComponentStorage>& scene); // not used but functions 

int main() {
	// Create window
	const int screenWidth = 900 * 2; // 900
	const int screenHeight = 450 * 2; // 450
	raylib::Window window(screenWidth, screenHeight, "CS381 - Assignment 8");

	raylib::BufferedInput inputs; // Manager for actions 

    raylib::Text text;
    float textSize = 25;
    const char *Title = {"Pickles"};

    float dt = 0;
	bool inputTabbed = false;
	bool changeCamera = false;

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
	raylib::Texture water("textures/floor.jpg");
	water.SetFilter(TEXTURE_FILTER_BILINEAR);
	water.SetWrap(TEXTURE_WRAP_REPEAT);
	ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = water;

	int numberOfPlanes = 1;
	int counter = 0; // Initialization for tab indexing 

    raylib::Model plane("meshes/PolyPlane.glb");
	raylib::Model character("meshes/pickle.glb");
	// raylib::Model boat2("meshes/OilTanker.glb");
	// raylib::Model boat3("meshes/CargoG_HOSBrigadoon.glb");
	// raylib::Model boat4("meshes/Container_ShipLarge.glb");
	// raylib::Model boat5("meshes/OrientExplorer.glb");
	
	// character.transform = raylib::Transform(character.transform).RotateY(raylib::Degree(0));
	// boat2.transform = raylib::Transform(boat2.transform).RotateX(raylib::Degree(-90)).RotateY(raylib::Degree(-90)).RotateZ(raylib::Degree(180));
	// boat3.transform = raylib::Transform(boat3.transform).RotateX(raylib::Degree(-90)).RotateY(raylib::Degree(-90)).RotateZ(raylib::Degree(180));
	// boat4.transform = raylib::Transform(boat4.transform).RotateX(raylib::Degree(-90)).RotateY(raylib::Degree(-90)).RotateZ(raylib::Degree(180));
	// boat5.transform = raylib::Transform(boat5.transform).RotateX(raylib::Degree(-90)).RotateY(raylib::Degree(-90)).RotateZ(raylib::Degree(180));
	
    Scene scene;
	
	// PLANES
   	for (int i = 0; i < numberOfPlanes; ++i) {
		auto e = scene.CreateEntity();

		scene.AddComponent<Rendering>(e) = {&character,false, GREEN}; // Plane with no bounding box, ie false 
		scene.AddComponent<transformcomp>(e) = {(Vector3){0, 0, 0}, (Vector3){1000,1000,1000}, QuaternionIdentity(), false}; 
		scene.AddComponent<physics>(e) = {15, QuaternionIdentity(), QuaternionIdentity(), true};
		scene.AddComponent<veloKinematics>(e) = {15,(Vector3){0, 0, 0}, 0, 
		0, 25, 0};
		scene.AddComponent<bufferedComponent>(e) = {&inputs, false};
	}
	
	// Boat 1 SmitHouston_Tug
	// auto b1 = scene.CreateEntity();
	// scene.AddComponent<Rendering>(b1) = {&character, false, GRAY}; 
	// scene.AddComponent<transformcomp>(b1) = {(Vector3){-100, 0, -50}, (Vector3){3,3,3}, QuaternionIdentity()}; // Adjust position based on 'i'
	// scene.AddComponent<physics>(b1) = {5, QuaternionIdentity(), QuaternionIdentity(), true};
	// scene.AddComponent<veloKinematics>(b1) = {6, (Vector3){0, 0, 0},
	// 5, 5, 50, 0};
	// scene.AddComponent<bufferedComponent>(b1) = {&inputs, false};
	// // Boat 2 OilTanker
	// auto b2 = scene.CreateEntity();
	// scene.AddComponent<Rendering>(b2) = {&boat2, false, ORANGE}; 
	// scene.AddComponent<transformcomp>(b2) = {(Vector3){-200, 0, 0}, (Vector3){.015,.015,.015}, QuaternionIdentity()}; // Adjust position based on 'i'
	// scene.AddComponent<physics>(b2) = {.05, QuaternionIdentity(), QuaternionIdentity(), true};
	// scene.AddComponent<veloKinematics>(b2) = {.5, (Vector3){0, 0, 0}, 
	// 5,5, 40, 0};
	// scene.AddComponent<bufferedComponent>(b2) = {&inputs, false};
	// // Boat 3 CargoG_HOSBrigadoon
	// auto b3 = scene.CreateEntity();
	// scene.AddComponent<Rendering>(b3) = {&boat3, false, BEIGE}; 
	// scene.AddComponent<transformcomp>(b3) = {(Vector3){-300, 0, 200}, (Vector3){.04,.04,.04}, QuaternionIdentity()}; 
	// scene.AddComponent<physics>(b3) = {.3, QuaternionIdentity(), QuaternionIdentity(), true};
	// scene.AddComponent<veloKinematics>(b3) = {.2, (Vector3){0, 0, 0}, 
	// 5, 5, 30, 0};
	// scene.AddComponent<bufferedComponent>(b3) = {&inputs, false};
	// // Boat 4 Container_ShipLarge
	// auto b4 = scene.CreateEntity();
	// scene.AddComponent<Rendering>(b4) = {&boat4, false, BLUE}; 
	// scene.AddComponent<transformcomp>(b4) = {(Vector3){100, 0, 100}, (Vector3){.01, .01, .01}, QuaternionIdentity()}; 
	// scene.AddComponent<physics>(b4) = {.02, QuaternionIdentity(), QuaternionIdentity(), true};
	// scene.AddComponent<veloKinematics>(b4) = {.1, (Vector3){0, 0, 0}, 
	// 5, 5, 20, 0};
	// scene.AddComponent<bufferedComponent>(b4) = {&inputs, false};
	// // Boat 5 OrientExplorer
	// auto b5 = scene.CreateEntity();
	// scene.AddComponent<Rendering>(b5) = {&boat5,false, GREEN};
	// scene.AddComponent<transformcomp>(b5) = {(Vector3){200, 0, 0}, (Vector3){.005,.005,.005}, QuaternionIdentity()};
	// scene.AddComponent<physics>(b5) = {15,QuaternionIdentity(), QuaternionIdentity(),true};
	// scene.AddComponent<veloKinematics>(b5) = {30, (Vector3){0, 0, 0}, 
	// 5, 5, 400, 0};
	// scene.AddComponent<bufferedComponent>(b5) = {&inputs,false};

	// Buffered input actions for setup in buffered input component
	inputs["forward"] = raylib::Action::key(KEY_W).move();
	inputs["backwards"] = raylib::Action::key(KEY_S).move();
	inputs["right"] = raylib::Action::key(KEY_D).move();
	inputs["left"] = raylib::Action::key(KEY_A).move();
	inputs["tiltleft"] = raylib::Action::key(KEY_F).move();
	inputs["tiltright"] = raylib::Action::key(KEY_R).move();
	inputs["tiltback"] = raylib::Action::key(KEY_E).move();
	inputs["tiltforward"] = raylib::Action::key(KEY_Q).move();
	inputs["space"] = raylib::Action::key(KEY_SPACE).move(); // spin jump
	inputs["change"] = raylib::Action::key(KEY_TAB).move();
    

	// inputs["change"] = raylib::Action::key(KEY_TAB).SetPressedCallback([&scene, &counter, &inputTabbed](){
	// 	counter = (counter + 1) % scene.entityMasks.size();
	// 	inputTabbed = true;
	// 	for (Entity e = 0; e < scene.entityMasks.size(); e++) {
	// 		if(!scene.HasComponent<bufferedComponent>(e)) continue; // get optional reference to transform component
	// 		auto& bufInput = scene.GetComponent<bufferedComponent>(e); // get values stored in reference if it exists
	// 		bufInput.selected = counter == e;
	// 	}
	// }).move();

	SetTargetFPS(60);
    ProcessInputSystem(scene); // setup process input for incremented key input 

	// Main loop
	bool keepRunning = true;
	while (!window.ShouldClose() && keepRunning) { // Main loop: Continue running while the window is open and keepRunning is true

		inputs.PollEvents(); // Poll buffered input from user
		dt = window.GetFrameTime(); // Get the time elapsed since the last frame

		// Rendering
		window.BeginDrawing(); // Begin drawing
		{
			// Clear screen
			window.ClearBackground(BLACK);

			if (IsKeyReleased(KEY_C)) { // Check if the 'C' key is released to toggle camera view
				if (changeCamera == false) changeCamera = true; // Toggle camera view
				else changeCamera = false; // Toggle camera view off
			}

			camera.BeginMode(); // Begin camera mode
			{
				if (counter == 10) camera.fovy = 65.0; // Change field of view for the largest ship

				if (changeCamera) { // If changing camera and input is tabbed
					// Target the position of the current selected model
					for (Entity e = 0; e < scene.entityMasks.size(); e++) {
						if (!scene.HasComponent<bufferedComponent>(e)) continue; // Check if the entity has a buffered component
						auto& bufInput = scene.GetComponent<bufferedComponent>(e); // Get buffered input component

						bufInput.selected = counter == e; // Set selected status based on the counter value

						// Set camera target and position based on the selected model's position
						camera.target = (Vector3){
							scene.GetComponent<transformcomp>(counter).position.x,
							scene.GetComponent<transformcomp>(counter).position.y,
							scene.GetComponent<transformcomp>(counter).position.z
						};
						camera.position = (Vector3){
							scene.GetComponent<transformcomp>(counter).position.x - 300,
							scene.GetComponent<transformcomp>(counter).position.y + 355,
							scene.GetComponent<transformcomp>(counter).position.z
						};
						camera.up = raylib::Vector3::Up(); // Set camera up direction
						camera.fovy = (counter == 9) ? 60.0 : 35.0f; // Set camera field of view
						camera.projection = CAMERA_PERSPECTIVE; // Set camera projection
					}
				} else { // If not changing camera
					// Revert to old camera settings
					camera.target = raylib::Vector3(0, 0, 300); // Set camera target
					camera.position = raylib::Vector3(0, 120, -500); // Set camera position
					camera.up = raylib::Vector3::Up(); // Set camera up direction
					camera.fovy = 50.0f; // Set camera field of view
					camera.projection = CAMERA_PERSPECTIVE; // Set camera projection
				}

				// Render skybox and ground
				skybox.Draw(); // Draw skybox
				ground.Draw({}); // Draw ground
			}
			VelocitySystem(scene, dt); // Update velocity based on the scene and time
			ThreeDPhysicsSystem(scene, dt); // Update 3D physics based on the scene and time
			DrawSystem(scene); // Draw the scene
			camera.EndMode(); // End camera mode

			// Get window height and width
			int height = window.GetHeight();
			int width = window.GetWidth();

			DrawFPS(10, 10); // Draw frames per second counter

            text.Draw(Title, (width / 100), height * .91, textSize * 3.0, raylib::Color::RayWhite());
		}
		window.EndDrawing(); // End drawing
	}

	return 0; // Return 0 when the loop exits

}

// Function to draw a bounded model with a specified transformation
void DrawBoundedModel(raylib::Model& model, const raylib::Color& color, Transformer auto transformer) {
    raylib::Transform backupTransform = model.transform; // Save the model's transform
    model.transform = transformer(backupTransform); // Update the model's transform based on the transformation function
    model.Draw({}, 1.0f, color);
    model.GetTransformedBoundingBox().Draw(raylib::Color(ORANGE));
    model.transform = backupTransform; // Restore the original transform
}

// Function to draw a model with a specified transformation
void DrawModel(raylib::Model& model, const raylib::Color& color, Transformer auto transformer) {
    raylib::Transform backupTransform = model.transform; // Save the model's transform
    model.transform = transformer(backupTransform); // Update the model's transform based on the transformation function
    model.Draw({}, 1.0f, color);
    model.transform = backupTransform; // Restore the original transform
}

void DrawSystem(Scene<ComponentStorage>& scene) {
    for(Entity e = 0; e < scene.entityMasks.size(); e++) {
        if(!scene.HasComponent<Rendering>(e)) continue;
        if(!scene.HasComponent<transformcomp>(e)) continue;
		if(!scene.HasComponent<bufferedComponent>(e)) continue;
        auto & rendering = scene.GetComponent<Rendering>(e);
        auto & transformComponent = scene.GetComponent<transformcomp>(e);
		auto & buffer = scene.GetComponent<bufferedComponent>(e);

		raylib::Color color = rendering.color;

        auto Transformer = [&transformComponent](raylib::Transform t) -> raylib::Transform {
			auto [axis, angle] = transformComponent.rotation.ToAxisAngle();
            return t.
            Translate(transformComponent.position).
            Scale(transformComponent.scale.x, transformComponent.scale.y, transformComponent.scale.z).Rotate(axis, angle);
        };
        
        // if (buffer.selected) {
        //     DrawBoundedModel(*rendering.model, color, Transformer);
        // } else {
            DrawModel(*rendering.model, color, Transformer);
        // }
    }
}


void ProcessInputSystem(Scene<ComponentStorage>& scene) {

	for(Entity e = 0; e < scene.entityMasks.size(); e++) {

		if(!scene.HasComponent<bufferedComponent>(e)) continue;
        if(!scene.HasComponent<transformcomp>(e)) continue;
		if(!scene.HasComponent<physics>(e)) continue;
		if(!scene.HasComponent<veloKinematics>(e)) continue;
		auto& buffer = scene.GetComponent<bufferedComponent>(e);
		auto& physicsComponent = scene.GetComponent<physics>(e); // Declare physicsComponent here
		auto& kinematics = scene.GetComponent<veloKinematics>(e);
        auto& transformComponent = scene.GetComponent<transformcomp>(e);

		bool& is2D = physicsComponent._2d;

		(*buffer.inputs)["forward"].AddPressedCallback([&kinematics, &buffer]()-> void {   
            kinematics.targetSpeed += 10;// thus doing   
        });
        (*buffer.inputs)["backwards"].AddPressedCallback([&kinematics, &buffer]()-> void {           
			kinematics.targetSpeed -= 2; // thus doing 
        });
        (*buffer.inputs)["right"].AddPressedCallback([&physicsComponent, &buffer]()-> void { 
			physicsComponent.targetRotation = physicsComponent.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Down(), raylib::Degree(15));  
        });
        (*buffer.inputs)["left"].AddPressedCallback([&physicsComponent, &buffer]()-> void { 
			physicsComponent.targetRotation = physicsComponent.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Up(), raylib::Degree(15)); 
        });
		// (*buffer.inputs)["tiltright"].AddPressedCallback([&physicsComponent, &buffer, &is2D]()-> void { 
		// 	physicsComponent.targetRotation = physicsComponent.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Right(), raylib::Degree(10));  
        // });
		// (*buffer.inputs)["tiltleft"].AddPressedCallback([&physicsComponent, &buffer, &is2D]()-> void { 
		// 	physicsComponent.targetRotation = physicsComponent.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Left(), raylib::Degree(10));
		// });
		// (*buffer.inputs)["tiltforward"].AddPressedCallback([&physicsComponent, &buffer, &is2D]()-> void { 
		// 	physicsComponent.targetRotation = physicsComponent.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Forward(), raylib::Degree(12));
		// });
		// (*buffer.inputs)["tiltback"].AddPressedCallback([&physicsComponent, &buffer, &is2D]()-> void { 
		// 	physicsComponent.targetRotation = physicsComponent.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Back(), raylib::Degree(12));
		// });
        (*buffer.inputs)["space"].AddPressedCallback([&physicsComponent, &buffer, &transformComponent]()-> void { 
			// physicsComponent.targetRotation = physicsComponent.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Up(), raylib::Degree(180)); 
            // transformComponent.position.y = transformComponent.position.y + 25;
            // if (!transformComponent.jumping) transformComponent.jumping = true;
            
        });
		
		
	}
}

void VelocitySystem(Scene<ComponentStorage>& scene, float dt) {
	for (Entity e = 0; e < scene.entityMasks.size(); e++) {
		if (!scene.HasComponent<veloKinematics>(e)) continue;
		if (!scene.HasComponent<transformcomp>(e)) continue;
		
		auto& kinematics = scene.GetComponent<veloKinematics>(e);
		auto& transformComponent = scene.GetComponent<transformcomp>(e);

		raylib::Vector3 velocity = kinematics.velocity;
		float targetSpeed = kinematics.targetSpeed;
        float maxSpeed = kinematics.maxSpeed;
        float minSpeed = kinematics.minSpeed;
        float& speed = kinematics.speed;
		float acceleration = kinematics.acceleration;

		float target = Clamp(targetSpeed, minSpeed, maxSpeed);
        if (speed < target)
            speed += acceleration * dt;
        else if (speed > target)
            speed -= acceleration * dt;
        speed = Clamp(speed, minSpeed, maxSpeed);


        transformComponent.position += velocity * dt;
		// Update position based on velocity
        // if(transformComponent.jumping) transformComponent.position.y += 3 * dt;
        // if (transformComponent.position.y < .05) transformComponent.position = 0;
        // if (transformComponent.position.y > 4.9) {
        //     transformComponent.position.y -= 1 * dt;
        //     transformComponent.jumping = false;
        // }
            
		
	}
}

void ThreeDPhysicsSystem(Scene<ComponentStorage>& scene, float dt) {
    for (Entity e = 0; e < scene.entityMasks.size(); e++) {
        if (!scene.HasComponent<transformcomp>(e)) continue;
        if (!scene.HasComponent<physics>(e)) continue;
		if (!scene.HasComponent<veloKinematics>(e)) continue;

        auto& transformComponent = scene.GetComponent<transformcomp>(e);
        auto& physicsComponent = scene.GetComponent<physics>(e);
		auto& kinematics = scene.GetComponent<veloKinematics>(e); 
		auto& render = scene.GetComponent<Rendering>(e);

        // Update speed based on physics parameters
		raylib::Vector3& velocity = kinematics.velocity;
		float& angularAcceleration = physicsComponent.angularAcceleration;
		raylib::Quaternion& rotation = physicsComponent.rotation;
        raylib::Quaternion& targetRotation = physicsComponent.targetRotation;
		bool& is2D = physicsComponent._2d;

		float& speed = kinematics.speed;

       	rotation = QuaternionSlerp(rotation, targetRotation, angularAcceleration * dt); 
		
		// Calculate velocity based on updated speed and rotation
		velocity = raylib::Vector3::Left().RotateByQuaternion(rotation) * speed;

        if (transformComponent.jumping) physicsComponent.targetRotation = physicsComponent.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Up(), raylib::Degree(10));
		transformComponent.rotation = rotation;

    }
}

// void TwoDPhysicsSystem(Scene<ComponentStorage>& scene, float dt) {

// 	for (Entity e = 0; e < scene.entityMasks.size(); e++) {

//         if (!scene.HasComponent<transformcomp>(e)) continue;
//         if (!scene.HasComponent<TwoDphysics>(e)) continue;
// 		if (!scene.HasComponent<veloKinematics>(e)) continue;

//         auto& transformComponent = scene.GetComponent<transformcomp>(e);
//         auto& TwoDphysicsComponent = scene.GetComponent<TwoDphysics>(e);
// 		auto& kinematics = scene.GetComponent<veloKinematics>(e);

// 		// static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
// 		// 	int intPart = angle;
// 		// 	float floatPart = float(angle) - intPart;
// 		// 	intPart %= 360;
// 		// 	intPart += (intPart < 0) * 360;
// 		// 	return intPart + floatPart;
// 		// }; 

// 		raylib::Vector3& velocity = kinematics.velocity;
// 		float& speed = kinematics.speed;
// 		float angularAcceleration = TwoDphysicsComponent.angularAcceleration;
// 		raylib::Degree& heading = TwoDphysicsComponent.heading;
// 		raylib::Degree& targetHeading = TwoDphysicsComponent.targetHeading;
//         float difference = abs(targetHeading - heading);

//         if (targetHeading > heading) {
//             if (difference < 180 * DEG2RAD)
//                 heading += angularAcceleration * dt;
//             else if (difference > 180 * DEG2RAD)
//                 heading -= angularAcceleration * dt;
//         } else if (targetHeading < heading) {
//             if (difference < 180 * DEG2RAD)
//                 heading -= angularAcceleration * dt;
//             else if (difference > 180 * DEG2RAD)
//                 heading += angularAcceleration * dt;
//         }

// 		std::cout << "<-----------Difference: " << difference << std::endl;
// 		std::cout << "<-----Updated Heading: " << heading << std::endl;
//         if (difference < .005) heading = targetHeading;

// 		std::cout << heading << " <---  heading ." << std::endl;
// 		std::cout << targetHeading << " <---  targetHeading ." << std::endl;
// 		std::cout << angularAcceleration << " <---  ang acc ." << std::endl;

// 		raylib::Quaternion rotation = raylib::Quaternion::FromAxisAngle({0, 1, 0}, heading);
// 		// From Chatgpt - "how do i add  ninety degrees to the heading for the following two lines of code?"
// 		raylib::Quaternion offsetY = raylib::Quaternion::FromAxisAngle({0, 1, 0}, PI / 2.0f);

// 		raylib::Quaternion newRotation = offsetY * rotation;
	
// 		velocity = raylib::Vector3::Left().RotateByQuaternion(rotation) * speed;

// 		transformComponent.rotation = newRotation;

// 		// std::cout << transformComponent.rotation.x << " <---  x rotation from the physics." << std::endl;
//         // std::cout << transformComponent.rotation.y << " <---  y rotation from the physics." << std::endl;
//         // std::cout << transformComponent.rotation.z << " <---  z rotation from the physics." << std::endl;
// 		// std::cout << transformComponent.rotation.w << " <---  w rotation from the physics." << std::endl;
// 	}
// }

// void BoatProcessInputSystem(Scene<ComponentStorage>& scene) {

// 	for(Entity e = 0; e < scene.entityMasks.size(); e++) {

// 		if(!scene.HasComponent<bufferedComponent>(e)) continue;
// 		if(!scene.HasComponent<veloKinematics>(e)) continue;
// 		auto& buffer = scene.GetComponent<bufferedComponent>(e);
// 		auto& kinematics = scene.GetComponent<veloKinematics>(e);
// 		if(!scene.HasComponent<TwoDphysics>(e)) {
// 			std::cout << "I DO NOT HAVE 2 D PHYSICS" << std::endl;
// 			continue;
// 		}
// 		auto& TwoDPhysicsComponent = scene.GetComponent<TwoDphysics>(e); 


// 		(*buffer.inputs)["forward"].AddPressedCallback([&kinematics, &buffer]()-> void {   
// 			if (buffer.selected)
//             kinematics.targetSpeed += 3;// thus doing   
//         });
//         (*buffer.inputs)["backwards"].AddPressedCallback([&kinematics, &buffer]()-> void {           
//             if (buffer.selected)
// 			kinematics.targetSpeed -= 1; // thus doing 
//         });
//         (*buffer.inputs)["right"].AddPressedCallback([&TwoDPhysicsComponent, &buffer]()-> void { 
//             if (buffer.selected) {
//                 TwoDPhysicsComponent.targetHeading -= 20 * DEG2RAD;
//                 std::cout << "Right key pressed. New Target Heading: " << TwoDPhysicsComponent.targetHeading << std::endl;
//             }
//         });
//         (*buffer.inputs)["left"].AddPressedCallback([&TwoDPhysicsComponent, &buffer]()-> void { 
//             if (buffer.selected) {
//                 TwoDPhysicsComponent.targetHeading += 20 * DEG2RAD;
//                 std::cout << "Left key pressed. New Target Heading: " << TwoDPhysicsComponent.targetHeading << std::endl;
//             }
//         });
//         (*buffer.inputs)["space"].AddPressedCallback([&kinematics, &buffer]()-> void { 
//             if (buffer.selected)
// 			kinematics.targetSpeed = 0;
//         });
// 	}
// }