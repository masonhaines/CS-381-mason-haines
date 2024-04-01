//  3/11 goes over component storages 

// can only store one type of thing 
// #include "src/transformComponent.hpp"
#include <raylib-cpp.hpp>
#include <cstddef>
#include <cassert>
#include <cstdint>
#include <vector>
#include <limits>
// #include <ranges>
// #include <memory>

using Entity = uint8_t;

struct SkipListComponentStorage {
    size_t elementSize = -1;
    std::vector<uint16_t> indicies;
    std::vector<std::byte> data; // buffer that is 4 bytes long 

    SkipListComponentStorage() : elementSize(-1), data(1, std::byte{0}) {};
    SkipListComponentStorage (size_t elementSize) : elementSize(elementSize) {data.resize(5 * elementSize);}

    template<typename Tcomponent> 
    SkipListComponentStorage(Tcomponent reference = {}) : SkipListComponentStorage(sizeof(Tcomponent)) {}

    template<typename Tcomponent> 
    Tcomponent& Get(Entity e) {
        assert(sizeof(Tcomponent) == elementSize);
        // assert(e < (data.size() / elementSize));
        assert(e < indicies.size());
        assert(indicies[e] != std::numeric_limits<uint16_t>::max());
        return *(Tcomponent*)(data.data() + e * elementSize);
    }

private:
    template<typename Tcomponent> 
    std::pair<Tcomponent&, Entity> Allocate() {
        assert(sizeof(Tcomponent) == elementSize);

        auto originalEnd = data.size();
        data.insert(data.end(), elementSize, std::byte{0});
        return {
            *new(data.data() + data.size() - elementSize) Tcomponent(), data.size() / elementSize
        };

    }
public:
    template<typename Tcomponent> 
    std::pair<Tcomponent&, Entity> Allocate(Entity e) {
        auto[ret, i] = Allocate<Tcomponent>();

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
    std::vector<SkipListComponentStorage> storages = {SkipListComponentStorage()};

    template<typename Tcomponent>
    SkipListComponentStorage& GetStorage() {
        size_t id = getComponentID<Tcomponent>();
        if (storages.size() <= id) {
            storages.insert(storages.cend(), std::max<int64_t>(id - storages.size(), 1));
        }
        if (storages[id].elementSize == std::numeric_limits<size_t>::max()) 
            storages[id] = SkipListComponentStorage(Tcomponent{});
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
    raylib::Vector3 rotation;
};

template<typename T>
concept Transformer = requires( T t, raylib::Transform m) {
    {t.operator()(m)}->std::convertible_to<raylib::Transform>;
};

void DrawSystem(Scene& scene) {

    raylib::Color color;

    for(Entity e = 0; e < scene.entityMasks.size(); e++) {
        if(!scene.hasComponent<Rendering>(e)) continue;

        auto & rendering = scene.getComponent<Rendering>(e);
        raylib::Transform backupTransform = rendering.model->transform; // Save the model's transform

        

        // it's up to you to implement that transformer given a transform component that you write.
        rendering.model->transform = Transformer<Rendering>();

        rendering.model->Draw({});//Default position, scale, color-tint
        rendering.model->transform = backupTransform; // Restore the original transform
    }
}