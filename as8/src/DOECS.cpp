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

// using Entity = uint8_t;

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

	using Entity = uint32_t; 

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
	bool twoD = false;
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

void DrawBoundedModel(raylib::Model& model, Transformer auto transformer);
void DrawModel(raylib::Model& model, Transformer auto transformer);
void ThreeDVelocitySystem(Scene<ComponentStorage>& scene, float dt);
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

        auto Transformer = [&transformComponent](raylib::Transform t) -> raylib::Transform {
			auto [axis, angle] = transformComponent.rotation.ToAxisAngle();
            return t.
            Translate(transformComponent.position).
            Scale(transformComponent.scale.x, transformComponent.scale.y, transformComponent.scale.z).Rotate(axis, angle);
        };
        
        if (buffer.selected) {
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

	int numberOfPlanes = 1;
	int counter = -1;

    raylib::Model plane("meshes/PolyPlane.glb");
    Scene scene;
 
    // Add a transform component to the entity
//    for (int i = 0; i < numberOfPlanes; ++i) {
// 		auto e = scene.CreateEntity();
// 		scene.AddComponent<Rendering>(e) = {&plane, true}; // Plane with no bounding box, ie false 
// 		scene.AddComponent<transformcomp>(e).position = {(Vector3){i * 100.0f - 200, 90, 0}}; // Adjust position based on 'i'
// 		scene.AddComponent<transformcomp>(e).scale = {(Vector3){2,2,2}};
// 		scene.AddComponent<transformcomp>(e).rotation = QuaternionIdentity(); // Initialize with no rotation
// 		scene.AddComponent<physics>(e).acceleration = 5;
// 		scene.AddComponent<physics>(e).angularAcceleration = 12;
// 		scene.AddComponent<physics>(e).rotation = QuaternionIdentity(); // Initialize with no rotation
// 		scene.AddComponent<physics>(e).targetRotation = QuaternionIdentity(); // Initialize with no target rotation
// 		scene.AddComponent<physics>(e).speed = 5;
// 		scene.AddComponent<physics>(e).targetSpeed = 5;
// 		scene.AddComponent<physics>(e).minSpeed = 0;
// 		scene.AddComponent<physics>(e).maxSpeed = 50;

// 		scene.AddComponent<bufferedComponent>(e).inputs = &inputs;
// 		scene.AddComponent<bufferedComponent>(e).selected = false;
// 	}

	auto b1 = scene.CreateEntity();

	scene.AddComponent<Rendering>(b1) = {
	&plane, 
	true}; // Plane with no bounding box, ie false 
	scene.AddComponent<transformcomp>(b1) = {
	(Vector3){100.0f - 200, 90, 0}, 
	(Vector3){2,2,2}, 
	QuaternionIdentity()}; // Adjust position based on 'i'

	// scene.AddComponent<transformcomp>(b1).scale = {(Vector3){2,2,2}};
	// scene.AddComponent<transformcomp>(b1).rotation = QuaternionIdentity(); // Initialize with no rotation
	scene.AddComponent<TwoDphysics>(b1) = {
	5, 12, 
	5, 5, 
	0, 0, 
	0, 50};

	// scene.AddComponent<TwoDphysics>(b1).angularAcceleration = 12;
	// scene.AddComponent<TwoDphysics>(b1).heading = 0; // Initialize with no rotation
	// scene.AddComponent<TwoDphysics>(b1).targetHeading = 0; // Initialize with no target rotation
	// scene.AddComponent<TwoDphysics>(b1).speed = 5;
	// scene.AddComponent<TwoDphysics>(b1).targetSpeed = 5;
	// scene.AddComponent<TwoDphysics>(b1).minSpeed = 0;
	// scene.AddComponent<TwoDphysics>(b1).maxSpeed = 50;

	// Rendering rem;
	// // rem.//
	// scene.AddComponent<Rendering>(...)=rem;
	// rem.drawBoundingBox = true;

	scene.AddComponent<bufferedComponent>(b1).inputs = &inputs;
	scene.AddComponent<bufferedComponent>(b1).selected = false;
    
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

	inputs["change"] = raylib::Action::key(KEY_TAB).SetPressedCallback([&scene, &counter](){
		counter = (counter + 1) % scene.entityMasks.size();
		for (Entity e = 0; e < scene.entityMasks.size(); e++) {
			if(!scene.HasComponent<bufferedComponent>(e)) continue; // get optional reference to transform component
			auto& bufInput = scene.GetComponent<bufferedComponent>(e); // get values stored in reference if it exists
			bufInput.selected = counter == e;
		}
	}).move();

	SetTargetFPS(60);

    ProcessInputSystem(scene);
	BoatProcessInputSystem(scene);
    

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

			camera.BeginMode();
			{	
				
				// Render skybox and ground
				skybox.Draw();
				ground.Draw({});

			}
			ThreeDVelocitySystem(scene, dt);
            TwoDPhysicsSystem(scene, dt);
			ThreeDPhysicsSystem(scene, dt);
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


// void BoatProcessInputSystem(Scene<ComponentStorage>& scene) {

// 	bool inputting = false;

// 	for(Entity e = 0; e < scene.entityMasks.size(); e++) {

// 		if(!scene.HasComponent<bufferedComponent>(e)) continue;
// 		if(!scene.HasComponent<TwoDphysics>(e)) continue;
		
// 		auto& buffer = scene.GetComponent<bufferedComponent>(e);
// 		auto& physicsComponent = scene.GetComponent<TwoDphysics>(e); // Declare physicsComponent here

// 		(*buffer.inputs)["forward"].AddPressedCallback([&physicsComponent, &buffer]()-> void { //lambda function that is creating call back for action   
// 			if (buffer.selected)
//             physicsComponent.targetSpeed += 3;// thus doing   
//         });
//         (*buffer.inputs)["backwards"].AddPressedCallback([&physicsComponent, &buffer]()-> void { //lambda function that is creating call back for action           
//             if (buffer.selected)
// 			physicsComponent.targetSpeed -= 1; // thus doing 
//         });
//         (*buffer.inputs)["right"].AddPressedCallback([&physicsComponent, &buffer]()-> void { //lambda function that is creating call back for action 
//             if (buffer.selected)
// 			physicsComponent.targetHeading += 10;
//         });
//         (*buffer.inputs)["left"].AddPressedCallback([&physicsComponent, &buffer]()-> void { //lambda function that is creating call back for action 
//             if (buffer.selected)
// 			physicsComponent.targetHeading -= 10;
//         });
//         (*buffer.inputs)["space"].AddPressedCallback([&physicsComponent, &buffer]()-> void { //lambda function that is creating call back for action 
//             if (buffer.selected)
// 			physicsComponent.targetSpeed = 0;
//         });
// 	}
// }

//call a function when a event happened, 
void ProcessInputSystem(Scene<ComponentStorage>& scene) {

	bool inputting = false;

	for(Entity e = 0; e < scene.entityMasks.size(); e++) {

		if(!scene.HasComponent<bufferedComponent>(e)) continue;
		if(!scene.HasComponent<physics>(e)) continue;
		if(!scene.HasComponent<veloKinematics>(e)) continue;
		auto& buffer = scene.GetComponent<bufferedComponent>(e);
		auto& physicsComponent = scene.GetComponent<physics>(e); // Declare physicsComponent here
		auto& veloComponent = scene.GetComponent<veloKinematics>(e);

		(*buffer.inputs)["forward"].AddPressedCallback([&veloComponent, &buffer]()-> void { //lambda function that is creating call back for action   
			if (buffer.selected)
            veloComponent.targetSpeed += 3;// thus doing   
        });
        (*buffer.inputs)["backwards"].AddPressedCallback([&veloComponent, &buffer]()-> void { //lambda function that is creating call back for action           
            if (buffer.selected)
			veloComponent.targetSpeed -= 1; // thus doing 

        });
        (*buffer.inputs)["right"].AddPressedCallback([&physicsComponent, &buffer]()-> void { //lambda function that is creating call back for action 
            if (buffer.selected)
			physicsComponent.targetRotation = physicsComponent.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Down(), raylib::Degree(10));  
        });
        (*buffer.inputs)["left"].AddPressedCallback([&physicsComponent, &buffer]()-> void { //lambda function that is creating call back for action 
            if (buffer.selected)
			physicsComponent.targetRotation = physicsComponent.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Up(), raylib::Degree(10)); 
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
			physicsComponent.targetRotation = physicsComponent.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Forward(), raylib::Degree(10));
		});
		(*buffer.inputs)["tiltback"].AddPressedCallback([&physicsComponent, &buffer]()-> void { 
			if (buffer.selected)
			physicsComponent.targetRotation = physicsComponent.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Back(), raylib::Degree(10));
		});
        (*buffer.inputs)["space"].AddPressedCallback([&veloComponent, &buffer]()-> void { //lambda function that is creating call back for action 
            if (buffer.selected)
			veloComponent.targetSpeed = 0;
        });

		
		if(!scene.HasComponent<TwoDphysics>(e)) continue;
		
		auto& TwoDPhysicsComponent = scene.GetComponent<TwoDphysics>(e); 

		(*buffer.inputs)["forward"].AddPressedCallback([&veloComponent, &buffer]()-> void { //lambda function that is creating call back for action   
			if (buffer.selected)
            veloComponent.targetSpeed += 3;// thus doing   
        });
        (*buffer.inputs)["backwards"].AddPressedCallback([&veloComponent, &buffer]()-> void { //lambda function that is creating call back for action           
            if (buffer.selected)
			veloComponent.targetSpeed -= 1; // thus doing 
        });
        (*buffer.inputs)["right"].AddPressedCallback([&TwoDPhysicsComponent, &buffer]()-> void { //lambda function that is creating call back for action 
            if (buffer.selected)
			TwoDPhysicsComponent.targetHeading += 10;
        });
        (*buffer.inputs)["left"].AddPressedCallback([&TwoDPhysicsComponent, &buffer]()-> void { //lambda function that is creating call back for action 
            if (buffer.selected)
			TwoDPhysicsComponent.targetHeading -= 10;
        });
        (*buffer.inputs)["space"].AddPressedCallback([&veloComponent, &buffer]()-> void { //lambda function that is creating call back for action 
            if (buffer.selected)
			veloComponent.targetSpeed = 0;
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

void ThreeDVelocitySystem(Scene<ComponentStorage>& scene, float dt) {

	for (Entity e = 0; e < scene.entityMasks.size(); e++) {
		if (!scene.HasComponent<veloKinematics>(e)) continue;
		if (!scene.HasComponent<transformcomp>(e)) continue;
		
		auto& velcomp = scene.GetComponent<veloKinematics>(e);
		auto& transformComponent = scene.GetComponent<transformcomp>(e);

		// raylib::Quaternion& rotation = physicsComponent.rotation;
        // raylib::Quaternion& targetRotation = physicsComponent.targetRotation;
		// float angularAcceleration = physicsComponent.angularAcceleration;
		raylib::Vector3 velo = velcomp.velocity;
		float targetSpeed = velcomp.targetSpeed;
        float maxSpeed = velcomp.maxSpeed;
        float minSpeed = velcomp.minSpeed;
        float& speed = velcomp.speed;
		float acceleration = velcomp.acceleration;


		 float target = Clamp(targetSpeed, minSpeed, maxSpeed);
        if (speed < target)
            speed += acceleration * dt;
        else if (speed > target)
            speed -= acceleration * dt;
        speed = Clamp(speed, minSpeed, maxSpeed);
		// float& speed = physicsComponent.speed;

		

		// Update position based on velocity
		transformComponent.position = velo * dt;
		// transformComponent.rotation
		
		
		
	}
}


void ThreeDPhysicsSystem(Scene<ComponentStorage>& scene, float dt) {
    for (Entity e = 0; e < scene.entityMasks.size(); e++) {
        if (!scene.HasComponent<transformcomp>(e)) continue;
        if (!scene.HasComponent<physics>(e)) continue;
		if (!scene.HasComponent<bufferedComponent>(e)) continue;
		if (!scene.HasComponent<veloKinematics>(e)) continue;


        auto& transformComponent = scene.GetComponent<transformcomp>(e);
        auto& physicsComponent = scene.GetComponent<physics>(e);
		auto& buffer = scene.GetComponent<bufferedComponent>(e); 
		auto& velcomp = scene.GetComponent<veloKinematics>(e); 

        // Update speed based on physics parameters
		// float angularAcceleration = physicsComponent.angularAcceleration;
        
        
		raylib::Quaternion& rotation = physicsComponent.rotation;
        raylib::Quaternion& targetRotation = physicsComponent.targetRotation;

		float& speed = velcomp.speed;

       rotation = QuaternionSlerp(rotation, targetRotation, physicsComponent.angularAcceleration * dt); 
		
		// Calculate velocity based on updated speed and rotation
		raylib::Vector3 velocity = raylib::Vector3::Left().RotateByQuaternion(rotation) * speed;

		
		
        // // Output for debugging
        // std::cout << dt << " <---  dt from the physics." << std::endl;
        // std::cout << rotation.x << " <---  x rotation from the physics." << std::endl;
        // std::cout << rotation.y << " <---  y rotation from the physics." << std::endl;
        // std::cout << rotation.z << " <---  z rotation from the physics." << std::endl;
        // std::cout << speed << " <---  speed from the physics." << std::endl;
    }
}

void TwoDPhysicsSystem(Scene<ComponentStorage>& scene, float dt) {

	for (Entity e = 0; e < scene.entityMasks.size(); e++) {

		static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
			int intPart = angle;
			float floatPart = float(angle) - intPart;
			intPart %= 360;
			intPart += (intPart < 0) * 360;
			return intPart + floatPart;
		};

        if (!scene.HasComponent<transformcomp>(e)) continue;
        if (!scene.HasComponent<TwoDphysics>(e)) continue;
		if (!scene.HasComponent<bufferedComponent>(e)) continue;

        auto& transformComponent = scene.GetComponent<transformcomp>(e);
        auto& TwoDphysicsComponent = scene.GetComponent<TwoDphysics>(e);
		auto& buffer = scene.GetComponent<bufferedComponent>(e); 

        // Update speed based on TwoDphysics parameters
		float angularAcceleration = TwoDphysicsComponent.angularAcceleration;
        float acceleration = TwoDphysicsComponent.acceleration;
        // float targetSpeed = TwoDphysicsComponent.targetSpeed;
        // float maxSpeed = TwoDphysicsComponent.maxSpeed;
        // float minSpeed = TwoDphysicsComponent.minSpeed;
        // float& speed = TwoDphysicsComponent.speed;
		float& heading = TwoDphysicsComponent.heading;
		float& targetHeading = TwoDphysicsComponent.targetHeading;
		// raylib::Quaternion& rotation = TwoDphysicsComponent.rotation;
        // raylib::Quaternion& targetRotation = TwoDphysicsComponent.targetRotation;

        float target = Clamp(targetSpeed, minSpeed, maxSpeed);
        if (speed < target)
            speed += acceleration * dt;
        else if (speed > target)
            speed -= acceleration * dt;
        speed = Clamp(speed, minSpeed, maxSpeed);
		
        targetHeading = AngleClamp(targetHeading);
        float difference = abs(targetHeading - heading);
        if (targetHeading > heading) {
            if (difference < 180)
                heading += angularAcceleration * dt;
            else if (difference > 180)
                heading -= angularAcceleration * dt;
        } else if (targetHeading < heading) {
            if (difference < 180)
                heading -= angularAcceleration * dt;
            else if (difference > 180)
                heading += angularAcceleration * dt;
        }
        if (difference < .5)
            heading = targetHeading;

        // Calculate velocity based on updated speed and heading
        heading = AngleClamp(heading);
        float angle = raylib::Degree(heading); // convert heading

        // Initial velocity 
		raylib::Vector3 velocity;
        velocity.x = speed * cos(angle);
        velocity.y = 0;
        velocity.z = -speed * sin(angle);
	}
}















// need a 2d physics struct with vvector 3 and that needs is own input and its own system







// void TwoDPhysicsSystem(Scene<ComponentStorage>& scene, float dt) {
    
    
//     // Iterate over all entities in the scene
//     for (Entity e = 0; e < scene.entityMasks.size(); e++) {

//         static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
//             float decimal = float(angle) - int(angle);
//             int whole = int(angle) % 360;
//             whole += (whole < 0) * 360;
//             return decimal + whole;
//         };

//         std::cout << "i have physics " << std::endl;
//         if (!scene.HasComponent<transformcomp>(e)) continue;
//         if (!scene.HasComponent<physics>(e)) continue;
//         std::cout << "i am in physics " << std::endl;
//         // Get references to the transform and physics components
//         auto& transformComponent = scene.GetComponent<transformcomp>(e);
//         auto& physicsComponent = scene.GetComponent<physics>(e); // Declare physicsComponent here

//         // Calculate velocity based on physics parameters
//         float acceleration = physicsComponent.acceleration;
//         float angularAcceleration = physicsComponent.angularAcceleration;
//         float targetSpeed = physicsComponent.targetSpeed;
//         raylib::Degree targetHeading = physicsComponent.targetHeading;
//         float maxSpeed = physicsComponent.maxSpeed;
//         float minSpeed = physicsComponent.minSpeed;
//         float& speed = physicsComponent.speed;
//         raylib::Degree& heading = physicsComponent.heading;
//         // float dt = physicsComponent.dt;

        
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

//         // Calculate velocity based on updated speed and heading
//         heading = AngleClamp(heading);
//         float angle = raylib::Degree(heading); // convert heading
		
//         raylib::Vector3 velocity;
//         velocity.x = speed * cos(angle * DEG2RAD * 2);
//         velocity.y = 0;
//         velocity.z = -speed * sin(angle * DEG2RAD * 2);


//         std::cout << dt << " <---  dt from the physics." << std::endl;
//         std::cout << angle << " <---  angle from the physics." << std::endl;
//         std::cout << speed << " <---  speed from the physics." << std::endl;
//         std::cout << heading << " <---  heading from the physics." << std::endl;

//         // Update position based on velocity
//         transformComponent.position.x += velocity.x * dt;
//         transformComponent.position.y += velocity.y * dt;
//         transformComponent.position.z += velocity.z * dt;

//         transformComponent.rotation.y = heading;
// 		transformComponent.rotation.w = 1;

//     }
// }


// Physics System
// void TwoDPhysicsSystem(Scene& scene) {
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