#ifndef PHYSICSCOMPONENT_HPP
#define PHYSICSCOMPONENT_HPP

#include "component.hpp"
#include "entity.hpp" 
#include "transformComponent.hpp" 


struct PhysicsComponent : public Component {
    raylib::Vector3 velocity = {0, 0, 0};
    float speed = 5;
    // float heading = 0; 

    PhysicsComponent(Entity& entity, const raylib::Vector3& initialVelocity = {0, 0, 0}, float initialSpeed = 0)
        : Component(entity), velocity(initialVelocity), speed(initialSpeed) {}

    void tick(float dt) override {
        auto ref = object->GetComponent<TransformComponent>(); // get optional reference to transform component 
        if (!ref) return; // does it exist 
        auto& transform = ref->get(); // get values stored in reference if it exists

        auto [axis, angle] = transform.rotation.ToAxisAngle(); // gets quaternion
        raylib::Vector3 velocity3D = {cos(angle) * speed, velocity.y, -sin(angle) * speed}; // Compute 3D velocity based on heading
        // Update position
        transform.position.x += velocity3D.x * dt; 
        transform.position.y += velocity3D.y * dt;
        transform.position.z += velocity3D.z * dt;
    }
};

#endif // PHYSICSCOMPONENT_HPP
