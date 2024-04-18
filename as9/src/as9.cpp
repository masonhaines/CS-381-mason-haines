// Mason Haines as8
#include <raylib-cpp.hpp>
#include <cstddef>
#include <cassert>
#include <cstdint>
#include <limits>
#include <string>
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
		assert(e <= (data.size() / elementSize));

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

	bool bound = false;

};

struct physics {
    
    float angularAcceleration;

    raylib::Quaternion rotation; // Using quaternion for rotation
    raylib::Quaternion targetRotation; // Target rotation 

	// bool _2d = false;
};

struct flipFlag {};
// struct TwoDphysics {
    
//     float angularAcceleration;

//     raylib::Degree heading; 
//     raylib::Degree targetHeading; 
// };

struct Rendering {
    raylib::Model* model;
    bool drawBoundingBox = false;

	raylib::Color color;

	int counter = 0;
};

struct transformcomp {
    raylib::Vector3 position;
    raylib::Vector3 scale;
    raylib::Quaternion rotation;

    bool jumping = false;
	int jumpCounter = 0;
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
void DrawWorldSystem(Scene<ComponentStorage>& scene);
void ProcessInputSystem(Scene<ComponentStorage>& scene);
void BoatProcessInputSystem(Scene<ComponentStorage>& scene); // not used but functions 
void spinSystem(Scene<ComponentStorage>& scene, float dt);
void jumpSystem(Scene<ComponentStorage>& scene, float dt);

void CheckCollisions(Scene<ComponentStorage>& scene, Entity character1, Entity character2, Entity islands);




int main() {
	// Create window
	const int screenWidth = 400 * 2; // 900
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

	int numberOfIslands = 2;
	int counter = 0; // Initialization for tab indexing 

	raylib::Model character("meshes/pickle.glb");
	raylib::Model ducky("meshes/Duck.glb");
	raylib::Model island1("meshes/Island.glb");
	
	
	// character.transform = raylib::Transform(character.transform).RotateY(raylib::Degree(0));
	
    Scene scene;
	
	
   	// PICKLE
	auto pickle = scene.CreateEntity();
	scene.AddComponent<Rendering>(pickle) = {&character,true, WHITE}; 
	scene.AddComponent<transformcomp>(pickle) = {(Vector3){0, 0, 0}, (Vector3){1000,1000,1000}, QuaternionIdentity(), false}; 
	scene.AddComponent<physics>(pickle) = {15, QuaternionIdentity(), QuaternionIdentity()};
	scene.AddComponent<veloKinematics>(pickle) = {100,(Vector3){0, 0, 0}, 0, 
	0, 50, -50};
	scene.AddComponent<bufferedComponent>(pickle) = {&inputs, true};

	auto duck = scene.CreateEntity();
	scene.AddComponent<Rendering>(duck) = {&ducky,false, WHITE}; 
	scene.AddComponent<transformcomp>(duck) = {(Vector3){0, 0, 0}, (Vector3){3,3, 3}, QuaternionIdentity(), false}; 
	scene.AddComponent<physics>(duck) = {15, QuaternionIdentity(), QuaternionIdentity()};
	scene.AddComponent<veloKinematics>(duck) = {100,(Vector3){0, 0, 0}, 0, 
	0, 50, -50};
	scene.AddComponent<bufferedComponent>(duck) = {&inputs, false};
	scene.AddComponent<flipFlag>(duck);

	Entity island;

	for (int i = 1; i <= numberOfIslands; ++i) {
		island = scene.CreateEntity();
		scene.AddComponent<Rendering>(island) = {&island1,true, WHITE}; 
		scene.AddComponent<transformcomp>(island) = {(Vector3){150.0f * i, -70.0f + (i * 20), 35.0f * i}, (Vector3){100,100,100}, QuaternionIdentity()}; 
	}

	

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
				if (changeCamera) { // If changing camera and input is tabbed
					// Target the position of the current selected model
					for (Entity e = 0; e < scene.entityMasks.size(); e++) {
						if (!scene.HasComponent<bufferedComponent>(e)) continue; // Check if the entity has a buffered component
						auto& bufInput = scene.GetComponent<bufferedComponent>(e); // Get buffered input component

						// Set camera target and position based on the selected model's position
						camera.target = (Vector3){
							scene.GetComponent<transformcomp>(counter).position.x,
							scene.GetComponent<transformcomp>(counter).position.y,
							scene.GetComponent<transformcomp>(counter).position.z
						};
						camera.position = (Vector3){
							scene.GetComponent<transformcomp>(counter).position.x - 375,
							scene.GetComponent<transformcomp>(counter).position.y + 155,
							scene.GetComponent<transformcomp>(counter).position.z
						};
						camera.up = raylib::Vector3::Up(); // Set camera up direction
						camera.fovy = 35.0f; // Set camera field of view
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
			CheckCollisions(scene, pickle, duck, island);
			// jumpSystem(scene, dt);
			// spinSystem(scene, dt);
			VelocitySystem(scene, dt); // Update velocity based on the scene and time
			ThreeDPhysicsSystem(scene, dt); // Update 3D physics based on the scene and time
			DrawSystem(scene); // Draw the scene
			
			DrawWorldSystem(scene);
			// CheckCollisions(scene, pickle, duck, island);
			camera.EndMode(); // End camera mode

			// Get window height and width
			int height = window.GetHeight();
			int width = window.GetWidth();

			DrawFPS(10, 10); // Draw frames per second counter
			

            text.Draw(std::to_string(scene.GetComponent<transformcomp>(duck).jumpCounter), (width / 100), height * .91, textSize * 3.0, raylib::Color::RayWhite());
		}
		window.EndDrawing(); // End drawing
	}

	return 0; // Return 0 when the loop exits
}


void CheckCollisions(Scene<ComponentStorage>& scene, Entity character,Entity character2,  Entity islands) {

    int collisionCount = 0;

		auto& kinematics = scene.GetComponent<veloKinematics>(character);
		raylib::Vector3& velocity = kinematics.velocity;
	

        auto& characterTransform = scene.GetComponent<transformcomp>(character);
        auto& characterRender = scene.GetComponent<Rendering>(character);
        auto characterBox = characterRender.model->GetTransformedBoundingBox();
		// auto characterBox = characterRender.model->GetBoundingBox();

		raylib::Vector3 cPos = characterTransform.position;

		auto& character2Transform = scene.GetComponent<transformcomp>(character2);
        auto& character2Render = scene.GetComponent<Rendering>(character2);
        auto character2Box = character2Render.model->GetTransformedBoundingBox();
		// auto characterBox = characterRender.model->GetBoundingBox();

		raylib::Vector3 c2Pos = character2Transform.position;

		auto& islandsTransform = scene.GetComponent<transformcomp>(islands);
        auto& islandsRender = scene.GetComponent<Rendering>(islands);
        auto islandsBox = islandsRender.model->GetTransformedBoundingBox();
		// auto islandsBox = islandsRender.model->GetBoundingBox();

		raylib::Vector3 iPos = islandsTransform.position;

		// if (CheckCollisionBoxes(characterBox, islandsBox)) {
		
		// 	collisionCount++;
		// 	std::cout << " collision between character and island " << std::endl;
			
		// 	collisionCount++;
		// 	characterTransform.jumping = false;
		// 	kinematics.velocity.y = -kinematics.velocity.y;
			
		// }
		
		
			// if ((abs(cPos.y - iPos.y) < 90  && abs(cPos.x - iPos.x) < 50)) {
				
			// 	collisionCount++;
			// 	// characterTransform.jumping = false;
			// 	std::cout << " collision between character and island " << std::endl;
				
			// 	kinematics.bound = true;
			// 	if (cPos.y > iPos.y) {
			// 		characterTransform.position.y = iPos.y + 80; // Adjust the value as needed
    		// 	}

        	// } 
		// std::cout << cPos.x << " " << cPos.y << " " << cPos.z << std::endl;
		// std::cout << iPos.x << " " << iPos.y << " " << iPos.z << std::endl;


		DrawBoundingBox(characterBox, BLUE);  
		DrawBoundingBox(islandsBox, RED);  
			
		// if (CheckCollisionBoxes(characterBox, islandsBox)) {
		
		// 	collisionCount++;
		// 	std::cout << " collision between character and island " << std::endl;
			
		// 	collisionCount++;
		// 	characterTransform.jumping = false;
		// 	kinematics.velocity.y = -kinematics.velocity.y;
			
		// }

       
        
    // }
	

    // for (Entity characterEntity = 0; characterEntity < scene.entityMasks.size(); characterEntity++) {
    //     if (!scene.HasComponent<transformcomp>(characterEntity)) continue;
    //     if (!scene.HasComponent<Rendering>(characterEntity)) continue;

    //     auto& characterTransform = scene.GetComponent<transformcomp>(characterEntity);
    //     auto& characterRender = scene.GetComponent<Rendering>(characterEntity);
    //     auto characterBox = characterRender.model->GetTransformedBoundingBox();

		

        for (Entity islandEntity = 0; islandEntity < scene.entityMasks.size(); islandEntity++) {
            if (islandEntity == character) continue;
			if (islandEntity == character2) continue;
			// if (islandEntity == characterEntity) continue;
            if (!scene.HasComponent<transformcomp>(islandEntity)) continue;
            if (!scene.HasComponent<Rendering>(islandEntity)) continue;
			// if (!scene.HasComponent<flipFlag>(islandEntity)) continue;

			
            auto& islandTransform = scene.GetComponent<transformcomp>(islandEntity);
            auto& islandRender = scene.GetComponent<Rendering>(islandEntity);
            auto islandBox = islandRender.model->GetTransformedBoundingBox();

			raylib::Vector3 islandPos = islandTransform.position;

			// if ((((cPos.y - islandPos.y) < 90 && (cPos.y - islandPos.y) > 70)  && abs(cPos.x - islandPos.x) < 50) && abs(cPos.z - islandPos.z) < 50) {
				
			// 	collisionCount++;
				
			// 	std::cout << " collision between character and island " << std::endl;
				
			// 	kinematics.bound = true;
				
			// 		characterTransform.position.y = islandPos.y + 75; // Adjust the value as needed
    			

        	// } 
			if ((((c2Pos.y - islandPos.y) < 90 && (c2Pos.y - islandPos.y) > 70) && abs(c2Pos.x - islandPos.x) < 50) && abs(c2Pos.z - islandPos.z) < 50) {
				
				collisionCount++;
				
				std::cout << " collision between character and island " << std::endl;
				
				kinematics.bound = true;
			
					// character2Transform.position.y = islandPos.y + 75; // Adjust the value as needed
					// characterTransform.position.y = islandPos.y + 75; // Adjust the value as needed
    			
					kinematics.velocity.y = 0;
					characterTransform.jumpCounter = 0;
					character2Transform.jumpCounter = 0;
        	} 

		// }
        
    }
    std::cout << "total collisions: " << collisionCount << std::endl;
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

void spinSystem(Scene<ComponentStorage>& scene, float dt) {
	for (Entity e = 0; e < scene.entityMasks.size(); e++) {
        if (!scene.HasComponent<transformcomp>(e)) continue;
        if (!scene.HasComponent<physics>(e)) continue;
		if (!scene.HasComponent<veloKinematics>(e)) continue;
		if (!scene.HasComponent<Rendering>(e)) continue;

        auto& transformComponent = scene.GetComponent<transformcomp>(e);
        auto& physicsComponent = scene.GetComponent<physics>(e);
		auto& kinematics = scene.GetComponent<veloKinematics>(e); 
		auto& render = scene.GetComponent<Rendering>(e);

		float& angularAcceleration = physicsComponent.angularAcceleration;
		int& counter = render.counter;
		
		if ((transformComponent.jumping && counter < 1) && !scene.HasComponent<flipFlag>(e)) {
			render.model->transform = raylib::Transform(render.model->transform).RotateY(raylib::Degree(6.1));
			render.color = YELLOW;
			
		}
		if (transformComponent.jumping && scene.HasComponent<flipFlag>(e)) render.model->transform = raylib::Transform(render.model->transform).RotateX(raylib::Degree(6.1));

    }
}

void jumpSystem(Scene<ComponentStorage>& scene, float dt) {
	int counter = 0;
	for (Entity e = 0; e < scene.entityMasks.size(); e++) {
		if (!scene.HasComponent<veloKinematics>(e)) continue;
		if (!scene.HasComponent<transformcomp>(e)) continue;
		if (!scene.HasComponent<Rendering>(e)) continue;

		auto& render = scene.GetComponent<Rendering>(e);
		auto& kinematics = scene.GetComponent<veloKinematics>(e);
		auto& transformComponent = scene.GetComponent<transformcomp>(e);


		if ((transformComponent.position.y > -1 && transformComponent.position.y < 25) && transformComponent.jumping) {
			kinematics.velocity.y += 15;
			// transformComponent.jumpCounter++;
		}
		if((transformComponent.position.y > (24 * transformComponent.jumpCounter)) && (transformComponent.position.y < (26 * transformComponent.jumpCounter))) {
			transformComponent.jumping = false;
		}
        // if(transformComponent.jumping && (transformComponent.position.y < 25 * transformComponent.jumpCounter)) {
		// 	if (transformComponent.position.y >= 25) transformComponent.jumpCounter++;
		// 	kinematics.velocity.y += 10;
		// }
		if(!transformComponent.jumping) {
			
			kinematics.velocity.y -= 15;
		}
		if (transformComponent.position.y < .05 && !transformComponent.jumping) {
			transformComponent.position.y = 0;
			// render.counter = 0;
			render.color = WHITE;
			transformComponent.jumpCounter = 0;
		}


		transformComponent.position.y += kinematics.velocity.y * dt;
		
		
	}
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
        
        if (rendering.drawBoundingBox) {
            DrawBoundedModel(*rendering.model, color, Transformer);
        } else {
            DrawModel(*rendering.model, color, Transformer);
        }
    }
}

void DrawWorldSystem(Scene<ComponentStorage>& scene) {
    for(Entity e = 0; e < scene.entityMasks.size(); e++) {
        if(!scene.HasComponent<Rendering>(e)) continue;
        if(!scene.HasComponent<transformcomp>(e)) continue;
		
        auto & rendering = scene.GetComponent<Rendering>(e);
        auto & transformComponent = scene.GetComponent<transformcomp>(e);
	
		raylib::Color color = rendering.color;

        auto Transformer = [&transformComponent](raylib::Transform t) -> raylib::Transform {
			auto [axis, angle] = transformComponent.rotation.ToAxisAngle();
            return t.
            Translate(transformComponent.position).
            Scale(transformComponent.scale.x, transformComponent.scale.y, transformComponent.scale.z).Rotate(axis, angle);
        };
        
        if (rendering.drawBoundingBox) {
            DrawBoundedModel(*rendering.model, color, Transformer);
        } else {
            DrawModel(*rendering.model, color, Transformer);
        }
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

		// bool& is2D = physicsComponent._2d;
		
		(*buffer.inputs)["forward"].AddPressedCallback([&kinematics]()-> void {   
			if(kinematics.maxSpeed > kinematics.targetSpeed) kinematics.targetSpeed += 5;
        });
        (*buffer.inputs)["backwards"].AddPressedCallback([&kinematics]()-> void {    
			if(kinematics.minSpeed < kinematics.targetSpeed)kinematics.targetSpeed -= 5; 
        });
        (*buffer.inputs)["right"].AddPressedCallback([&physicsComponent]()-> void { 
			physicsComponent.targetRotation = physicsComponent.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Down(), raylib::Degree(35));  
        });
        (*buffer.inputs)["left"].AddPressedCallback([&physicsComponent]()-> void { 
			physicsComponent.targetRotation = physicsComponent.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Up(), raylib::Degree(35)); 
        });
		
