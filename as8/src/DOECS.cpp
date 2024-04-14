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
#include <BufferedInput.hpp>
// #include "Counter.cpp"

template<typename T>
concept Transformer = requires( T t, raylib::Transform m) {
    {t.operator()(m)}->std::convertible_to<raylib::Transform>;
};

// using Entity = uint32_t;

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
			// std::cout << globalComponentCounter << "global counter for components" << std::endl;
			// assert(e < (data.size() / elementSize));

			std::cout << "assertion error value : e is " << static_cast<unsigned int>(e) << " data size divided by element size " << data.size() / elementSize << std::endl;
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
};

struct TwoDphysics {
    
    float angularAcceleration;

    float heading; // Using quaternion for rotation
    float targetHeading; // Target rotation
};

struct Rendering {
    raylib::Model* model;
    bool drawBoundingBox = false;

	raylib::Color color;
};

struct transformcomp {
    raylib::Vector3 position;
    raylib::Vector3 scale;
    raylib::Quaternion rotation;
};

struct bufferedComponent {
	raylib::BufferedInput* inputs; // Manager for actions 
    bool selected = false;
	
};

void DrawBoundedModel(raylib::Model& model, const raylib::Color& color, Transformer auto transformer);
void DrawModel(raylib::Model& model, const raylib::Color& color, Transformer auto transformer);
void VelocitySystem(Scene<ComponentStorage>& scene, float dt);
void TwoDPhysicsSystem(Scene<ComponentStorage>& scene, float dt);
void ThreeDPhysicsSystem(Scene<ComponentStorage>& scene, float dt);
void DrawSystem(Scene<ComponentStorage>& scene);
void ProcessInputSystem(Scene<ComponentStorage>& scene);
void BoatProcessInputSystem(Scene<ComponentStorage>& scene);



void DrawSystem(Scene<ComponentStorage>& scene) {

    // raylib::Color color;

    for(Entity e = 0; e < scene.entityMasks.size(); e++) {
        if(!scene.HasComponent<Rendering>(e)) continue;
        if(!scene.HasComponent<transformcomp>(e)) continue;
		if(!scene.HasComponent<bufferedComponent>(e)) continue;
        auto & rendering = scene.GetComponent<Rendering>(e);
        auto & transformComponent = scene.GetComponent<transformcomp>(e);
		auto & buffer = scene.GetComponent<bufferedComponent>(e);
                // std::cout << "axis.x" << transformComponent.rotation.ToEuler().x << std::endl;
                // std::cout << "axis.y" << transformComponent.rotation.ToEuler().y << std::endl;
                // std::cout << "axis.z" << transformComponent.rotation.ToEuler().z << std::endl;

		raylib::Color color = rendering.color;

        auto Transformer = [&transformComponent](raylib::Transform t) -> raylib::Transform {
			auto [axis, angle] = transformComponent.rotation.ToAxisAngle();
            return t.
            Translate(transformComponent.position).
            Scale(transformComponent.scale.x, transformComponent.scale.y, transformComponent.scale.z).Rotate(axis, angle);
        };
        
        if (buffer.selected) {
            DrawBoundedModel(*rendering.model, color, Transformer);
        } else {
            DrawModel(*rendering.model, color, Transformer);
        }
    }
}

