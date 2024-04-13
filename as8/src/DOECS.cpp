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
	const int screenWidth = 400 * 2; // 900
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

	int numberOfPlanes = 1;
	int counter = -1;

    raylib::Model plane("meshes/PolyPlane.glb");
    Scene scene;
 
    // Add a transform component to the entity
//    for (int i = 0; i < numberOfPlanes; ++i) {
		// auto e = scene.CreateEntity();

		// scene.AddComponent<Rendering>(e) = {                                          //1
		// &plane, 
		// false}; // Plane with no bounding box, ie false 

		// scene.AddComponent<transformcomp>(e) = {                                          //2
		// (Vector3){1 * 100.0f - 200, 90, 0}, 
		// (Vector3){2,2,2}, 
		// QuaternionIdentity()}; // Adjust position based on 'i'

		// scene.AddComponent<physics>(e) = {                                          //3
		// 8, 
		// QuaternionIdentity(), QuaternionIdentity()};

		// scene.AddComponent<veloKinematics>(e) = {                                          //4
		// 5, 
		// (Vector3){0, 0, 0}, 
		// 5, 
		// 5, 
		// 50, 
		// 0};
		
		// scene.AddComponent<bufferedComponent>(e) = {                                          //5
		// &inputs, 
		// false};
	// }
	
	auto b1 = scene.CreateEntity();
	

	scene.AddComponent<Rendering>(b1) = {
	&plane, 
	false}; // Plane with no bounding box, ie false 

	scene.AddComponent<transformcomp>(b1) = {
	(Vector3){.0, 50, 0}, 
	(Vector3){2,2,2}, 
	QuaternionIdentity()}; // Adjust position based on 'i'

	scene.AddComponent<TwoDphysics>(b1) = {
	35, 
	0.0f, 0.0f};

	// scene.AddComponent<physics>(b1) = {                                          //3
	// 	8, 
	// 	QuaternionIdentity(), QuaternionIdentity()};

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
				// skybox.Draw();

				// ground.Draw({});

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


void BoatProcessInputSystem(Scene<ComponentStorage>& scene) {

	// bool inputting = false;

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
        (*buffer.inputs)["right"].AddPressedCallback([&TwoDPhysicsComponent, &buffer]()-> void { //lambda function that is creating call back for action 
            if (buffer.selected)
			TwoDPhysicsComponent.targetHeading -= 10 * DEG2RAD;
			// std::cout << "I DO NOT HAVE 2 D PHYSICS" << std::endl;
        });
        (*buffer.inputs)["left"].AddPressedCallback([&TwoDPhysicsComponent, &buffer]()-> void { //lambda function that is creating call back for action 
            if (buffer.selected)
			TwoDPhysicsComponent.targetHeading += 10 * DEG2RAD;
			// std::cout << "I DO NOT HAVE 2 D PHYSICS" << std::endl;
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

	static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
			int intPart = angle;
			float floatPart = float(angle) - intPart;
			intPart %= 360;
			intPart += (intPart < 0) * 360;
			return intPart + floatPart;
		};

	for (Entity e = 0; e < scene.entityMasks.size(); e++) {

		

        if (!scene.HasComponent<transformcomp>(e)) continue;
        if (!scene.HasComponent<TwoDphysics>(e)) continue;
		if (!scene.HasComponent<veloKinematics>(e)) continue;

        auto& transformComponent = scene.GetComponent<transformcomp>(e);
        auto& TwoDphysicsComponent = scene.GetComponent<TwoDphysics>(e);
		auto& kinematics = scene.GetComponent<veloKinematics>(e); 


		raylib::Vector3& velocity = kinematics.velocity;
		float& speed = kinematics.speed;
        // Update speed based on TwoDphysics parameters

		float angularAcceleration = TwoDphysicsComponent.angularAcceleration;
		float& heading = TwoDphysicsComponent.heading;
		float& targetHeading = TwoDphysicsComponent.targetHeading;
		
        // targetHeading = AngleClamp(targetHeading);
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
        if (difference < .5) heading = targetHeading;

		std::cout << heading << " <---  heading ." << std::endl;
		std::cout << targetHeading << " <---  targetHeading ." << std::endl;
		// std::cout << speed << " <---  speed from the physics." << std::endl;
        // Calculate velocity based on updated speed and heading
        // heading = AngleClamp(heading);
        raylib::Radian angle = raylib::Degree(heading); // convert heading

		raylib::Quaternion rotation = raylib::Quaternion::FromAxisAngle({0, 1, 0}, heading);
		

		velocity = raylib::Vector3::Left().RotateByQuaternion(rotation) * speed;



		transformComponent.rotation = rotation;

		std::cout << transformComponent.rotation.x << " <---  x rotation from the physics." << std::endl;
        std::cout << transformComponent.rotation.y << " <---  y rotation from the physics." << std::endl;
        std::cout << transformComponent.rotation.z << " <---  z rotation from the physics." << std::endl;
		std::cout << transformComponent.rotation.w << " <---  w rotation from the physics." << std::endl;
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