



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

using Entity = uint8_t;


struct ComponentStorage {
    size_t elementSize = -1;
    std::vector<std::byte> data; // buffer that is 4 bytes long 

    ComponentStorage() : elementSize(-1), data(1, std::byte{0}) {};
    ComponentStorage (size_t elementSize) : elementSize(elementSize) {data.resize(5 * elementSize);}

    template<typename Tcomponent> 
    ComponentStorage(Tcomponent reference = {}) : ComponentStorage(sizeof(Tcomponent)) {}

    template<typename Tcomponent> 
    Tcomponent& Get(Entity e) {
        assert(sizeof(Tcomponent) == elementSize);
        assert(e < (data.size() / elementSize));
        return *(Tcomponent*)(data.data() + e * elementSize);
    }

    template<typename Tcomponent> 
    std::pair<Tcomponent&, Entity> Allocate(size_t count = 1) {
        assert(sizeof(Tcomponent) == elementSize);
        assert (count < 255);
        auto originalEnd = data.size();
        data.insert(data.end(), elementSize * count, std::byte{0});
        for (size_t i = 0; i < count - 1; i ++) 
            new(data.data() + originalEnd + i * elementSize) Tcomponent();
        return {
            *new(data.data() + data.size() - elementSize) Tcomponent(), data.size() / elementSize
        };

    }

    template<typename Tcomponent> 
    Tcomponent& GetOrAllocate(Entity e) {
        assert(sizeof(Tcomponent) == elementSize);
        size_t size = data.size() / elementSize;
        if (size <= e) {
            Allocate<Tcomponent>(std::max<int64_t>(int64_t(e) - size, 1));
        }
        return Get<Tcomponent>(e);
    }
};

extern size_t globalComponentCounter;

template<typename T> // mapping a type to an integer 
size_t getComponentID(T reference = {}) {
    static size_t id = globalComponentCounter++;
    return id;
}

struct Scene {
    std::vector<std::vector<bool>> entityMasks;
    std::vector<ComponentStorage> storages = {ComponentStorage()};

    template<typename Tcomponent>
    ComponentStorage& GetStorage() {
        size_t id = getComponentID<Tcomponent>();
        if (storages.size() <= id) {
            storages.insert(storages.cend(), std::max<int64_t>(id - storages.size(), 1));
        }
        if (storages[id].elementSize == std::numeric_limits<size_t>::max()) 
            storages[id] = ComponentStorage(Tcomponent{});
        return storages[id];
    }

    Entity CreateEntity( ) {
        Entity e = entityMasks.size();
        entityMasks.emplace_back(std::vector<bool>{false});
        return e;

    }

    template< typename Tcomponent> 
    Tcomponent& AddComponent(Entity e) {
        size_t id = getComponentID<Tcomponent>();
        auto& eMask = entityMasks[e];
        
        if (eMask.size() <= id) {
            eMask.resize(id + 1, false);
        }
        eMask[id] = true;
        return GetStorage<Tcomponent>().template GetOrAllocate<Tcomponent>(e);
    }

    template<typename Tcomponent>
    void RemoveComponent(Entity e) {
        size_t id = getComponentID<Tcomponent>(); // Get ID
        auto& eMask = entityMasks[e]; // Access the entity mask for the given entity
        
        if (eMask.size() > id) { // if amount of masks is greater than number of components
            eMask[id] = false; // Set entity mask at component index to false, swap bit set 
        }
    }

    template<typename Tcomponent>
    Tcomponent& getComponent(Entity e) {
        size_t id = getComponentID<Tcomponent>();
        assert(entityMasks[e][id]);
        return GetStorage<Tcomponent>().template Get<Tcomponent>(e);
    }

    template<typename Tcomponent>
    bool hasComponent(Entity e) {
        size_t id = getComponentID<Tcomponent>();
        return entityMasks.size() > e && entityMasks[e].size() > id && entityMasks[e][id];
    }

};

struct Rendering {
    raylib::Model* model;
    bool drawBoundingBox = false;
    
};

struct transform {
    raylib::Vector3 position;
    raylib::Vector3 scale;
    // raylib::Vector3 rotation;
};

template<typename T>
concept Transformer = requires( T t, raylib::Transform m) {
    {t.operator()(m)}->std::convertible_to<raylib::Transform>;
};

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

void DrawSystem(Scene& scene) {

    raylib::Color color;

    for(Entity e = 0; e < scene.entityMasks.size(); e++) {
        if(!scene.hasComponent<Rendering>(e)) continue;
        if(!scene.hasComponent<transform>(e)) continue;
        auto & rendering = scene.getComponent<Rendering>(e);
        auto & transformComponent = scene.getComponent<transform>(e);

        auto Transformer = [&transformComponent](raylib::Transform t) -> raylib::Transform {
            t = MatrixTranslate(transformComponent.position.x, transformComponent.position.y, transformComponent.position.z);

            return t;
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
	raylib::Texture water("textures/water.jpg");
	water.SetFilter(TEXTURE_FILTER_BILINEAR);
	water.SetWrap(TEXTURE_WRAP_REPEAT);
	ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = water;

    raylib::Model plane("meshes/PolyPlane.glb");
    Scene scene;
    auto e = scene.CreateEntity();
    // Add a transform component to the entity
    scene.AddComponent<Rendering>(e) = {&plane, false}; // Plane with no bounding box, ie false 
    scene.AddComponent<transform>(e).position = raylib::Vector3 {1, 1, 1};
   
    
    

	raylib::Text text;
    float textSize = 25;
    const char *speed = {"Speed: "};
	const char *start = {"Press TAB begin simulation"};

	SetTargetFPS(60);

	// Main loop
	bool keepRunning = true;
	while(!window.ShouldClose() && keepRunning) {
		
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

                DrawSystem(scene);
				
			}
			camera.EndMode();

			int height = window.GetHeight();
        	int width = window.GetWidth();

			DrawFPS(10, 10);
		}
		window.EndDrawing();
	}

	return 0;
}