int main() {
	// Create window
	const int screenWidth = 900 * 2; // 900
	const int screenHeight = 450 * 2; // 450
	raylib::Window window(screenWidth, screenHeight, "CS381 - Assignment 8");

	raylib::BufferedInput inputs; // Manager for actions 

    float dt = 0;

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

	int numberOfPlanes = 5;
	int counter = -1;

    raylib::Model plane("meshes/PolyPlane.glb");
	raylib::Model boat1("meshes/SmitHouston_Tug.glb");
    Scene scene;
	
 
    // Add a transform component to the entity
   for (int i = 0; i < numberOfPlanes; ++i) {
		auto e = scene.CreateEntity();

		scene.AddComponent<Rendering>(e) = {                                          //1
		&plane, 
		false, DARKGRAY}; // Plane with no bounding box, ie false 

		scene.AddComponent<transformcomp>(e) = {                                          //2
		(Vector3){i * 100.0f - 200, 90, 0}, 
		(Vector3){1,1,1}, 
		QuaternionIdentity()}; // Adjust position based on 'i'

		scene.AddComponent<physics>(e) = {                                          //3
		5, 
		QuaternionIdentity(), QuaternionIdentity()};

		scene.AddComponent<veloKinematics>(e) = {                                          //4
		5, 
		(Vector3){0, 0, 0}, 
		5, 
		5, 
		50, 
		0};
		
		scene.AddComponent<bufferedComponent>(e) = {                                          //5
		&inputs, 
		false};
	}
	

	////////////////////////////////////////////////////////

	auto b1 = scene.CreateEntity();
	scene.AddComponent<Rendering>(b1) = {
	&boat1, 
	false, GRAY}; // Plane with no bounding box, ie false 
	scene.AddComponent<transformcomp>(b1) = {
	(Vector3){-100, 0, -50}, 
	(Vector3){1,.5,1}, 
	QuaternionIdentity()}; // Adjust position based on 'i'
	scene.AddComponent<TwoDphysics>(b1) = {
	.08, 
	0.0f, 0.0f};
	scene.AddComponent<veloKinematics>(b1) = {
	5, 
	(Vector3){0, 0, 0}, 
	5, 
	5, 
	50, 
	0};
	scene.AddComponent<bufferedComponent>(b1) = {
	&inputs, 
	false};

	auto b2 = scene.CreateEntity();
	scene.AddComponent<Rendering>(b2) = {
	&boat1, 
	false, ORANGE}; // Plane with no bounding box, ie false 
	scene.AddComponent<transformcomp>(b2) = {
	(Vector3){-200, 0, 0}, 
	(Vector3){1,1,5}, 
	QuaternionIdentity()}; // Adjust position based on 'i'
	scene.AddComponent<TwoDphysics>(b2) = {
	.08, 
	0.0f, 0.0f};
	scene.AddComponent<veloKinematics>(b2) = {
	5, 
	(Vector3){0, 0, 0}, 
	5, 
	5, 
	50, 
	0};
	scene.AddComponent<bufferedComponent>(b2) = {
	&inputs, 
	false};

	auto b3 = scene.CreateEntity();
	scene.AddComponent<Rendering>(b3) = {
	&boat1, 
	false, PINK}; // Plane with no bounding box, ie false 
	scene.AddComponent<transformcomp>(b3) = {
	(Vector3){-300, 0, 200}, 
	(Vector3){2,4,2}, 
	QuaternionIdentity()}; // Adjust position based on 'i'
	scene.AddComponent<TwoDphysics>(b3) = {
	.08, 
	0.0f, 0.0f};
	scene.AddComponent<veloKinematics>(b3) = {
	5, 
	(Vector3){0, 0, 0}, 
	5, 
	5, 
	50, 
	0};
	scene.AddComponent<bufferedComponent>(b3) = {
	&inputs, 
	false};

	auto b4 = scene.CreateEntity();
	scene.AddComponent<Rendering>(b4) = {
	&boat1, 
	false, BLUE}; // Plane with no bounding box, ie false 
	scene.AddComponent<transformcomp>(b4) = {
	(Vector3){100, 0, 100}, 
	(Vector3){2,1,7}, 
	QuaternionIdentity()}; // Adjust position based on 'i'
	scene.AddComponent<TwoDphysics>(b4) = {
	.08, 
	0.0f, 0.0f};
	scene.AddComponent<veloKinematics>(b4) = {
	5, 
	(Vector3){0, 0, 0}, 
	5, 
	5, 
	50, 
	0};
	scene.AddComponent<bufferedComponent>(b4) = {
	&inputs, 
	false};

	auto b5 = scene.CreateEntity();
	scene.AddComponent<Rendering>(b5) = {
	&boat1, 
	false, GREEN}; // Plane with no bounding box, ie false 
	scene.AddComponent<transformcomp>(b5) = {
	(Vector3){200, 0, 0}, 
	(Vector3){.5,.25,.5}, 
	QuaternionIdentity()}; // Adjust position based on 'i'
	scene.AddComponent<TwoDphysics>(b5) = {
	.08, 
	0.0f, 0.0f};
	scene.AddComponent<veloKinematics>(b5) = {
	5, 
	(Vector3){0, 0, 0}, 
	5, 
	5, 
	50, 
	0};
	scene.AddComponent<bufferedComponent>(b5) = {
	&inputs, 
	false};


	

	///////////////////////////////////////


	// Rendering rem;
	// // rem.//
	// scene.AddComponent<Rendering>(...)=rem;
	// rem.drawBoundingBox = true;
    
	// Buffered input actions for setup in buffered input component
	inputs["forward"] = raylib::Action::key(KEY_W).move();
	inputs["backwards"] = raylib::Action::key(KEY_S).move();
	inputs["right"] = raylib::Action::key(KEY_D).move();
	inputs["left"] = raylib::Action::key(KEY_A).move();
	inputs["tiltleft"] = raylib::Action::key(KEY_F).move();
	inputs["tiltright"] = raylib::Action::key(KEY_R).move();
	inputs["tiltback"] = raylib::Action::key(KEY_E).move();
	inputs["tiltforward"] = raylib::Action::key(KEY_Q).move();
	inputs["space"] = raylib::Action::key(KEY_SPACE).move();
	inputs["change"] = raylib::Action::key(KEY_TAB).move();

	bool inputTabbed = false;

	inputs["change"] = raylib::Action::key(KEY_TAB).SetPressedCallback([&scene, &counter, &inputTabbed](){
		counter = (counter + 1) % scene.entityMasks.size();
		inputTabbed = true;
		for (Entity e = 0; e < scene.entityMasks.size(); e++) {
			if(!scene.HasComponent<bufferedComponent>(e)) continue; // get optional reference to transform component
			auto& bufInput = scene.GetComponent<bufferedComponent>(e); // get values stored in reference if it exists
			bufInput.selected = counter == e;
		}
	}).move();

	SetTargetFPS(60);

    ProcessInputSystem(scene);
	BoatProcessInputSystem(scene);

	bool changeCamera = false;
    

	// Main loop
	bool keepRunning = true;
	while(!window.ShouldClose() && keepRunning) {

		inputs.PollEvents(); // Poll buffered input from user

        dt = window.GetFrameTime();
		
		// Rendering
		window.BeginDrawing();
		{
			// Clear screen
			window.ClearBackground(BLACK);

			if (IsKeyReleased(KEY_C)) { // press C to chnage camera veiw
                if (changeCamera == false) changeCamera = true;
                else changeCamera = false;
            }

			camera.BeginMode();
			{	

				if (counter == 10) camera.fovy = 65.0; // Change feild of veiw for largest ship
				if (changeCamera && inputTabbed) { // Target is now position of current selected model

					for (Entity e = 0; e < scene.entityMasks.size(); e++) {
						if(!scene.HasComponent<bufferedComponent>(e)) continue; // get optional reference to transform component
						auto& bufInput = scene.GetComponent<bufferedComponent>(e); // get values stored in reference if it exists
						bufInput.selected = counter == e;
					
						camera.target = (Vector3){
							scene.GetComponent<transformcomp>(counter).position.x, 
							scene.GetComponent<transformcomp>(counter).position.y, 
							scene.GetComponent<transformcomp>(counter).position.z
						};
						camera.position = (Vector3){ // Camera position
							scene.GetComponent<transformcomp>(counter).position.x - 200, 
							scene.GetComponent<transformcomp>(counter).position.y + 105, 
							scene.GetComponent<transformcomp>(counter).position.z 
						};  
						camera.up = raylib::Vector3::Up(); // Up direction         
						if (counter == 9) camera.fovy = 60.0;
						else camera.fovy = 35.0f; // Camera field-of-view Y
						camera.projection = CAMERA_PERSPECTIVE;

				}
				} else { // revert to old camera settings 
					camera.target = raylib::Vector3(0, 0, 300), // Target
					camera.position = raylib::Vector3(0, 120, -500), // Position
					camera.up = raylib::Vector3::Up(), 
					camera.fovy = 50.0f; // Camera field-of-view Y
					camera.projection = CAMERA_PERSPECTIVE;
				}
				
				// Render skybox and ground
				// skybox.Draw();

				ground.Draw({});

			}
			VelocitySystem(scene, dt);

			ThreeDPhysicsSystem(scene, dt);
			
            TwoDPhysicsSystem(scene, dt);
			
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


void BoatProcessInputSystem(Scene<ComponentStorage>& scene) {

	for(Entity e = 0; e < scene.entityMasks.size(); e++) {

		if(!scene.HasComponent<bufferedComponent>(e)) continue;
		if(!scene.HasComponent<veloKinematics>(e)) continue;
		auto& buffer = scene.GetComponent<bufferedComponent>(e);
		auto& kinematics = scene.GetComponent<veloKinematics>(e);
		if(!scene.HasComponent<TwoDphysics>(e)) {
			std::cout << "I DO NOT HAVE 2 D PHYSICS" << std::endl;
			continue;
		}
		auto& TwoDPhysicsComponent = scene.GetComponent<TwoDphysics>(e); 


		(*buffer.inputs)["forward"].AddPressedCallback([&kinematics, &buffer]()-> void { //lambda function that is creating call back for action   
			if (buffer.selected)
            kinematics.targetSpeed += 3;// thus doing   
        });
        (*buffer.inputs)["backwards"].AddPressedCallback([&kinematics, &buffer]()-> void { //lambda function that is creating call back for action           
            if (buffer.selected)
			kinematics.targetSpeed -= 1; // thus doing 
        });
        (*buffer.inputs)["right"].AddPressedCallback([&TwoDPhysicsComponent, &buffer]()-> void { 
            if (buffer.selected) {
                TwoDPhysicsComponent.targetHeading -= 10 * DEG2RAD;
                std::cout << "Right key pressed. New Target Heading: " << TwoDPhysicsComponent.targetHeading << std::endl;
            }
        });
        (*buffer.inputs)["left"].AddPressedCallback([&TwoDPhysicsComponent, &buffer]()-> void { 
            if (buffer.selected) {
                TwoDPhysicsComponent.targetHeading += 10 * DEG2RAD;
                std::cout << "Left key pressed. New Target Heading: " << TwoDPhysicsComponent.targetHeading << std::endl;
            }
        });
        (*buffer.inputs)["space"].AddPressedCallback([&kinematics, &buffer]()-> void { //lambda function that is creating call back for action 
            if (buffer.selected)
			kinematics.targetSpeed = 0;
        });
	}
}

//call a function when a event happened, 
void ProcessInputSystem(Scene<ComponentStorage>& scene) {

	for(Entity e = 0; e < scene.entityMasks.size(); e++) {

		if(!scene.HasComponent<bufferedComponent>(e)) continue;
		if(!scene.HasComponent<physics>(e)) { 
			std::cout << "I DO NOT HAVE 3 D PHYSICS" << std::endl; 
		continue;}
		if(!scene.HasComponent<veloKinematics>(e)) continue;
		auto& buffer = scene.GetComponent<bufferedComponent>(e);
		auto& physicsComponent = scene.GetComponent<physics>(e); // Declare physicsComponent here
		auto& kinematics = scene.GetComponent<veloKinematics>(e);

		(*buffer.inputs)["forward"].AddPressedCallback([&kinematics, &buffer]()-> void { //lambda function that is creating call back for action   
			if (buffer.selected)
            kinematics.targetSpeed += 3;// thus doing   
        });
        (*buffer.inputs)["backwards"].AddPressedCallback([&kinematics, &buffer]()-> void { //lambda function that is creating call back for action           
            if (buffer.selected)
			kinematics.targetSpeed -= 1; // thus doing 

        });
        (*buffer.inputs)["right"].AddPressedCallback([&physicsComponent, &buffer]()-> void { //lambda function that is creating call back for action 
            if (buffer.selected)
			physicsComponent.targetRotation = physicsComponent.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Down(), raylib::Degree(15));  
        });
        (*buffer.inputs)["left"].AddPressedCallback([&physicsComponent, &buffer]()-> void { //lambda function that is creating call back for action 
            if (buffer.selected)
			physicsComponent.targetRotation = physicsComponent.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Up(), raylib::Degree(15)); 
        });
		(*buffer.inputs)["tiltright"].AddPressedCallback([&physicsComponent, &buffer]()-> void { //lambda function that is creating call back for action 
            if (buffer.selected)
			physicsComponent.targetRotation = physicsComponent.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Right(), raylib::Degree(10));  
        });
		(*buffer.inputs)["tiltleft"].AddPressedCallback([&physicsComponent, &buffer]()-> void { 
			if (buffer.selected)
			physicsComponent.targetRotation = physicsComponent.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Left(), raylib::Degree(10));
		});
		(*buffer.inputs)["tiltforward"].AddPressedCallback([&physicsComponent, &buffer]()-> void { 
			if (buffer.selected)
			physicsComponent.targetRotation = physicsComponent.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Forward(), raylib::Degree(12));
		});
		(*buffer.inputs)["tiltback"].AddPressedCallback([&physicsComponent, &buffer]()-> void { 
			if (buffer.selected)
			physicsComponent.targetRotation = physicsComponent.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Back(), raylib::Degree(12));
		});
        (*buffer.inputs)["space"].AddPressedCallback([&kinematics, &buffer]()-> void { //lambda function that is creating call back for action 
            if (buffer.selected)
			kinematics.targetSpeed = 0;
        });
		
		
	}
}

