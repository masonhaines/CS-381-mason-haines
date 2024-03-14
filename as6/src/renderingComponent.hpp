#ifndef RENDERINGCOMPONENT_HPP
#define RENDERINGCOMPONENT_HPP

#include "component.hpp" 
#include "entity.hpp" 
#include "transformComponent.hpp" 

struct RenderingComponent : public Component{
    raylib::Model model;

    RenderingComponent(Entity& e, raylib::Model&& model = {}) : Component(e), model(std::move(model)) {}
    
    void tick(float dt) override {
        auto ref = object->GetComponent<TransformComponent>(); // get optional reference to transform component 
        if (!ref) return; // does it exist 
        auto& transform = ref->get(); // get values stored in reference if it exists

        auto [axis, angle] = transform.rotation.ToAxisAngle(); // gets quaternion
        // model.Draw(transform.position, axis ,angle);
        model.Draw(transform.position, axis ,angle, transform.scale, transform.shade);
    }
};

#endif