        (*buffer.inputs)["space"].AddPressedCallback([&physicsComponent, &transformComponent, &kinematics]()-> void { 
            if (transformComponent.jumpCounter < 3) {
				kinematics.velocity.y += 25;
				transformComponent.jumpCounter++;
			} 
        });
	}
}

void VelocitySystem(Scene<ComponentStorage>& scene, float dt) {
	for (Entity e = 0; e < scene.entityMasks.size(); e++) {
		if (!scene.HasComponent<veloKinematics>(e)) continue;
		if (!scene.HasComponent<transformcomp>(e)) continue;
		
		auto& kinematics = scene.GetComponent<veloKinematics>(e);
		auto& transformComponent = scene.GetComponent<transformcomp>(e);

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


		kinematics.velocity.y -= 20 * dt;

		if ((transformComponent.position.y < -.005) ) {
			kinematics.velocity.y = 0;
		}

        transformComponent.position.x += kinematics.velocity.x * dt;
		transformComponent.position.y += kinematics.velocity.y * dt;
		transformComponent.position.z += kinematics.velocity.z * dt;
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
		

		float& speed = kinematics.speed;

       	rotation = QuaternionSlerp(rotation, targetRotation, angularAcceleration * dt); 
		
		// Calculate velocity based on updated speed and rotation
		auto upward = velocity.y;
		velocity = raylib::Vector3::Left().RotateByQuaternion(rotation) * speed;
		velocity.y = upward;

		transformComponent.rotation = rotation;
    }
}