float angClamp(float& targetRot, float& Rotation, float angAcc, float dt) {

	static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
		int intPart = angle;
		float floatPart = float(angle) - intPart;
		intPart %= 360;
		intPart += (intPart < 0) * 360;
		return intPart + floatPart;
	};

	// float target = AngleClamp(targetRot);
	int difference = abs(targetRot - Rotation);
	if(targetRot > Rotation) {
		if(difference < 180) Rotation += angAcc * dt;
		else if(difference > 180) Rotation -= angAcc * dt;
	} else if(targetRot < Rotation) {
		if(difference < 180) Rotation -= angAcc * dt;
		else if(difference > 180) Rotation += angAcc * dt;
	} 
	if(difference < .005) Rotation = targetRot; // If the Rotation is really close to correct 
	// Rotation = AngleClamp(Rotation);
	// float angle = raylib::Degree(Rotation); // convert heading

	return Rotation;
}

// rotate a vector and we can slerp and two axis angle 

// slerping is interpolating two quats.

//{sin()x, sin()y, sin()z, cos()}

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

		// Update position based on velocity
		transformComponent.position += velocity * dt;
	
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

        // Update speed based on physics parameters
		raylib::Vector3& velocity = kinematics.velocity;
		float angularAcceleration = physicsComponent.angularAcceleration;
		raylib::Quaternion& rotation = physicsComponent.rotation;
        raylib::Quaternion& targetRotation = physicsComponent.targetRotation;

		float& speed = kinematics.speed;

       	rotation = QuaternionSlerp(rotation, targetRotation, angularAcceleration * dt); 
		
		// Calculate velocity based on updated speed and rotation
		/*-----kinematics.velocity --------*/velocity = raylib::Vector3::Left().RotateByQuaternion(rotation) * speed;

		transformComponent.rotation = rotation;
		
        // Output for debugging
        // std::cout << dt << " <---  dt from the physics." << std::endl;
        // std::cout << rotation.x << " <---  x rotation from the physics." << std::endl;
        // std::cout << rotation.y << " <---  y rotation from the physics." << std::endl;
        // std::cout << rotation.z << " <---  z rotation from the physics." << std::endl;
        // std::cout << speed << " <---  speed from the physics." << std::endl;
    }
}

