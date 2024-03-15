#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "component.hpp" 
#include "transformComponent.hpp" 
#include <optional>

struct Entity {
    std::vector<std::unique_ptr<Component>> components;

    Entity() { AddComponent<TransformComponent>(); }
    Entity(const Entity&) = delete;

    Entity(Entity&& other) : components(std::move(other.components)) {
        for (auto& c :components) {
            c->object = this;
        }
    }

    template<std::derived_from<Component> T, typename... Ts>
    size_t AddComponent(Ts... args) {
        auto c = std::make_unique<T>(*this, std::forward<Ts>(args)...);
        // components.back()->object = this;
        components.emplace_back(std::move(c));
        return components.size() - 1;
    }

    // template<std::derived_from<Component> T, typename... Ts>
    // size_t AddComponent(Ts... args) {
    //     auto c = std::make_unique<T>(*this, std::forward<Ts>(args)...);
    //     c->object = this; // Set the object pointer
    //     components.push_back(std::move(c)); // Push the new component
    //     return components.size() - 1;
    // }


    template<std::derived_from<Component> T> 
    std::optional<std::reference_wrapper<T>> GetComponent() {

        if constexpr(std::is_same_v<T, TransformComponent>) {
            T* cast = dynamic_cast<T*>(components[0].get()); // dynamic cast means if pointer can be converted, convert it 
            if (cast) return *cast;
        }

        for(auto& c : components) {
            T* cast = dynamic_cast<T*>(c.get()); // dynamic cast means if pointer can be converted, convert it 
            if (cast) return *cast;
        }

        return std::nullopt; // nullptr for options 
    }

    void tick (float dt) {
        for (auto& c : components) {
            c->tick(dt);
        }
    }

    void setup() {
        for (auto& c : components) {
            c->setup();
        }
    }

    // void cleanup() {
    //     for (auto& c : components) {
    //         c->cleanup();
    //     }
    // }
};

#endif
