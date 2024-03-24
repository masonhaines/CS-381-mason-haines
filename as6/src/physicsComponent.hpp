#ifndef PHYSICSCOMPONENT_HPP
#define PHYSICSCOMPONENT_HPP

#include "component.hpp"
#include "entity.hpp" 
#include "transformComponent.hpp" 
// #include <iostream>


struct PhysicsComponent : public Component {
    raylib::Vector3 velocity = {0, 0, 0};
    raylib::Vector3 offsetROT = {0, 0, 0};

    float speed;
    float targetSpeed;

    float acceleration = 10;
    float angularAcceleration = 15;

    int minSpeed = 0;
    int maxSpeed = 50;

    raylib::Degree targetHeading;
    raylib::Degree heading = 0; 
    
    //Default Constructor
    PhysicsComponent(
        Entity& entity,
        const raylib::Vector3& OFFSET  = {0, 0, 0}, 
        const raylib::Vector3& initVELO = {0, 0, 0}, 
        float initSPEED = 0, 
        float targSpeed = 0,
        float ACC = 10, 
        float angAcc = 15, 
        int MIN = 0, 
        int MAX = 50
    )
        : Component(entity), offsetROT(OFFSET), velocity(initVELO), speed(initSPEED), targetSpeed(targSpeed), 
        acceleration(ACC), angularAcceleration(angAcc), minSpeed(MIN), maxSpeed(MAX){}
    
    void tick(float dt) override {

        static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
            int intPart = angle;
            float floatPart = float(angle) - intPart;
            intPart %= 360;
            intPart += (intPart < 0) * 360;
            return intPart + floatPart;
        };

        float target = Clamp(targetSpeed, minSpeed, maxSpeed); // Restricting target speed range
        if (speed < targetSpeed) {
            speed += acceleration * dt;
        } else if (speed > targetSpeed) {
            speed -= acceleration * dt;
        }

        // std::cout << "speed: " << speed << std::endl;
        // std::cout << "targetSpeed: " << targetSpeed << std::endl;
        // std::cout << "minSpeed: " << minSpeed << std::endl;
        // std::cout << "maxSpeed: " << maxSpeed << std::endl;
        // std::cout << "acceleration: " << acceleration << std::endl;
        // std::cout << "angularAcceleration: " << angularAcceleration << std::endl;
        // std::cout << "targetHeading: " << targetHeading << std::endl;
        // std::cout << "heading: " << heading << std::endl;
        // Ensure speed stays within bounds
        speed = Clamp(speed, minSpeed, maxSpeed); // Restricting speed range
        
        target = AngleClamp(targetHeading);
        int difference = abs(target - heading);
        if(target > heading) {
            if(difference < 180) heading += angularAcceleration * dt;
            else if(difference > 180) heading -= angularAcceleration * dt;
        } else if(target < heading) {
            if(difference < 180) heading -= angularAcceleration * dt;
            else if(difference > 180) heading += angularAcceleration * dt;
        } 
        heading = AngleClamp(heading);
        float angle = raylib::Degree(heading);



        velocity.x = speed * cos(angle * DEG2RAD * 2);
        velocity.y = 0;
        velocity.z = -speed * sin(angle * DEG2RAD * 2);

         auto ref = object->GetComponent<TransformComponent>(); // get optional reference to transform component 
        if (!ref) return; // does it exist 
        auto& transform = ref->get(); // get values stored in reference if it exists

        // Update position based on velocity
        transform.position.x += velocity.x * dt;
        transform.position.y += velocity.y * dt;
        transform.position.z += velocity.z * dt;

        transform.rotation.y = (angle * DEG2RAD *2) + offsetROT.y;

        // // Debug output
        // std::cout << "PhysicsComponent tick function called." << std::endl;
        // // std::cout << "Position: (" << transform.position.x << ", " << transform.position.y << ", " << transform.position.z << ")" << std::endl;
        // std::cout << "axis: (" << transform.rotation.x << ", " << transform.rotation.y << ", " << transform.rotation.z << ")" << std::endl;
        // // std::cout << "Velocity mag: (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;
    }
};

#endif 