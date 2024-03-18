#ifndef PHYSICSCOMPONENT_HPP
#define PHYSICSCOMPONENT_HPP

#include "component.hpp"
#include "entity.hpp" 
#include "transformComponent.hpp" 
#include <iostream>


struct PhysicsComponent : public Component {
    raylib::Vector3 velocity = {0, 0, 0};
    float speed = 5;
    // float heading = 0; 

    PhysicsComponent(Entity& entity, const raylib::Vector3& initialVelocity = {0, 0, 0}, float initialSpeed = 0, float HEADING = 0 )
        : Component(entity), velocity(initialVelocity), speed(initialSpeed){}

    void tick(float dt) override {
        auto ref = object->GetComponent<TransformComponent>(); // get optional reference to transform component 
        if (!ref) return; // does it exist 
        auto& transform = ref->get(); // get values stored in reference if it exists

        velocity.x = speed * cos(transform.heading);
        velocity.z = -speed * sin(transform.heading);

        // Update position based on velocity
        transform.position.x += velocity.x * dt;
        transform.position.y += velocity.y * dt;
        transform.position.z += velocity.z * dt;

        // Debug output
        std::cout << "PhysicsComponent tick function called." << std::endl;
        std::cout << "Position: (" << transform.position.x << ", " << transform.position.y << ", " << transform.position.z << ")" << std::endl;
        std::cout << "Position: (" << transform.rotation.x << ", " << transform.rotation.y << ", " << transform.rotation.z << ")" << std::endl;
    }

};

#endif 