void chaseVelocitySystem(Scene<ComponentStorage>& scene, Entity character, float dt) {
	auto& CtransComponent = scene.GetComponent<transformcomp>(character);

    for (Entity e = 0; e < scene.entityMasks.size(); e++) {
        if (!scene.HasComponent<veloKinematics>(e)) continue;
        if (!scene.HasComponent<transformcomp>(e)) continue;

        auto& kinematics = scene.GetComponent<veloKinematics>(e);
        auto& transformComponent = scene.GetComponent<transformcomp>(e);

        raylib::Vector3& velocity = kinematics.velocity;
        float& speed = kinematics.speed;
        float acceleration = kinematics.acceleration;

        // Calculate direction towards the target position
        raylib::Vector3 targetvelo = CtransComponent.position;
        

		targetvelo.x = transformComponent.position.x - targetvelo.x;

        

        // Update speed based on acceleration
        float targetSpeed = kinematics.targetSpeed;
        float maxSpeed = kinematics.maxSpeed;
        float minSpeed = kinematics.minSpeed;

        float target = Clamp(targetSpeed, minSpeed, maxSpeed);
        if (speed < target)
            speed += acceleration * dt;
        else if (speed > target)
            speed -= acceleration * dt;
        speed = Clamp(speed, minSpeed, maxSpeed);

        // Update position
        transformComponent.position.x += targetvelo.x * dt;
    }
}