void TwoDPhysicsSystem(Scene<ComponentStorage>& scene, float dt) {

	for (Entity e = 0; e < scene.entityMasks.size(); e++) {

        if (!scene.HasComponent<transformcomp>(e)) continue;
        if (!scene.HasComponent<TwoDphysics>(e)) continue;
		if (!scene.HasComponent<veloKinematics>(e)) continue;

        auto& transformComponent = scene.GetComponent<transformcomp>(e);
        auto& TwoDphysicsComponent = scene.GetComponent<TwoDphysics>(e);
		auto& kinematics = scene.GetComponent<veloKinematics>(e);

		// static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
		// 	int intPart = angle;
		// 	float floatPart = float(angle) - intPart;
		// 	intPart %= 360;
		// 	intPart += (intPart < 0) * 360;
		// 	return intPart + floatPart;
		// }; 

		raylib::Vector3& velocity = kinematics.velocity;
		float& speed = kinematics.speed;
		float angularAcceleration = TwoDphysicsComponent.angularAcceleration;
		float& heading = TwoDphysicsComponent.heading;
		float& targetHeading = TwoDphysicsComponent.targetHeading;
        float difference = abs(targetHeading - heading);

        if (targetHeading > heading) {
            if (difference < 180 * DEG2RAD)
                heading += angularAcceleration * dt;
            else if (difference > 180 * DEG2RAD)
                heading -= angularAcceleration * dt;
        } else if (targetHeading < heading) {
            if (difference < 180 * DEG2RAD)
                heading -= angularAcceleration * dt;
            else if (difference > 180 * DEG2RAD)
                heading += angularAcceleration * dt;
        }

		std::cout << "<-----------Difference: " << difference << std::endl;
		std::cout << "<-----Updated Heading: " << heading << std::endl;
        if (difference < .005) heading = targetHeading;

		std::cout << heading << " <---  heading ." << std::endl;
		std::cout << targetHeading << " <---  targetHeading ." << std::endl;
		std::cout << angularAcceleration << " <---  ang acc ." << std::endl;

		raylib::Quaternion rotation = raylib::Quaternion::FromAxisAngle({0, 1, 0}, heading);
		// From Chatgpt - "how do i add  ninety degrees to the heading for the following two lines of code?"
		raylib::Quaternion offsetY = raylib::Quaternion::FromAxisAngle({0, 1, 0}, PI / 2.0f);

		raylib::Quaternion newRotation = offsetY * rotation;
	
		velocity = raylib::Vector3::Left().RotateByQuaternion(rotation) * speed;

		transformComponent.rotation = newRotation;

		// std::cout << transformComponent.rotation.x << " <---  x rotation from the physics." << std::endl;
        // std::cout << transformComponent.rotation.y << " <---  y rotation from the physics." << std::endl;
        // std::cout << transformComponent.rotation.z << " <---  z rotation from the physics." << std::endl;
		// std::cout << transformComponent.rotation.w << " <---  w rotation from the physics." << std::endl;
	}
}