#ifndef PHYSICSCOMPONENT_HPP
#define PHYSICSCOMPONENT_HPP

#include "component.hpp"
#include "entity.hpp" 
#include "transformComponent.hpp" 
#include <iostream>


struct PhysicsComponent : public Component {
    raylib::Vector3 velocity = {0, 0, 0};
    float speed = 5;
    float heading = 0; 

    PhysicsComponent(Entity& entity, const raylib::Vector3& initialVelocity = {0, 0, 0}, float initialSpeed = 0, float HEADING= 0 )
        : Component(entity), velocity(initialVelocity), speed(initialSpeed), heading(HEADING){}

    void tick(float dt) override {
    auto ref = object->GetComponent<TransformComponent>(); // Get reference to transform component
    if (!ref) return;

    auto& transform = ref->get(); // Get the actual transform component
    auto [axis, angle] = transform.rotation.ToAxisAngle(); // gets quaternion
    
  
    raylib::Quaternion rotationDelta = QuaternionFromAxisAngle(raylib::Vector3{0, 1, 0}, heading * dt);
    transform.rotation = rotationDelta * transform.rotation;
    
  
    velocity.x = speed * cos(heading);
    velocity.z = -speed * sin(heading);

    transform.position.x += velocity.x * dt;
    transform.position.y += velocity.y * dt;
    transform.position.z += velocity.z * dt;
}

};

#endif 