// void flipFlagSystem(Scene<ComponentStorage>& scene, float dt) {

// 	for (Entity e = 0; e < scene.entityMasks.size(); e++) {

//         if (!scene.HasComponent<transformcomp>(e)) continue;
//         if (!scene.HasComponent<flipFlag>(e)) continue;
// 		if (!scene.HasComponent<veloKinematics>(e)) continue;

//         auto& transformComponent = scene.GetComponent<transformcomp>(e);
//         auto& flipFlagComponent = scene.GetComponent<flipFlag>(e);
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
// 		float angularAcceleration = flipFlagComponent.angularAcceleration;
// 		raylib::Degree& heading = flipFlagComponent.heading;
// 		raylib::Degree& targetHeading = flipFlagComponent.targetHeading;
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
// 		if(!scene.HasComponent<flipFlag>(e)) {
// 			std::cout << "I DO NOT HAVE 2 D PHYSICS" << std::endl;
// 			continue;
// 		}
// 		auto& flipFlagComponent = scene.GetComponent<flipFlag>(e); 


// 		(*buffer.inputs)["forward"].AddPressedCallback([&kinematics, &buffer]()-> void {   
// 			if (buffer.selected)
//             kinematics.targetSpeed += 3;// thus doing   
//         });
//         (*buffer.inputs)["backwards"].AddPressedCallback([&kinematics, &buffer]()-> void {           
//             if (buffer.selected)
// 			kinematics.targetSpeed -= 1; // thus doing 
//         });
//         (*buffer.inputs)["right"].AddPressedCallback([&flipFlagComponent, &buffer]()-> void { 
//             if (buffer.selected) {
//                 flipFlagComponent.targetHeading -= 20 * DEG2RAD;
//                 std::cout << "Right key pressed. New Target Heading: " << flipFlagComponent.targetHeading << std::endl;
//             }
//         });
//         (*buffer.inputs)["left"].AddPressedCallback([&flipFlagComponent, &buffer]()-> void { 
//             if (buffer.selected) {
//                 flipFlagComponent.targetHeading += 20 * DEG2RAD;
//                 std::cout << "Left key pressed. New Target Heading: " << flipFlagComponent.targetHeading << std::endl;
//             }
//         });
//         (*buffer.inputs)["space"].AddPressedCallback([&kinematics, &buffer]()-> void { 
//             if (buffer.selected)
// 			kinematics.targetSpeed = 0;
//         });
// 	}
// }