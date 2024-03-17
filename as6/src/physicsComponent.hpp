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

    PhysicsComponent(Entity& entity, const raylib::Vector3& initialVelocity = {0, 0, 0}, float initialSpeed = 0, float HEADING = 0 )
        : Component(entity), velocity(initialVelocity), speed(initialSpeed), heading(HEADING){}

    void tick(float dt) override {
        auto ref = object->GetComponent<TransformComponent>(); // get optional reference to transform component 
        if (!ref) return; // does it exist 
        auto& transform = ref->get(); // get values stored in reference if it exists

        auto [axis, angle] = transform.rotation.ToAxisAngle(); // gets quaternion
        
        std::cout << "Axis: (" << axis.x << ", " << axis.y << ", " << axis.z << "), Angle: " << angle << std::endl;
    
        velocity.x = speed * cos(heading);
        velocity.z = -speed * sin(heading);
        
        std::cout << "Velocity: (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;

        transform.position.x += velocity.x * dt;
        transform.rotation.w += velocity.x ;
        transform.position.y += velocity.y * dt;
        transform.position.z += velocity.z * dt;
        transform.rotation = raylib::Quaternion::FromAxisAngle({0, 1, 0}, 180 * DEG2RAD);

        // transform.rotation.y += 50;
        // transform.rotation.z += 50;
        // transform.rotation.w ++;
        std::cout << "New Position: (" << transform.position.x << ", " << transform.position.y << ", " << transform.position.z << ")" << std::endl;

        raylib::Quaternion rotationDelta = raylib::Quaternion::FromAxisAngle(raylib::Vector3{0, 1, 0}, heading * DEG2RAD * dt);
        transform.rotation = rotationDelta * transform.rotation;

        // Debug output
        std::cout << "PhysicsComponent tick function called." << std::endl;
        std::cout << "Heading: " << heading << std::endl;
    }


};

#endif 
