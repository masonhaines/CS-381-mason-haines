#ifndef PHYSICSCOMPONENT_HPP
#define PHYSICSCOMPONENT_HPP

#include "component.hpp"
#include "entity.hpp" 
#include "transformComponent.hpp" 
#include <iostream>


struct PhysicsComponent : public Component {
    raylib::Vector3 velocity = {0, 0, 0};
    float speed;
    float targetSpeed;
    raylib::Degree targetHeading;
    raylib::Degree heading = 0; 

    int minSpeed = 0;
    int maxSpeed = 25;

    static constexpr float acceleration = 10;
    static constexpr float angularAcceleration = 15;



    PhysicsComponent(Entity& entity, const raylib::Vector3& initVELO = {0, 0, 0}, float initSPEED = 0, float HEADING = 0, float targSpeed = 0, float targHEADING = 0, int MAX = 50, int MIN = 0)
        : Component(entity), velocity(initVELO), speed(initSPEED), heading(HEADING), targetHeading(targHEADING), targetSpeed(targSpeed), maxSpeed(MAX), minSpeed(MIN) {}

    void tick(float dt) override {

        static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
            int intPart = angle;
            float floatPart = float(angle) - intPart;
            intPart %= 360;
            intPart += (intPart < 0) * 360;
            return intPart + floatPart;
        };


        auto ref = object->GetComponent<TransformComponent>(); // get optional reference to transform component 
        if (!ref) return; // does it exist 
        auto& transform = ref->get(); // get values stored in reference if it exists

        float target = Clamp(targetSpeed, 0, 50);
        if(speed < target) speed += acceleration * dt;
        else if(speed > target) speed -= acceleration * dt;
        if (speed > maxSpeed) speed = maxSpeed;
        else if (speed < minSpeed) speed = minSpeed;
        // speed = Clamp(speed, minSpeed, maxSpeed);

        std::cout << speed << "<< speed >> " << std::endl;
        

        target = AngleClamp(targetHeading);
        int difference = abs(target - heading);
        if(target > heading) {
            if(difference < 180) heading += angularAcceleration * dt;
            else if(difference > 180) heading -= angularAcceleration * dt;
        } else if(target < heading) {
            if(difference < 180) heading -= angularAcceleration * dt;
            else if(difference > 180) heading += angularAcceleration * dt;
        } 
        if(difference < .5) heading = target; // If the heading is really close to correct 
        heading = AngleClamp(heading);
        float angle = raylib::Degree(heading);



        velocity.x = speed * cos(angle * DEG2RAD * 2);
        velocity.z = -speed * sin(angle * DEG2RAD * 2);

        // Update position based on velocity
        transform.position.x += velocity.x * dt;
        transform.position.y += velocity.y * dt;
        transform.position.z += velocity.z * dt;

        transform.rotation.y = angle * DEG2RAD *2;
        // transform.rotation.x = -.1* angle * DEG2RAD;

        // Debug output
        std::cout << "PhysicsComponent tick function called." << std::endl;
        // std::cout << "Position: (" << transform.position.x << ", " << transform.position.y << ", " << transform.position.z << ")" << std::endl;
        std::cout << "axis: (" << transform.rotation.x << ", " << transform.rotation.y << ", " << transform.rotation.z << ")" << std::endl;
        // std::cout << "Velocity mag: (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;
    }
};

#endif 